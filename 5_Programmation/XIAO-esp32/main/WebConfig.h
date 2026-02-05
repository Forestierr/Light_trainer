#ifndef WEBCONFIG_H
#define WEBCONFIG_H

#include <WebServer.h>
#include <Preferences.h>
#include "Config.h"
#include <WiFi.h>
#include <DNSServer.h> // Include for Captive Portal functionality

class WebConfigManager {
public:
  void init();                   // Initialisation basique
  bool shouldEnterConfigMode();  // Détection du geste d'entrée
  void run();                    // Boucle principale du mode config

  // Gestion des données
  void loadSettings(GameMode* modes, int count);

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
  GameMode* currentModes;
};

extern WebConfigManager WebConfig;

#endif