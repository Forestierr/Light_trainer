/*
 =========================================================================================
   LIGHT TRAINER - SUITE DE TESTS ET QUALIFICATION DU PROTOTYPE (TEST BENCH)
 =========================================================================================
   Microcontrôleur : Seeed Studio XIAO ESP32-C3
   Version Matérielle : V3 (Veroboard / PCB V3)

   INSTRUCTIONS D'UTILISATION :
   ---------------------------
   Choisissez le test à exécuter en modifiant simplement la ligne `#define CURRENT_TEST`
   ci-dessous, puis téléversez le code dans votre XIAO ESP32-C3.
   Ouvrez le moniteur série Arduino à 115200 bauds.
 =========================================================================================
*/

#include <Arduino.h>
#include <FastLED.h>
#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

// =========================================================================================
//  1. SELECTION DU TEST ACTIF
// =========================================================================================
#define TEST_BATTERY_STRESS         1  // Décharge maximale (LEDs 100% blanc + WiFi broadcast continu + ToF)
#define TEST_BATTERY_GAME_SIM       2  // Décharge en conditions de jeu réelles (LEDs 50% + comms normales)
#define TEST_TOF_BENCHMARK          3  // Qualification ToF, mesure distance, compteur de Hits & perturbations solaires
#define TEST_RADIO_PING_RTT_RSSI    4  // Portée radio ESP-NOW, mesure Latence RTT (ms), RSSI (dBm) et perte de paquets
#define TEST_DEEP_SLEEP_POWER       5  // Test de l'interrupteur ON/OFF, détection USB et mesure veille profonde
#define TEST_ALL_INTERACTIVE        6  // Tableau de bord complet & menu interactif en direct sur le port Série

// >>> CHOISISSEZ VOTRE TEST ICI <<<
#define CURRENT_TEST TEST_TOF_BENCHMARK

// =========================================================================================
//  2. OPTIONS SPECIFIQUES AUX TESTS
// =========================================================================================
// Pour TEST_RADIO_PING_RTT_RSSI :
#define ROLE_PINGER  1  // Pod Émetteur (envoie les pings et mesure RTT / RSSI / Packet Loss)
#define ROLE_PONGER  2  // Pod Répondeur (renvoie instantanément l'écho)
#define RADIO_ROLE   ROLE_PINGER  // Choisissez ROLE_PINGER sur le pod 1 et ROLE_PONGER sur le pod 2

// Intervalle d'envoi des pings radio (ms)
#define RADIO_PING_INTERVAL_MS 100 // 10 pings par seconde

// Pour TEST_TOF_BENCHMARK :
#define TOF_HIT_DISTANCE_MM    150 // Seuil de détection de la main / frappe (mm)
#define TOF_HIT_MIN_DURATION   30  // Durée min de présence pour valider un hit (filtrage bruit) en ms
#define TOF_HIT_DEBOUNCE_MS    120 // Temps mort anti-rebond après un hit (ms)

// =========================================================================================
//  3. PINOUT ET CONFIGURATION MATERIELLE (V3)
// =========================================================================================
#define LED_PIN       D7    // Broche 8 de U1 (WS2812B Data)
#define NUM_LEDS      7
#define SCL_PIN       D4    // Broche 5 de U1 (I2C SCL - VL53L0X)
#define SDA_PIN       D5    // Broche 6 de U1 (I2C SDA - VL53L0X)
#define SW_PIN        D2    // Broche 3 de U1 (Interrupteur 3V3_S / Wakeup)
#define CHARGE_PIN    D10   // Broche 11 de U1 (Détection USB 5V VBUS)
#define BAT_ADC_PIN   D1    // Broche 2 de U1 (Pont diviseur 100k/100k)
#define BUZZER_PIN    D3    // Broche 4 de U1 (Buzzer optionnel)

// Paramètres de mesure batterie (Pont 100k / 100k -> Ratio = 0.5)
#define BAT_VOLT_FULL   4.20f
#define BAT_VOLT_LOW    3.40f
#define BAT_VOLT_EMPTY  3.00f
#define VOLTAGE_DIVIDER_RATIO 0.50f // R2 / (R1 + R2)

