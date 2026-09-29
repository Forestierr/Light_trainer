<h1 align="center">⚡ Light Trainer — Système d'Entraînement Réactif Sans Fil ⚡</h1>

<p align="center">
  <img src="https://img.shields.io/badge/Hardware-V3%20(XIAO%20ESP32--C3)-blue.svg" alt="Hardware Version"/>
  <img src="https://img.shields.io/badge/Firmware-Arduino%20%2F%20ESP--IDF-orange.svg" alt="Firmware"/>
  <img src="https://img.shields.io/badge/Mesh-ESP--NOW%20(2.4%20GHz)-green.svg" alt="ESP-NOW"/>
  <img src="https://img.shields.io/badge/Sensor-VL53L0X%20ToF%20Laser-red.svg" alt="VL53L0X"/>
  <a href="LICENSE"><img src="https://img.shields.io/badge/License-Apache%202.0-blue.svg" alt="License"/></a>
</p>

<p align="center">
  <b>Pods lumineux interactifs, ultra-réactifs et modulaires pour l'entraînement sportif, le travail des réflexes, de l'agilité et de la réactivité cognitive.</b>
</p>

---

## 🌟 Points Forts du Système

* 🚀 **Zéro Latence & Réseau Maillé Auto-Organisé** : Communication instantanée multi-sauts via **ESP-NOW (2.4 GHz)**. Pas besoin de routeur Wi-Fi, les pods se découvrent et se synchronisent automatiquement.
* ✋ **Interaction 100% Sans Contact (ToF Laser)** : Télémètre **VL53L0X** cadencé à 50 Hz pour une détection millimétrique des frappes sans usure mécanique.
* 🌈 **Affichage 360° Haute Visibilité** : Anneau de 7 LEDs RVB adressables **WS2812B** avec gestion de jauges de progression et flashs de confirmation.
* 🔋 **Gestion d'Énergie Avancée & Autonomie** :
  * Batterie LiPo 3.7V avec **recharge USB-C automatique**.
  * **Deep Sleep ultra-basse consommation ($\approx 10\,\mu\text{A}$)** via coupure d'alimentation commutée $3\text{V}3\_S$ et réveil instantané sur interrupteur.
  * Mesure continue de la tension de batterie avec compensation ADC d'usine eFuse.
* 📱 **Double Interface** : Pilotage 100% gestuel sur le pod + portail Web de configuration intégré (Wi-Fi SoftAP).

---

## 📂 Structure du Répertoire

```
Light_trainer/
├── 1_Documentation/               # 📖 Documentation complète & schémas
│   ├── Architecture_Systeme.md    # Spécifications techniques & protocoles
│   ├── Dashboard_Web_App.md       # Spécifications de l'interface Web & API
│   ├── Guide_Montage_Materiel.md  # Guide pas-à-pas d'assemblage & BOM
│   ├── Manuel_Utilisation_et_Modes.md # Guide des gestes & 5 modes de jeu
│   ├── Roadmap_Idees_et_TODO.md   # Feuille de route & TODO pour la production
│   └── DOC_FAB/schema.pdf         # Schéma électronique officiel V3
├── 2_Mesures/                     # 📊 Cahier d'expérimentation & banc d'essais
│   └── Protocole_de_Tests_et_Mesures.md # Tableaux de relevés et qualification
├── 3_Datasheets/                  # 📑 Fiches techniques des composants
├── 4_Fabrication/                 # 🛠️ Fichiers de fabrication KiCad & Gerber
│   └── Version_02/light_trainer/  # Projet PCB KiCad V3
├── 5_Programmation/               # 💻 Codes sources & firmware
│   ├── XIAO-esp32-v3/main/        # Firmware principal complet (Production)
│   └── TEST_BENCH/                # Suite de qualification unitaire matérielle
└── README.md                      # Ce document
```

---

## 📐 Schéma & Câblage Matériel (V3 / Veroboard)

### Pinout du Seeed Studio XIAO ESP32-C3

```
                       +-------------------+
                       |    [ USB-C ]      |
     (ADC Batt)   D1 --| 2              14 |-- 5V VBUS  (Détection Charge USB)
   (Switch SW1)   D2 --| 3              13 |-- GND      (Masse Commune)
  (Buzzer Opt.)   D3 --| 4              12 |-- 3V3_OUT  (Sortie Régulateur LDO)
     (I2C SCL)    D4 --| 5              11 |-- D10      (Entrée Diviseur Charge)
     (I2C SDA)    D5 --| 6              10 |-- D9
                  D6 --| 7               9 |-- D8
    (LED Data)    D7 --| 8               8 |-- 3V3
                       +-------------------+
                          |  [BAT+]  [GND] |
                          |   (21)    (22) |
```

> [!TIP]
> **Résistances $100\text{ k}\Omega$ Unifiées** : Le circuit utilise uniquement des résistances standard de **$100\text{ k}\Omega$** ($R_1$ à $R_6$), simplifiant l'approvisionnement et le montage.

