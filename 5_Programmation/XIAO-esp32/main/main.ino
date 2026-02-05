#include "Config.h"
#include "Hardware.h"
#include "GameNetwork.h"
#include "GameEngine.h"
#include "WebConfig.h"

unsigned long lastDiscovery = 0;

void setup() {
  Serial.begin(115200);
  DEBUG_PRINT(DEBUG_VERBOSE, "Entering setup()");

  // Initialiser le Hardware minimal
  Hardware.init();

  WebConfig.loadSettings(Game.modes, 4);
  
  if (WebConfig.shouldEnterConfigMode()) {
    DEBUG_PRINT(DEBUG_INFO, "WebConfig mode triggered.");
    // --- MODE CONFIGURATION ---
    // On ne sort jamais de cette fonction, le système rebootera à la fin
    WebConfig.run();
  }

  // --- MODE JEU NORMAL ---
  DEBUG_PRINT(DEBUG_INFO, "Starting Game...");
  GameNetwork.init();
  Game.init();
  DEBUG_PRINT(DEBUG_INFO, "System Ready. ID: %llu", GameNetwork.myId);
}

void loop() {
  // Boucle de jeu classique
  Hardware.update();
  Hardware.checkSleep();
  Game.loop();

  // Découverte réseau périodique
  if (millis() - lastDiscovery > DISCOVERY_INT) {
    lastDiscovery = millis();
    DEBUG_PRINT(DEBUG_VERBOSE, "Sending discovery ping.");
    GameNetwork.broadcast(CMD_PING, 0);
  }
}