// =========================================================================================
//  4. STRUCTURES ET OBJETS GLOBAUX
// =========================================================================================
CRGB leds[NUM_LEDS];
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
bool hasSensor = false;

// Structure de trame pour le test Radio
typedef struct __attribute__((packed)) {
  uint32_t seqNum;       // Numéro de séquence incrémental
  uint32_t timestampMs;  // Timestamp de départ émetteur
  int8_t   rssiEcho;     // RSSI mesuré par le répondeur
  uint8_t  payload[16];  // Données de test
} RadioTestPacket;

uint8_t broadcastMac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Variables statistiques pour le test Radio
volatile bool newPacketReceived = false;
RadioTestPacket rxPacket;
int8_t lastRxRssi = 0;
uint32_t radioPingsSent = 0;
uint32_t radioPongsReceived = 0;
uint32_t radioPacketsLost = 0;
float radioRttAvgMs = 0;
float radioLastRttMs = 0;

// Variables statistiques pour le test ToF
uint32_t tofHitsCount = 0;
uint32_t tofErrorCount = 0;
uint32_t tofSamplesCount = 0;
uint32_t tofLastHitTime = 0;
bool handWasPresent = false;
uint32_t handEnterTime = 0;

// Variables générales de temps
uint32_t testStartTime = 0;
uint32_t lastLogTime = 0;

// =========================================================================================
//  5. FONCTIONS UTILITAIRES MATERIELLES
// =========================================================================================

// Mesure de la tension batterie calibrée eFuse
float readBatteryVoltage() {
  uint32_t mv = analogReadMilliVolts(BAT_ADC_PIN);
  float vAdc = mv / 1000.0f;
  return vAdc / VOLTAGE_DIVIDER_RATIO;
}

// Pourcentage restant
int calculateBatteryPercentage(float voltage) {
  int pct = (int)((voltage - BAT_VOLT_EMPTY) / (BAT_VOLT_FULL - BAT_VOLT_EMPTY) * 100.0f);
  return constrain(pct, 0, 100);
}

// Détection alimentation USB
bool isUsbConnected() {
  return digitalRead(CHARGE_PIN) == HIGH;
}

// Détection interrupteur mécanique
bool isPowerSwitchOn() {
  return digitalRead(SW_PIN) == HIGH;
}

// Éteindre les LEDs
void clearLeds() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

// Allumer toutes les LEDs d'une couleur
void setAllLeds(CRGB color) {
  fill_solid(leds, NUM_LEDS, color);
  FastLED.show();
}

// Jauge circulaire (0.0 à 1.0)
void setLedGauge(CRGB color, float progress) {
  progress = constrain(progress, 0.0f, 1.0f);
  int count = round(progress * NUM_LEDS);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = (i < count) ? color : CRGB::Black;
  }
  FastLED.show();
}

// Formatage du temps en hh:mm:ss
String formatTime(uint32_t ms) {
  uint32_t totalSec = ms / 1000;
  uint32_t hours = totalSec / 3600;
  uint32_t minutes = (totalSec % 3600) / 60;
  uint32_t seconds = totalSec % 60;
  char buf[32];
  snprintf(buf, sizeof(buf), "%02lu:%02lu:%02lu", hours, minutes, seconds);
  return String(buf);
}

// =========================================================================================
//  6. CALLBACKS ESP-NOW (Radio)
// =========================================================================================
void onEspNowSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  // Callback d'émission
}

void onEspNowRecv(const esp_now_recv_info *recv_info, const uint8_t *data, int len) {
  if (len == sizeof(RadioTestPacket)) {
    memcpy(&rxPacket, data, sizeof(RadioTestPacket));
    if (recv_info->rx_ctrl != NULL) {
      lastRxRssi = recv_info->rx_ctrl->rssi;
    } else {
      lastRxRssi = 0;
    }
    newPacketReceived = true;
  }
}

