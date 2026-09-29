#ifndef WEBCONFIG_H
#define WEBCONFIG_H

#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include "Config.h"

class WebConfigManager {
public:
  WebConfigManager();
  void init();
  void startSoftAP();
  void stopSoftAP();
  bool isApActive() const { return apActive; }
  void update(); // Appel non-bloquant dans loop()

  // Gestion des paramètres NVS
  void loadSettings(GameMode (&modes)[5]);
  uint8_t getBrightness() const { return currentBrightness; }
  void setBrightness(uint8_t b) { currentBrightness = b; }

private:
  WebServer server;
  DNSServer dnsServer;
  Preferences preferences;

  bool apActive = false;
  bool routesRegistered = false;
  uint8_t currentBrightness;
  GameMode* gameModesRef;

  // Handlers Web
  void handleRoot();
  void handleApiConfig();
  void handleApiStatus();
  void handleApiPods();
  void handleApiLogs();
  void handleApiHistory();
  void handleApiExport();
  void handleApiGameControl();
  void handleApiManualControl();
  void handleApiTest();
  void handleApiSettings();
  void handleNotFound();
};

extern WebConfigManager WebConfig;

#endif