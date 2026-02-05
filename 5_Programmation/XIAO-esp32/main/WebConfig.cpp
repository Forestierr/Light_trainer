#include "WebConfig.h"
#include "Hardware.h"  // Pour le capteur de distance

WebConfigManager WebConfig;

// ---- INITIALISATION & DETECTION ----

void WebConfigManager::init() {}

bool WebConfigManager::shouldEnterConfigMode() {
  Hardware.init();
  DEBUG_PRINT(DEBUG_INFO, "Hold hand in front of sensor to enter config mode...");

  unsigned long start = millis();
  bool detected = false;

  // On laisse 3 secondes à l'utilisateur au démarrage
  while (millis() - start < 3000) {
    int d = Hardware.getDistance();
    DEBUG_PRINT(DEBUG_VERBOSE, "Checking distance for config mode. Distance: %dmm", d);
    // Si main détectée entre 5cm et 15cm
    if (d > 50 && d < 150) {
      detected = true;
      Hardware.rainbowBlink(1, 75);  // Rainbow feedback = Mode Config
      break;
    }
    delay(10);
  }

  if (detected) {
    DEBUG_PRINT(DEBUG_INFO, "Config mode gesture detected!");
    // Clignotement de confirmation
    Hardware.rainbowBlink(3, 100); // Rainbow blink for confirmation
    return true;
  }

  Hardware.setLight(0);  // Eteindre si pas de config
  DEBUG_PRINT(DEBUG_INFO, "Config mode not triggered. Continuing to game mode.");
  return false;
}

void WebConfigManager::loadSettings(GameMode* modes, int count) {
  currentModes = modes;                    // Garder une référence
  preferences.begin("trainer-cfg", true);  // Lecture seule
  DEBUG_PRINT(DEBUG_INFO, "Loading settings from NVS...");

  for (int i = 0; i < count; i++) {
    String p = String(i);
    // On charge seulement si la clé existe, sinon on garde la valeur par défaut du code
    if (preferences.isKey(("c1_" + p).c_str())) {
      modes[i].color1 = preferences.getUInt(("c1_" + p).c_str());
      modes[i].color2 = preferences.getUInt(("c2_" + p).c_str());
      modes[i].delayMin = preferences.getUInt(("dmin_" + p).c_str());
      modes[i].delayMax = preferences.getUInt(("dmax_" + p).c_str());
      DEBUG_PRINT(DEBUG_VERBOSE, "Loaded mode %d: c1=%X, c2=%X, dmin=%d, dmax=%d", i, modes[i].color1, modes[i].color2, modes[i].delayMin, modes[i].delayMax);
    } else {
      DEBUG_PRINT(DEBUG_VERBOSE, "Key c1_%d not found in NVS. Using default for mode %d.", i, i);
    }
  }
  preferences.end();
}