// Initialisation ESP-NOW
void initRadio() {
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false); // Pas de mise en veille WiFi
  while (!WiFi.STA.started()) {
    delay(50);
  }
  
  // Verrouillage sur le canal 1
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);

  if (esp_now_init() != ESP_OK) {
    Serial.println("[RADIO ERROR] Échec esp_now_init() !");
    return;
  }

  esp_now_register_send_cb(onEspNowSent);
  esp_now_register_recv_cb(onEspNowRecv);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastMac, 6);
  peerInfo.channel = 1;
  peerInfo.encrypt = false;
  peerInfo.ifidx = WIFI_IF_STA;
  esp_now_add_peer(&peerInfo);

  Serial.println("[RADIO] ESP-NOW initialisé sur le Canal 1 (Mode Broadcast).");
}

// =========================================================================================
//  7. IMPLÉMENTATION DES DIFFÉRENTS MODES DE TEST
// =========================================================================================

// -----------------------------------------------------------------------------------------
// TEST 1 : DÉCHARGE BATTERIE MAXIMALE (STRESS TEST - PIRE CAS)
// -----------------------------------------------------------------------------------------
void setupTestBatteryStress() {
  Serial.println("==================================================================");
  Serial.println(">>> TEST 1 : DÉCHARGE MAXIMALE DE LA BATTERIE (STRESS TEST) <<<");
  Serial.println("Conditions : 7 LEDs BLANCHES 100% + WiFi broadcast 20Hz + ToF 50Hz");
  Serial.println("Objectif   : Mesurer l'autonomie minimale absolue et le courant max");
  Serial.println("Sortie CSV : Temps_s; Horodatage; Tension_mV; Pourcentage_pct; USB");
  Serial.println("==================================================================");
  
  // LEDs à 100% de luminosité en Blanc pur (consommation maximale ~400mA)
  FastLED.setBrightness(255);
  setAllLeds(CRGB::White);
  
  // Initialiser la radio pour transmission continue
  initRadio();
  testStartTime = millis();
}

void loopTestBatteryStress() {
  // 1. Transmission radio continue (20 paquets / sec pour solliciter l'étage RF)
  static uint32_t lastTxTime = 0;
  if (millis() - lastTxTime >= 50) {
    lastTxTime = millis();
    RadioTestPacket pkt;
    pkt.seqNum = ++radioPingsSent;
    pkt.timestampMs = millis();
    esp_now_send(broadcastMac, (uint8_t*)&pkt, sizeof(pkt));
  }

  // 2. Mesure continue ToF (sollicite le laser IR et le bus I2C)
  if (hasSensor && lox.isRangeComplete()) {
    lox.readRange();
  }

  // 3. Log régulier toutes les 5 secondes
  if (millis() - lastLogTime >= 5000) {
    lastLogTime = millis();
    uint32_t elapsedMs = millis() - testStartTime;
    float vBat = readBatteryVoltage();
    int pct = calculateBatteryPercentage(vBat);
    bool usb = isUsbConnected();

    Serial.printf("%lu; %s; %.0f; %d; %s\n",
                  elapsedMs / 1000,
                  formatTime(elapsedMs).c_str(),
                  vBat * 1000.0f,
                  pct,
                  usb ? "OUI" : "NON");

    // Alerte de coupure critique (< 3.0V)
    if (vBat <= BAT_VOLT_EMPTY && !usb) {
      Serial.println("\n[ATTENTION] SEUIL CRITIQUE ATTEINT (3.0V) ! Fin du test de décharge.");
      setAllLeds(CRGB::Red);
      while (1) { delay(1000); }
    }
  }
}

// -----------------------------------------------------------------------------------------
// TEST 2 : DÉCHARGE BATTERIE EN CONDITIONS DE JEU RÉELLES
// -----------------------------------------------------------------------------------------
void setupTestBatteryGameSim() {
  Serial.println("==================================================================");
  Serial.println(">>> TEST 2 : DÉCHARGE EN SIMULATION DE JEU RÉEL <<<");
  Serial.println("Conditions : Luminosité 50/255, alternance de couleurs et flashs");
  Serial.println("Objectif   : Mesurer l'autonomie typique en session sportive (~3-5h)");
  Serial.println("==================================================================");

  FastLED.setBrightness(50);
  initRadio();
  testStartTime = millis();
}

