#ifndef WEBCONFIG_H
#define WEBCONFIG_H

#include <WebServer.h>
#include <Preferences.h>
#include "Config.h"
#include <WiFi.h>
#include <DNSServer.h> // Include for Captive Portal functionality

class WebConfigManager {
public:
  bool shouldEnterConfigMode();  // Détection du geste d'entrée
  void run();                    // Boucle principale du mode config

  // Gestion des données
  void loadSettings(GameMode (&modes)[4]);
  uint8_t getBrightness() const { return currentBrightness; }

private:
  WebServer server = WebServer(80);
  Preferences preferences;
  DNSServer dnsServer; // DNS server for captive portal

  // Pages Web
  void handleRoot();
  void handleSave();

  // Helpers
  String getHTML();
  String colorToHex(uint32_t color);
  uint32_t hexToColor(String hex);

  // Stockage local temporaire pour l'affichage
  GameMode* gameModesRef;
  uint8_t currentBrightness; // New member for brightness
};

extern WebConfigManager WebConfig;

#endif