void WebConfigManager::run() {
  DEBUG_PRINT(DEBUG_INFO, "Starting WebConfig mode...");
  // Set static IP for AP
  IPAddress apIP(192, 168, 4, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(apIP, apIP, subnet);

  // Démarrage du Point d'Accès
  WiFi.softAP("LightTrainer_Setup", "12345678");  // MDP simple

  IPAddress IP = WiFi.softAPIP();
  DEBUG_PRINT(DEBUG_INFO, "WebConfig AP Started. IP: %s", IP.toString().c_str());

  // Start DNS server
  dnsServer.start(53, "*", apIP);
  DEBUG_PRINT(DEBUG_INFO, "DNS server started on %s", apIP.toString().c_str());

  // Routes Web
  server.on("/", [this]() {
    handleRoot();
  });
  server.on("/save", HTTP_POST, [this]() {
    handleSave();
  });
  // Redirect all requests to IP
  server.onNotFound([this]() {
    handleRoot();
  });
  server.begin();

  // Boucle infinie dédiée à la config
  bool configModeActive = true;
  unsigned long handDetectedStartTime = 0;
  const unsigned long exitGestureDuration = 3000; // 3 seconds
  const int handDetectionDistance = 250; // Distance to detect hand for exit (e.g., <25cm)

  while (configModeActive) {
    dnsServer.processNextRequest(); // Process DNS requests
    server.handleClient();
    Hardware.rainbowAnimation(); // Continue rainbow effect

    int dist = Hardware.getDistance();
    if (dist != -1) { // Only process if we got a valid distance reading (not -1 for "not ready")
      if (dist > 70 && dist < handDetectionDistance) { // Hand is within detection range
        if (handDetectedStartTime == 0) {
          handDetectedStartTime = millis();
          DEBUG_PRINT(DEBUG_INFO, "Exit gesture started (distance: %dmm).", dist);
        } else if (millis() - handDetectedStartTime >= exitGestureDuration) {
          DEBUG_PRINT(DEBUG_INFO, "Exit gesture completed. Exiting WebConfig mode.");
          Hardware.blink(0xFFFFFF, 5); // White blink for exit confirmation
          configModeActive = false; // Exit the loop
        }
      } else { // Distance is valid but outside detection range (hand moved away, or too close/far)
        if (handDetectedStartTime != 0) {
          DEBUG_PRINT(DEBUG_INFO, "Exit gesture interrupted (distance: %dmm).", dist);
          handDetectedStartTime = 0; // Reset if hand is removed or out of range
        }
      }
    } else { // dist == -1 (sensor not ready, ignore this reading for gesture detection)
      DEBUG_PRINT(DEBUG_VERBOSE, "Distance sensor not ready, ignoring reading for exit gesture.");
      // Do nothing, keep handDetectedStartTime as is, to not interrupt
    }
    delay(2);  // Petit délai pour laisser respirer le CPU
  }

  DEBUG_PRINT(DEBUG_INFO, "WebConfig mode exited. Restarting ESP...");
  ESP.restart(); // Restart ESP to go back to game mode
}

// ---- PAGES WEB & HTML ----

void WebConfigManager::handleRoot() {
  DEBUG_PRINT(DEBUG_VERBOSE, "Handling root request for %s", server.uri().c_str());
  server.send(200, "text/html", getHTML());
}

void WebConfigManager::handleSave() {
  preferences.begin("trainer-cfg", false);  // Mode écriture
  DEBUG_PRINT(DEBUG_INFO, "Saving settings to NVS...");

  // On sauvegarde les 4 modes
  for (int i = 0; i < 4; i++) {
    String p = String(i);

    // Récupération des valeurs du formulaire
    String hex1 = server.arg("c1_" + p);
    String hex2 = server.arg("c2_" + p);
    String dmin = server.arg("dmin_" + p);
    String dmax = server.arg("dmax_" + p);

    DEBUG_PRINT(DEBUG_VERBOSE, "Saving mode %d: c1=%s, c2=%s, dmin=%s, dmax=%s", i, hex1.c_str(), hex2.c_str(), dmin.c_str(), dmax.c_str());

    // Sauvegarde
    preferences.putUInt(("c1_" + p).c_str(), hexToColor(hex1));
    preferences.putUInt(("c2_" + p).c_str(), hexToColor(hex2));
    preferences.putUInt(("dmin_" + p).c_str(), dmin.toInt());
    preferences.putUInt(("dmax_" + p).c_str(), dmax.toInt());
  }

  preferences.end();

  String html = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>body{background:#121212;color:white;font-family:sans-serif;text-align:center;padding:50px;}</style>";
  html += "</head><body>";
  html += "<h1>Sauvegarde OK !</h1>";
  html += "<p>Le systeme va redemarrer dans 2 secondes...</p>";
  html += "</body></html>";

  server.send(200, "text/html", html);

  delay(2000);
  DEBUG_PRINT(DEBUG_INFO, "Settings saved. Restarting ESP...");
  ESP.restart();
}

String WebConfigManager::getHTML() {
  String h = "<!DOCTYPE html><html><head><meta charset='utf-8'>";
  h += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  h += "<title>Light Trainer Config</title>";
  h += "<style>";
  h += "body { font-family: 'Segoe UI', sans-serif; background-color: #1e1e1e; color: #e0e0e0; margin: 0; padding: 20px; }";
  h += "h1 { color: #00ff88; text-align: center; }";
  h += ".card { background: #2d2d2d; border-radius: 12px; padding: 15px; margin-bottom: 20px; box-shadow: 0 4px 6px rgba(0,0,0,0.3); }";
  h += ".card h2 { margin-top: 0; border-bottom: 1px solid #444; padding-bottom: 10px; font-size: 1.2em; color: #ffcc00; }";
  h += ".form-group { margin-bottom: 15px; display: flex; align-items: center; justify-content: space-between; }";
  h += "label { flex: 1; }";
  h += "input[type='color'] { border: none; width: 50px; height: 35px; cursor: pointer; background: none; }";
  h += "input[type='number'] { width: 60px; padding: 8px; border-radius: 5px; border: 1px solid #555; background: #333; color: white; }";
  h += ".btn { display: block; width: 100%; padding: 15px; background: #00ff88; color: #000; font-weight: bold; border: none; border-radius: 8px; font-size: 1.1em; cursor: pointer; margin-top: 20px; }";
  h += ".info { font-size: 0.8em; color: #888; text-align: center; margin-bottom: 20px; }";
  h += "</style></head><body>";

  h += "<h1>Light Trainer</h1>";
  h += "<div class='info'>ID Contrôleur: " + String((uint32_t)ESP.getEfuseMac(), HEX) + "<br>";
  h += "Note: Les esclaves ne sont pas visibles en mode WiFi.</div>";

  h += "<form action='/save' method='POST'>";

  String modeNames[4] = { "Mode 0 (Standard)", "Mode 1 (Duel Couleurs)", "Mode 2 (Réflexe)", "Mode 3 (Chaos)" };
  String modeDescriptions[4] = {
    "Un joueur contre le système. Touchez la cible de la couleur active pour marquer un point.",
    "Deux joueurs s'affrontent. Touchez votre couleur quand elle s'active. Le plus rapide gagne le point.",
    "Testez vos réflexes ! La cible s'allume après un délai aléatoire. Touchez-la le plus vite possible.",
    "Le mode le plus imprévisible. Plusieurs cibles et des couleurs aléatoires, avec des délais variés."
  };

  h += "<p class='info'>Note sur les délais : Si Délai Min et Délai Max sont tous deux à 0, l'activation est instantanée.</p>";

  for (int i = 0; i < 4; i++) {
    String p = String(i);
    h += "<div class='card'>";
    h += "<h2>" + modeNames[i] + "</h2>";
    h += "<p>" + modeDescriptions[i] + "</p>";

    // Couleur 1
    h += "<div class='form-group'><label>Couleur 1</label>";
    h += "<input type='color' name='c1_" + p + "' value='" + colorToHex(currentModes[i].color1) + "'></div>";

    // Couleur 2 (Optionnelle visuellement, mais toujours là)
    h += "<div class='form-group'><label>Couleur 2</label>";
    h += "<input type='color' name='c2_" + p + "' value='" + colorToHex(currentModes[i].color2) + "'></div>";

    // Délais
    h += "<div class='form-group'><label>Délai Min (s)</label>";
    h += "<input type='number' name='dmin_" + p + "' value='" + String(currentModes[i].delayMin) + "'></div>";

    h += "<div class='form-group'><label>Délai Max (s)</label>";
    h += "<input type='number' name='dmax_" + p + "' value='" + String(currentModes[i].delayMax) + "'></div>";

    h += "</div>";
  }

  h += "<button type='submit' class='btn'>SAUVEGARDER & REBOOT</button>";
  h += "</form></body></html>";

  return h;
}

// ---- HELPERS CONVERSION ----

String WebConfigManager::colorToHex(uint32_t color) {
  char hex[8];
  // FastLED est souvent GRB ou RGB, supposons RGB pour le web
  // On force le format #RRGGBB
  sprintf(hex, "#%06X", (unsigned int)color);
  return String(hex);
}

uint32_t WebConfigManager::hexToColor(String hex) {
  if (hex.startsWith("#")) {
    hex.remove(0, 1);
  }
  return strtoul(hex.c_str(), NULL, 16);
}