void loopTestBatteryGameSim() {
  static uint32_t lastStep = 0;
  static uint8_t step = 0;

  // Animation cyclique simulant des phases de jeu (attente, cible active, hit)
  if (millis() - lastStep >= 1000) {
    lastStep = millis();
    step = (step + 1) % 4;

    switch (step) {
      case 0: setAllLeds(CRGB::Blue); break;
      case 1: setAllLeds(CRGB::Green); break;
      case 2: setAllLeds(CRGB::Red); break;
      case 3: 
        // Flash blanc court
        setAllLeds(CRGB::White);
        delay(60);
        setAllLeds(CRGB::Black);
        break;
    }
  }

  // ToF lecture
  if (hasSensor && lox.isRangeComplete()) {
    lox.readRange();
  }

  // Log toutes les 10 secondes
  if (millis() - lastLogTime >= 10000) {
    lastLogTime = millis();
    uint32_t elapsedMs = millis() - testStartTime;
    float vBat = readBatteryVoltage();
    int pct = calculateBatteryPercentage(vBat);
    Serial.printf("[JEU SIM] Temps: %s | Vbat: %.2fV (%d%%) | USB: %s\n",
                  formatTime(elapsedMs).c_str(),
                  vBat,
                  pct,
                  isUsbConnected() ? "Connecté" : "Sur Batterie");
  }
}

// -----------------------------------------------------------------------------------------
// TEST 3 : QUALIFICATION DU CAPTEUR ToF (COMPTEUR HITS & PERTURBATIONS SOLAIRES)
// -----------------------------------------------------------------------------------------
void setupTestTofBenchmark() {
  Serial.println("==================================================================");
  Serial.println(">>> TEST 3 : QUALIFICATION DU CAPTEUR ToF & COMPTEUR DE HITS <<<");
  Serial.println("Objectif   : Tester la portée, cadence de frappe et tenue au soleil");
  Serial.println("Instructions :");
  Serial.println(" - Passez la main à moins de 150mm pour déclencher un Hit");
  Serial.println(" - Observez les LEDs (Jauge bleue de distance + Flash blanc sur Hit)");
  Serial.println(" - En extérieur : vérifiez si le soleil génère des faux hits ou erreurs");
  Serial.println("==================================================================");
  FastLED.setBrightness(100);
}

