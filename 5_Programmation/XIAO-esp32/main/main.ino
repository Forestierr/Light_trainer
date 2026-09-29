#include "Config.h"
#include "Hardware.h"
#include "GameNetwork.h"
#include "GameEngine.h"
#include "WebConfig.h"

unsigned long lastDiscovery = 0;

void setup() {
  Serial.begin(115200);
  
  // --- SÉCURITÉ ANTI-BRICK / FLASH USB ---
  // Délai de sécurité au démarrage avec port USB ouvert pour le téléversement
  delay(3000);
  DEBUG_PRINT(DEBUG_VERBOSE, "Entering setup()");

  // 1. Vérification de l'interrupteur ON/OFF après le délai de sécurité
  pinMode(SW_PIN, INPUT_PULLDOWN);
  pinMode(CHARGE_PIN, INPUT); // Haute impédance

  // Si l'interrupteur est sur OFF ET que le câble USB n'est pas branché -> Deep Sleep
  if (digitalRead(SW_PIN) == LOW && digitalRead(CHARGE_PIN) == LOW) {
    DEBUG_PRINT(DEBUG_INFO, "Switch OFF et pas d'USB -> Passage en Deep Sleep.");
    Hardware.goToDeepSleep();
  }

  // 2. Initialiser le Hardware
  Hardware.init();

  // 3. Charger les réglages persistants NVS
  WebConfig.loadSettings(Game.getAllGameModes());
  Hardware.setBrightness(WebConfig.getBrightness());

  // 4. Initialiser le réseau ESP-NOW (mixte AP_STA)
  DEBUG_PRINT(DEBUG_INFO, "Initialisation GameNetwork...");
  GameNetwork.init();

  // 5. Initialiser le Moteur de Jeu
  DEBUG_PRINT(DEBUG_INFO, "Initialisation GameEngine...");
  Game.init();

  // 6. Démarrer le Serveur Web & Portail Captif (Non-bloquant)
  DEBUG_PRINT(DEBUG_INFO, "Démarrage Web Dashboard & Portail Captif...");
  WebConfig.init();

  DEBUG_PRINT(DEBUG_INFO, "Système Prêt ! ID Pod : %llu", GameNetwork.getMyId());

  // Première diffusion de télémétrie et découverte
  uint8_t initPct = 85;
  float initVolt = 3.9f;
  bool initChg = false;
#if ENABLE_BATTERY_MANAGEMENT
  initPct = (uint8_t)Hardware.getBatteryPercentage();
  initVolt = Hardware.getBatteryVoltage();
  initChg = Hardware.isCharging();
#endif
  uint16_t initVCent = (uint16_t)(initVolt * 100.0f);
  uint32_t initTelem = (initPct & 0xFF) | ((initVCent & 0xFFFF) << 8) | ((initChg ? 1 : 0) << 24);
  GameNetwork.broadcast(CMD_POD_TELEMETRY, initTelem);
}

void loop() {
  // 1. Vérification de l'interrupteur mécanique ON/OFF
  Hardware.checkPowerSwitch();

  // 2. Boucle de jeu et matériel
  Hardware.update();
#if ENABLE_BATTERY_MANAGEMENT
  Hardware.updateBatteryLED(); // Mise à jour état de charge / batterie faible
#endif
  Hardware.checkSleep();
  Game.loop();
  GameNetwork.update();

  // 3. Gestion du Serveur Web & Portail Captif (Non-bloquant)
  WebConfig.update();

  // 4. Découverte réseau et télémétrie batterie périodique
  if (millis() - lastDiscovery > DISCOVERY_INT) {
    lastDiscovery = millis();
    uint8_t pct = 85;
    float volt = 3.9f;
    bool chg = false;
#if ENABLE_BATTERY_MANAGEMENT
    pct = (uint8_t)Hardware.getBatteryPercentage();
    volt = Hardware.getBatteryVoltage();
    chg = Hardware.isCharging();
#endif
    uint16_t vCent = (uint16_t)(volt * 100.0f);
    uint32_t telem = (pct & 0xFF) | ((vCent & 0xFFFF) << 8) | ((chg ? 1 : 0) << 24);
    DEBUG_PRINT(DEBUG_VERBOSE, "Diffusion télémétrie batterie: %d%% (%.2fV)", pct, volt);
    GameNetwork.broadcast(CMD_POD_TELEMETRY, telem);
  }
}