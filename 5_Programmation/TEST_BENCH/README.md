# Banc de Tests et Qualification Matérielle - Light Trainer V3

Ce banc de test (`TEST_BENCH.ino`) est conçu pour mesurer et qualifier avec précision les performances réelles de votre prototype Light Trainer V3 (Veroboard / PCB V3 avec XIAO ESP32-C3).

---

## 📋 Comment utiliser le Banc de Test ?

1. Ouvrez `5_Programmation/TEST_BENCH/TEST_BENCH.ino` dans l'IDE Arduino.
2. À la ligne 30, choisissez le test que vous souhaitez effectuer avec `#define CURRENT_TEST` :
   ```cpp
   #define CURRENT_TEST TEST_TOF_BENCHMARK
   ```
3. Téléversez le code dans le XIAO ESP32-C3.
4. Ouvrez le **Moniteur Série Arduino** réglé à **115200 bauds** (ou le Traceur Série pour les graphiques).

---

## 🧪 Description des Tests Disponibles

### 1. `TEST_BATTERY_STRESS` — Décharge Maximale (Pire Cas)
* **Conditions de test** :
  - 7 LEDs blanches à 100% de puissance (`RGB = 255, 255, 255`).
  - Transmission radio ESP-NOW continue à 20 paquets / seconde.
  - Télémètre ToF VL53L0X en lecture continue à 50 Hz.
* **Consommation estimée** : ~450 mA à 500 mA.
* **Format de sortie (CSV prêt pour Excel)** :
  ```csv
  Temps_s; Horodatage; Tension_mV; Pourcentage_pct; USB
  0; 00:00:00; 4180; 98; NON
  5; 00:00:05; 4165; 97; NON
  ...
  ```
* **Objectif** : Mesurer l'autonomie minimale absolue de la batterie LiPo et tracer la courbe de décharge sous forte charge. Coupure automatique à 3.0V pour préserver la batterie.

---

### 2. `TEST_BATTERY_GAME_SIM` — Décharge en Session de Jeu Réelle
* **Conditions de test** :
  - Luminosité nominale (50 / 255).
  - Alternance dynamique des couleurs et flashs courts (60 ms).
  - Échanges radio ESP-NOW réalistes.
* **Consommation estimée** : ~80 mA à 120 mA moyen.
* **Objectif** : Valider l'autonomie réelle en entraînement sportif (objectif : **> 3 à 5 heures**).

---

### 3. `TEST_TOF_BENCHMARK` — Qualification ToF, Frappes & Perturbations Solaires
* **Fonctionnalités** :
  - **Mesure de distance en temps réel** (mm) à 50 Hz.
  - **Compteur de Hits validés** avec calcul de la durée de frappe (ms) et de la cadence (hits/minute).
  - **Détection des perturbations solaires / optiques** : comptabilise les erreurs optiques et les pertes de signal dues aux infrarouges du soleil.
  - **Retour visuel dynamique** :
    - Jauge bleue progressive à l'approche de la main (150 mm à 500 mm).
    - Jauge verte en zone active (< 150 mm).
    - **Flash blanc instantané** lors de la validation d'un Hit.
* **Objectif** :
  - Mesurer la réactivité (latence < 30 ms).
  - Tester en extérieur sous plein soleil pour mesurer le taux d'erreur optique.

---

### 4. `TEST_RADIO_PING_RTT_RSSI` — Portée Radio, Latence & Force du Signal
* **Configuration** :
  - Flashé sur **Pod 1** avec `#define RADIO_ROLE ROLE_PINGER`
  - Flashé sur **Pod 2** avec `#define RADIO_ROLE ROLE_PONGER`
* **Métriques mesurées** :
  - **Latence RTT (Round Trip Time)** : temps aller-retour en millisecondes.
  - **Force du signal RSSI** : niveau en dBm mesuré en réception.
  - **Taux de perte de paquets (Packet Loss %)** : pourcentage d'échecs.
* **Indicateur LED de qualité du signal en direct** :
  - 🟢 **Vert** : Signal excellent ($> -65\text{ dBm}$)
  - 🟡 **Jaune** : Signal moyen ($-65\text{ dBm}$ à $-80\text{ dBm}$)
  - 🔴 **Rouge** : Signal faible / limite de décrochage ($< -85\text{ dBm}$)
* **Objectif** : Éloigner les pods pour trouver la distance maximale en champ libre et en intérieur à travers des murs.

---

### 5. `TEST_DEEP_SLEEP_POWER` — Interrupteur ON/OFF & Veille Profonde
* **Fonctionnalités** :
  - Détection de l'état de l'interrupteur mécanique `SW1` sur `D2` (3V3_S).
  - Détection de la présence du câble USB 5V sur `D10` (`CHARGE_PIN`).
  - Mesure instantanée de la tension batterie sur `D1`.
  - **Test du Deep Sleep** : Lorsque l'interrupteur est basculé sur OFF (et hors USB), le microcontrôleur coupe les LEDs, ferme le bus I2C, isole les GPIOs et passe en veille profonde.
  - **Réveil instantané** dès que l'interrupteur est rebasculé sur ON.
* **Objectif** : Vérifier au multimètre que la consommation en veille est inférieure à $20\,\mu\text{A}$.

---

### 6. `TEST_ALL_INTERACTIVE` — Tableau de Bord Interactif
* **Commandes Série (115200 bauds)** :
  - `r` / `g` / `b` : Allume les LEDs en Rouge / Vert / Bleu
  - `w` : Allume les LEDs en Blanc 100%
  - `o` : Éteint les LEDs
  - `t` : Déclenche une mesure ToF
  - `p` : Envoie un paquet Ping ESP-NOW
  - `?` : Affiche le menu d'aide