void loopTestTofBenchmark() {
  if (!hasSensor) {
    setAllLeds(CRGB::Orange);
    Serial.println("[ERREUR] Capteur VL53L0X non détecté sur le bus I2C !");
    delay(1000);
    return;
  }

  if (lox.isRangeComplete()) {
    uint16_t dist = lox.readRange();
    tofSamplesCount++;

    // Analyse de la validité de la mesure
    bool isOutOfRange = (dist >= 8000);
    bool isError = (dist == 0 || (dist > 2000 && dist < 8000));
    if (isError) {
      tofErrorCount++;
    }

    bool isHandNear = (dist <= TOF_HIT_DISTANCE_MM && !isOutOfRange);

    // Détection de transition pour la frappe (Hit)
    if (isHandNear) {
      if (!handWasPresent) {
        handWasPresent = true;
        handEnterTime = millis();
      }
    } else {
      if (handWasPresent) {
        uint32_t duration = millis() - handEnterTime;
        handWasPresent = false;

        // Validation du Hit si la durée dépasse le filtre anti-bruit
        if (duration >= TOF_HIT_MIN_DURATION && (millis() - tofLastHitTime >= TOF_HIT_DEBOUNCE_MS)) {
          tofHitsCount++;
          tofLastHitTime = millis();

          // Calcul de la cadence de frappe (Hits par minute sur les 10 dernières secondes)
          float hitsPerMin = (tofHitsCount * 60000.0f) / (millis() - testStartTime);

          Serial.printf(">>> [HIT #%lu VALIDÉ] Durée: %lums | Dist: %umm | Cadence: %.1f hits/min\n",
                        tofHitsCount, duration, dist, hitsPerMin);

          // Flash visuel blanc instantané
          setAllLeds(CRGB::White);
          delay(50);
        }
      }
    }

    // Affichage LED dynamique : Jauge circulaire montrant la proximité de la main
    if (isHandNear) {
      float proximity = 1.0f - ((float)dist / (float)TOF_HIT_DISTANCE_MM);
      setLedGauge(CRGB::Green, proximity);
    } else if (isOutOfRange) {
      clearLeds();
    } else {
      // Main en approche (entre 150mm et 500mm) -> LEDs bleues douces
      float approach = 1.0f - constrain(((float)dist - 150.0f) / 350.0f, 0.0f, 1.0f);
      setLedGauge(CRGB::Blue, approach * 0.5f);
    }

    // Affichage périodique sur le port série toutes les 500ms
    if (millis() - lastLogTime >= 500) {
      lastLogTime = millis();
      float errorRate = (tofSamplesCount > 0) ? ((float)tofErrorCount / tofSamplesCount * 100.0f) : 0.0f;
      Serial.printf("[ToF STREAM] Dist: %4d mm | Hits: %3lu | Erreurs optiques: %lu (%.1f%%) | Échantillons: %lu\n",
                    dist, tofHitsCount, tofErrorCount, errorRate, tofSamplesCount);
    }
  }
}

// -----------------------------------------------------------------------------------------
// TEST 4 : TEST RADIO ESP-NOW (PORTÉE, LATENCE RTT & RSSI)
// -----------------------------------------------------------------------------------------
void setupTestRadioPingRssi() {
  Serial.println("==================================================================");
  Serial.println(">>> TEST 4 : QUALIFICATION RADIO ESP-NOW (PORTÉE & LATENCE) <<<");
  #if RADIO_ROLE == ROLE_PINGER
  Serial.println("Rôle       : PINGER (Émetteur de pings & mesure de qualité)");
  Serial.printf("Cadence    : %d ms (%d pings/sec)\n", RADIO_PING_INTERVAL_MS, 1000 / RADIO_PING_INTERVAL_MS);
  #else
  Serial.println("Rôle       : PONGER (Récepteur en mode Écho)");
  #endif
  Serial.println("Feedback LED : Vert (> -65dBm), Jaune (> -80dBm), Rouge (< -85dBm / Perte)");
  Serial.println("==================================================================");

  FastLED.setBrightness(80);
  initRadio();
  testStartTime = millis();
}