Consultez le [**Guide de Montage Matériel**](file:///c:/Users/Robin/Desktop/GitHub/Light_trainer/1_Documentation/Guide_Montage_Materiel.md) pour les détails d'implantation et les schémas complets.

---

## 🎮 Modes de Jeu & Navigation Gestuelle

| Geste ToF | Durée | Action en Mode Menu | Action en Cours de Jeu |
| :--- | :--- | :--- | :--- |
| **Tap Court** | $< 500\text{ ms}$ | Défilement des 5 modes (0 $\rightarrow$ 1 $\rightarrow$ 2 $\rightarrow$ 3 $\rightarrow$ 4 $\rightarrow$ 0) | *Ignoré* |
| **Hold Départ** | $1.5\text{ s}$ | **Départ de partie** (Jauge circulaire 0 à 100%) | *Non applicable* |
| **Frappe (Hit)** | $< 300\text{ ms}$ | *Ignoré* | **Validation de la cible** (Flash blanc instantané) |
| **Hold Menu** | $3.0\text{ s}$ | *Réinitialisation* | **Arrêt du jeu** et retour synchronisé au Menu pour tout le réseau |

Consultez le [**Manuel d'Utilisation**](file:///c:/Users/Robin/Desktop/GitHub/Light_trainer/1_Documentation/Manuel_Utilisation_et_Modes.md) pour découvrir les **5 modes sportifs** disponibles (Vitesse Solo, Duel Bicolore, Réflexe Aléatoire, Agilité Cognitive et Jeu Simon).

---

## 🧪 Banc de Tests et Qualification

Un firmware dédié autonome [`5_Programmation/TEST_BENCH/TEST_BENCH.ino`](file:///c:/Users/Robin/Desktop/GitHub/Light_trainer/5_Programmation/TEST_BENCH/TEST_BENCH.ino) permet de mesurer précisément les performances de votre prototype :

1. `TEST_BATTERY_STRESS` : Décharge extrême à pleine puissance (LEDs 100% blanches + RF) avec export CSV.
2. `TEST_BATTERY_GAME_SIM` : Test d'autonomie en conditions de match réelles.
3. `TEST_TOF_BENCHMARK` : Test de portée ToF, cadence de frappe (hits/min) et tenue aux perturbations solaires.
4. `TEST_RADIO_PING_RTT_RSSI` : Mesure de la portée radio, de la latence RTT (ms) et de la force du signal (RSSI).
5. `TEST_DEEP_SLEEP_POWER` : Validation de l'interrupteur matériel et mesure du courant de veille.
6. `TEST_ALL_INTERACTIVE` : Tableau de bord interactif par commandes Série.

---

## 🚀 Démarrage Rapide

### Prérequis Logiciels
* [Arduino IDE](https://www.arduino.cc/en/software) (version 2.x recommandée).
* Package de cartes : **ESP32 by Espressif Systems** (version 2.0.14+ ou 3.x).
* Sélectionner la carte : **`XIAO_ESP32C3`**.
* **Configuration Indispensable de la Flash** :
  > [!IMPORTANT]
  > Dans le menu **Outils $\rightarrow$ Partition Scheme**, sélectionnez impérativement **`Huge APP (3MB No OTA/1MB SPIFFS)`**.
  > La puce physique du XIAO ESP32-C3 dispose de 4 Mo de Flash. Sans ce réglage, le schéma standard réserve seulement 1.3 Mo à l'application, ce qui est insuffisant pour accueillir l'interface Web complète et provoque une erreur de compilation *`Sketch too big`*.
* Bibliothèques Arduino requises :
  * `FastLED`
  * `Adafruit_VL53L0X`

### Flasher le Projet
1. Ouvrez [`5_Programmation/XIAO-esp32-v3/main/main.ino`](file:///c:/Users/Robin/Desktop/GitHub/Light_trainer/5_Programmation/XIAO-esp32-v3/main/main.ino).
2. Branchez le XIAO ESP32-C3 via un câble USB-C.
3. Vérifiez que la carte est bien configurée en **Huge APP (3MB)**.
4. Cliquez sur **Téléverser**.
5. Ouvrez le Moniteur Série à **115200 bauds** pour observer le tableau de bord temps réel.

---

## 🗺️ Feuille de Route & Production

Pour découvrir les axes d'évolution logicielle pour le déploiement à grande échelle (SoftAP master exclusif, WebSockets, Gzip, PWA, calibration ToF, Web OTA et PlatformIO), consultez notre [**Feuille de Route & TODO Backlog**](file:///c:/Users/Robin/Desktop/GitHub/Light_trainer/1_Documentation/Roadmap_Idees_et_TODO.md).

---

## 📄 Licence

Ce projet est distribué sous licence **Apache 2.0**. Consultez le fichier [`LICENSE`](file:///c:/Users/Robin/Desktop/GitHub/Light_trainer/LICENSE) pour plus de détails.

---
<p align="center"><i>Robin Forestier — Projet Light Trainer</i></p>
