#!/usr/bin/env python3
"""
Test de conformité et de cohérence statique du firmware Light Trainer V3.
Exécuté automatiquement en CI/CD (GitHub Actions) sur chaque Pull Request.
"""

import os
import re
import gzip
import unittest

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
V3_DIR = os.path.join(REPO_ROOT, "5_Programmation", "XIAO-esp32-v3", "main")

class TestFirmwareSanity(unittest.TestCase):

    def test_dashboard_gzip_integrity(self):
        """Vérifie que DashboardHtml.h est valide et se décompresse correctement."""
        header_path = os.path.join(V3_DIR, "DashboardHtml.h")
        self.assertTrue(os.path.exists(header_path), "DashboardHtml.h introuvable")

        with open(header_path, "r", encoding="utf-8") as f:
            content = f.read()

        # Extraire les octets hexadécimaux
        hex_values = re.findall(r"0x([0-9A-Fa-f]{2})", content)
        self.assertGreater(len(hex_values), 1000, "DashboardHtml.h semble trop petit")

        raw_bytes = bytes([int(h, 16) for h in hex_values])

        # Vérifier le header GZIP (0x1f, 0x8b)
        self.assertEqual(raw_bytes[0], 0x1f, "Signature GZIP magique invalide (byte 0)")
        self.assertEqual(raw_bytes[1], 0x8b, "Signature GZIP magique invalide (byte 1)")

        # Décompresser le HTML
        decompressed_html = gzip.decompress(raw_bytes).decode("utf-8")

        self.assertIn("<!DOCTYPE html>", decompressed_html)
        self.assertIn("<title>Light Trainer</title>", decompressed_html)
        self.assertIn("/api/config", decompressed_html, "Route /api/config absente du JS")
        self.assertIn("/api/status", decompressed_html, "Route /api/status absente du JS")
        self.assertIn("/api/pods", decompressed_html, "Route /api/pods absente du JS")
        self.assertIn("pollStatus", decompressed_html)
        self.assertIn("fetchConfig", decompressed_html)
        self.assertIn("/update", decompressed_html, "Route /update absente du JS")
        self.assertIn("checkGitHubUpdate", decompressed_html, "Fonction checkGitHubUpdate absente")
        self.assertIn("api.github.com/repos/Forestierr/Light_trainer", decompressed_html, "URL GitHub API absente")
        self.assertIn("subpageOtaDetail", decompressed_html, "Sous-page OTA absente")
        print(f"\n[OK] DashboardHtml.h validé : {len(raw_bytes)} octets GZIP -> {len(decompressed_html)} octets HTML décompressés")

    def test_config_pins_and_protocol(self):
        """Vérifie l'absence de conflits de broches matérielles et la validité du protocole."""
        config_path = os.path.join(V3_DIR, "Config.h")
        self.assertTrue(os.path.exists(config_path), "Config.h introuvable")

        with open(config_path, "r", encoding="utf-8") as f:
            content = f.read()

        # Vérifier les messages obligatoires du protocole
        required_cmds = [
            "CMD_START", "CMD_ACTIVATE", "CMD_HIT", "CMD_PING", 
            "CMD_ACK", "CMD_RETURN_TO_MENU", "CMD_MANUAL_SET", 
            "CMD_TEST_LEDS", "CMD_POD_TELEMETRY", "CMD_HEARTBEAT", 
            "CMD_CLAIM_MASTER"
        ]
        for cmd in required_cmds:
            self.assertIn(cmd, content, f"Commande obligatoire manquante : {cmd}")

        # Vérifier les constantes de watchdog Master
        self.assertIn("MASTER_HEARTBEAT_INTERVAL_MS", content)
        self.assertIn("MASTER_TIMEOUT_MS", content)
        self.assertIn("BOOT_ELECTION_WINDOW_MS", content)

        # Vérifier la validité des broches matérielles (pas de doublons de GPIO)
        pins = re.findall(r"#define\s+(\w+_PIN)\s+(D\d+)", content)
        pin_map = {}
        for name, pin in pins:
            self.assertNotIn(pin, pin_map, f"Conflit de broche matériel : {name} et {pin_map.get(pin)} partagent {pin}")
            pin_map[pin] = name

        print(f"[OK] Broches matérielles vérifiées sans conflit : {pin_map}")

    def test_battery_math_thresholds(self):
        """Vérifie la logique des seuils de batterie."""
        config_path = os.path.join(V3_DIR, "Config.h")
        with open(config_path, "r", encoding="utf-8") as f:
            content = f.read()

        full = float(re.search(r"#define\s+BAT_VOLT_FULL\s+([\d\.]+)", content).group(1))
        low = float(re.search(r"#define\s+BAT_VOLT_LOW\s+([\d\.]+)", content).group(1))
        empty = float(re.search(r"#define\s+BAT_VOLT_EMPTY\s+([\d\.]+)", content).group(1))

        self.assertGreater(full, low, "BAT_VOLT_FULL doit être strictement supérieur à BAT_VOLT_LOW")
        self.assertGreater(low, empty, "BAT_VOLT_LOW doit être strictement supérieur à BAT_VOLT_EMPTY")
        print(f"[OK] Seuils LiPo cohérents : Full={full}V, Low={low}V, Empty={empty}V")

    def test_firmware_version_semver(self):
        """Vérifie que la version du firmware est définie et respecte le Semantic Versioning (X.Y.Z)."""
        config_path = os.path.join(V3_DIR, "Config.h")
        with open(config_path, "r", encoding="utf-8") as f:
            content = f.read()

        match = re.search(r'#define\s+FIRMWARE_VERSION\s+"([^"]+)"', content)
        self.assertIsNotNone(match, "FIRMWARE_VERSION non trouvé dans Config.h")
        version = match.group(1)
        self.assertTrue(re.match(r"^\d+\.\d+\.\d+$", version), f"Version '{version}' ne respecte pas le format SemVer X.Y.Z")

        # Vérifier que WebConfig.cpp référence FIRMWARE_VERSION
        webconfig_path = os.path.join(V3_DIR, "WebConfig.cpp")
        with open(webconfig_path, "r", encoding="utf-8") as f:
            wc_content = f.read()
        self.assertIn("FIRMWARE_VERSION", wc_content, "FIRMWARE_VERSION non exposé dans WebConfig.cpp")
        print(f"[OK] Version Firmware SemVer validée : v{version}")

    def test_platformio_and_partitions_integrity(self):
        """Vérifie la validité de platformio.ini et le support OTA."""
        pio_ini_path = os.path.join(REPO_ROOT, "platformio.ini")
        self.assertTrue(os.path.exists(pio_ini_path), "platformio.ini introuvable à la racine")

        with open(pio_ini_path, "r", encoding="utf-8") as f:
            pio_content = f.read()

        self.assertIn("seeed_xiao_esp32c3", pio_content)
        self.assertIn("min_spiffs.csv", pio_content)
        self.assertIn("5_Programmation/XIAO-esp32-v3/main", pio_content)
        print("[OK] platformio.ini validé avec configuration partition OTA (min_spiffs.csv)")

if __name__ == "__main__":
    unittest.main()