void loopTestRadioPingRssi() {
#if RADIO_ROLE == ROLE_PINGER
  // 1. Émission périodique d'un Ping
  static uint32_t lastPingSentTime = 0;
  static uint32_t currentPingSeq = 0;

  if (millis() - lastPingSentTime >= RADIO_PING_INTERVAL_MS) {
    lastPingSentTime = millis();
    currentPingSeq++;
    radioPingsSent++;

    RadioTestPacket txPkt;
    txPkt.seqNum = currentPingSeq;
    txPkt.timestampMs = millis();
    txPkt.rssiEcho = 0;

    esp_now_send(broadcastMac, (uint8_t*)&txPkt, sizeof(txPkt));
  }

  // 2. Traitement de la réponse (Pong)
  if (newPacketReceived) {
    newPacketReceived = false;
    uint32_t rtt = millis() - rxPacket.timestampMs;
    radioPongsReceived++;
    radioLastRttMs = rtt;

    // Calcul de la moyenne glissante du RTT
    if (radioRttAvgMs == 0) radioRttAvgMs = rtt;
    else radioRttAvgMs = (radioRttAvgMs * 0.9f) + (rtt * 0.1f);

    // Calcul de la force du signal et retour LED
    // lastRxRssi est typiquement entre -30dBm (très proche) et -90dBm (limite décrochage)
    if (lastRxRssi > -65) {
      setAllLeds(CRGB::Green); // Signal excellent
    } else if (lastRxRssi > -80) {
      setAllLeds(CRGB::Yellow); // Signal moyen
    } else {
      setAllLeds(CRGB::Red); // Signal faible
    }
  }

  // 3. Rapport statistique toutes les 1 seconde
  if (millis() - lastLogTime >= 1000) {
    lastLogTime = millis();
    radioPacketsLost = (radioPingsSent > radioPongsReceived) ? (radioPingsSent - radioPongsReceived) : 0;
    float lossRate = (radioPingsSent > 0) ? ((float)radioPacketsLost / radioPingsSent * 100.0f) : 0.0f;

    Serial.printf("[RADIO PING #%lu] RSSI: %3d dBm | RTT: %2.1f ms (Moy: %2.1f ms) | Paquets: %lu envoyés, %lu reçus | Perte: %.1f%%\n",
                  radioPingsSent,
                  lastRxRssi,
                  radioLastRttMs,
                  radioRttAvgMs,
                  radioPingsSent,
                  radioPongsReceived,
                  lossRate);
  }

#elif RADIO_ROLE == ROLE_PONGER
  // Mode Ponger : Dès réception d'un ping, on le renvoie instantanément avec l'info RSSI
  if (newPacketReceived) {
    newPacketReceived = false;
    RadioTestPacket echoPkt = rxPacket;
    echoPkt.rssiEcho = lastRxRssi;

    esp_now_send(broadcastMac, (uint8_t*)&echoPkt, sizeof(echoPkt));
    radioPongsReceived++;

    // Retour LED : flash vert court lors de la réponse
    setAllLeds(CRGB::Blue);
    delayMicroseconds(500);
    clearLeds();
  }

  if (millis() - lastLogTime >= 2000) {
    lastLogTime = millis();
    Serial.printf("[PONGER ECHO] Pings répondus: %lu | Dernier RSSI reçu: %d dBm\n",
                  radioPongsReceived, lastRxRssi);
  }
#endif
}

// -----------------------------------------------------------------------------------------
// TEST 5 : TEST DE L'INTERRUPTEUR ON/OFF, DÉTECTION USB & VEILLE PROFONDE
// -----------------------------------------------------------------------------------------
void setupTestDeepSleepPower() {
  Serial.println("==================================================================");
  Serial.println(">>> TEST 5 : QUALIFICATION ÉTAGE ALIMENTATION & DEEP SLEEP <<<");
  Serial.println("Ce test valide :");
  Serial.println(" 1. L'état de l'interrupteur ON/OFF (SW_PIN D2 / 3V3_S)");
  Serial.println(" 2. La détection du câble USB 5V (CHARGE_PIN D10 / VBUS)");
  Serial.println(" 3. La mesure ADC de la batterie (BAT_ADC_PIN D1)");
  Serial.println(" 4. L'extinction propre et le réveil en Deep Sleep");
  Serial.println("------------------------------------------------------------------");
  Serial.println("Basculez l'interrupteur sur OFF pour tester la mise en veille !");
  Serial.println("==================================================================");
  FastLED.setBrightness(80);
}

void loopTestDeepSleepPower() {
  bool swState = isPowerSwitchOn();
  bool usbState = isUsbConnected();
  float vBat = readBatteryVoltage();
  int pct = calculateBatteryPercentage(vBat);

  // État des LEDs :
  // - Vert si allumé sur batterie
  // - Orange si en charge USB
  // - Clignotement rouge si batterie faible (<3.4V)
  if (usbState) {
    setAllLeds(CRGB::Orange);
  } else if (vBat < BAT_VOLT_LOW) {
    setAllLeds(CRGB::Red);
  } else {
    setAllLeds(CRGB::Green);
  }

  // Log régulier toutes les 1 seconde
  if (millis() - lastLogTime >= 1000) {
    lastLogTime = millis();
    Serial.printf("[ALIMENTATION] Switch SW1: %s | USB VBUS: %s | Batterie: %.2fV (%d%%)\n",
                  swState ? "ON (Actif)" : "OFF (Éteint)",
                  usbState ? "BRANCHÉ (En charge)" : "NON BRANCHÉ",
                  vBat,
                  pct);
  }

  // Test de mise en veille automatique lors du basculement de l'interrupteur
  // (uniquement si l'USB n'est pas branché pour ne pas briser la liaison de dev)
  if (!swState && !usbState) {
    Serial.println("\n[DEEP SLEEP] Interrupteur basculé sur OFF !");
    Serial.println("[DEEP SLEEP] Extinction des LEDs, fermeture I2C et passage en veille profonde...");
    Serial.flush();

    // 1. Éteindre les LEDs et isoler la ligne de données
    clearLeds();
    pinMode(LED_PIN, INPUT_PULLDOWN);

    // 2. Clôturer le bus I2C et isoler SDA/SCL
    Wire.end();
    pinMode(SCL_PIN, INPUT_PULLDOWN);
    pinMode(SDA_PIN, INPUT_PULLDOWN);

    // 3. Configurer le réveil sur front montant (D2 passe à HIGH quand on rallume SW1)
    esp_deep_sleep_enable_gpio_wakeup(1ULL << SW_PIN, ESP_GPIO_WAKEUP_GPIO_HIGH);

    // 4. Entrée en veille profonde
    esp_deep_sleep_start();
  }
}

// -----------------------------------------------------------------------------------------
// TEST 6 : TABLEAU DE BORD COMPLET & MENU INTERACTIF
// -----------------------------------------------------------------------------------------
void setupTestAllInteractive() {
  Serial.println("==================================================================");
  Serial.println(">>> TEST 6 : TABLEAU DE BORD GLOBAL INTERACTIF <<<");
  Serial.println("Commandes Série disponibles :");
  Serial.println(" 'r' : Allumer LEDs en ROUGE");
  Serial.println(" 'g' : Allumer LEDs en VERT");
  Serial.println(" 'b' : Allumer LEDs en BLEU");
  Serial.println(" 'w' : Allumer LEDs en BLANC 100%");
  Serial.println(" 'o' : Éteindre les LEDs");
  Serial.println(" 't' : Effectuer une mesure ToF manuelle");
  Serial.println(" 'p' : Envoyer un Ping ESP-NOW");
  Serial.println(" '?' : Réafficher le menu");
  Serial.println("==================================================================");

  FastLED.setBrightness(100);
  initRadio();
  testStartTime = millis();
}

void loopTestAllInteractive() {
  // Lecture des commandes série
  if (Serial.available()) {
    char c = Serial.read();
    switch (c) {
      case 'r': setAllLeds(CRGB::Red); Serial.println("[CMD] LEDs -> Rouge"); break;
      case 'g': setAllLeds(CRGB::Green); Serial.println("[CMD] LEDs -> Vert"); break;
      case 'b': setAllLeds(CRGB::Blue); Serial.println("[CMD] LEDs -> Bleu"); break;
      case 'w': FastLED.setBrightness(255); setAllLeds(CRGB::White); Serial.println("[CMD] LEDs -> Blanc 100%"); break;
      case 'o': clearLeds(); Serial.println("[CMD] LEDs -> Éteintes"); break;
      case 't': {
        if (hasSensor && lox.isRangeComplete()) {
          Serial.printf("[CMD ToF] Distance actuelle = %d mm\n", lox.readRange());
        } else {
          Serial.println("[CMD ToF] Mesure non disponible");
        }
        break;
      }
      case 'p': {
        RadioTestPacket pkt;
        pkt.seqNum = ++radioPingsSent;
        pkt.timestampMs = millis();
        esp_now_send(broadcastMac, (uint8_t*)&pkt, sizeof(pkt));
        Serial.printf("[CMD RADIO] Ping #%lu envoyé !\n", radioPingsSent);
        break;
      }
      case '?': setupTestAllInteractive(); break;
    }
  }

  // ToF continu
  if (hasSensor && lox.isRangeComplete()) {
    uint16_t d = lox.readRange();
    if (d <= TOF_HIT_DISTANCE_MM && d > 0) {
      setAllLeds(CRGB::White);
      delay(30);
      setAllLeds(CRGB::Green);
    }
  }

  // Dashboard mis à jour chaque seconde
  if (millis() - lastLogTime >= 1000) {
    lastLogTime = millis();
    float vBat = readBatteryVoltage();
    int pct = calculateBatteryPercentage(vBat);
    bool usb = isUsbConnected();
    bool sw = isPowerSwitchOn();

    Serial.printf("[MONITOR] Uptime: %s | Vbat: %.2fV (%d%%) | USB: %s | SW: %s | ToF: %s\n",
                  formatTime(millis() - testStartTime).c_str(),
                  vBat, pct,
                  usb ? "OUI" : "NON",
                  sw ? "ON" : "OFF",
                  hasSensor ? "OK" : "ERREUR");
  }
}

// =========================================================================================
//  8. SETUP ET LOOP PRINCIPAUX
// =========================================================================================
void setup() {
  Serial.begin(115200);
  
  // Délai de sécurité pour laisser le temps au port série USB de s'initialiser
  delay(2000);

  Serial.println("\n\n==================================================================");
  Serial.println("       LIGHT TRAINER V3 - BANC DE TEST & QUALIFICATION");
  Serial.println("==================================================================");

  // 1. Initialisation des broches matérielles
  pinMode(SW_PIN, INPUT_PULLDOWN);
  pinMode(CHARGE_PIN, INPUT);
  pinMode(BAT_ADC_PIN, INPUT);
  analogSetAttenuation(ADC_11db);

  // 2. Initialisation des LEDs FastLED
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(80);
  clearLeds();

  // 3. Initialisation du bus I2C et du capteur ToF
  Wire.begin(SDA_PIN, SCL_PIN);
  if (!lox.begin(0x29, false, &Wire, Adafruit_VL53L0X::VL53L0X_SENSE_HIGH_SPEED)) {
    hasSensor = false;
    Serial.println("[ATTENTION] Capteur VL53L0X non détecté !");
  } else {
    hasSensor = true;
    lox.startRangeContinuous();
    Serial.println("[OK] Capteur VL53L0X initialisé en mode continu rapide (50Hz).");
  }

  // 4. Démarrage du test sélectionné
  #if CURRENT_TEST == TEST_BATTERY_STRESS
    setupTestBatteryStress();
  #elif CURRENT_TEST == TEST_BATTERY_GAME_SIM
    setupTestBatteryGameSim();
  #elif CURRENT_TEST == TEST_TOF_BENCHMARK
    setupTestTofBenchmark();
  #elif CURRENT_TEST == TEST_RADIO_PING_RTT_RSSI
    setupTestRadioPingRssi();
  #elif CURRENT_TEST == TEST_DEEP_SLEEP_POWER
    setupTestDeepSleepPower();
  #elif CURRENT_TEST == TEST_ALL_INTERACTIVE
    setupTestAllInteractive();
  #else
    #error "Veuillez définir un test valide dans CURRENT_TEST !"
  #endif
}

void loop() {
  #if CURRENT_TEST == TEST_BATTERY_STRESS
    loopTestBatteryStress();
  #elif CURRENT_TEST == TEST_BATTERY_GAME_SIM
    loopTestBatteryGameSim();
  #elif CURRENT_TEST == TEST_TOF_BENCHMARK
    loopTestTofBenchmark();
  #elif CURRENT_TEST == TEST_RADIO_PING_RTT_RSSI
    loopTestRadioPingRssi();
  #elif CURRENT_TEST == TEST_DEEP_SLEEP_POWER
    loopTestDeepSleepPower();
  #elif CURRENT_TEST == TEST_ALL_INTERACTIVE
    loopTestAllInteractive();
  #endif
}
