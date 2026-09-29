#include "WebConfig.h"
#include "DashboardHtml.h"
#include "GameEngine.h"
#include "Hardware.h"
#include "GameNetwork.h"
#include <esp_wifi.h>

WebConfigManager WebConfig;

WebConfigManager::WebConfigManager() :
  server(80),
  currentBrightness(DEFAULT_BRIGHTNESS),
  gameModesRef(nullptr),
  apActive(false),
  routesRegistered(false)
{
}

void WebConfigManager::loadSettings(GameMode (&modes)[5]) {
  gameModesRef = modes;
  if (!preferences.begin("trainer-cfg", true)) {
    currentBrightness = DEFAULT_BRIGHTNESS;
    return;
  }

  currentBrightness = preferences.getUInt(NVS_KEY_BRIGHTNESS, DEFAULT_BRIGHTNESS);

  for (int i = 0; i < 5; i++) {
    String p = String(i);
    if (preferences.isKey(("c1_" + p).c_str())) {
      modes[i].color1 = preferences.getUInt(("c1_" + p).c_str());
      modes[i].color2 = preferences.getUInt(("c2_" + p).c_str());
      modes[i].delayMin = preferences.getUInt(("dmin_" + p).c_str());
      modes[i].delayMax = preferences.getUInt(("dmax_" + p).c_str());
    }
  }
  preferences.end();
}

void WebConfigManager::init() {
  if (routesRegistered) return;
  DEBUG_PRINT(DEBUG_INFO, "Enregistrement des routes Web Dashboard & API (Apple Design UI)...");

  // Forcer le DNS à répondre NoError (192.168.4.1) à TOUTES les requêtes
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);

  // Routes Web HTML & Sondes Captive Portal
  server.on("/", [this]() { handleRoot(); });
  server.on("/hotspot-detect.html", [this]() { handleRoot(); });
  server.on("/canonical.html", [this]() { handleRoot(); });
  server.on("/library/test/success.html", [this]() { handleRoot(); });
  server.on("/success.txt", [this]() { handleRoot(); });
  server.on("/ncsi.txt", [this]() { handleRoot(); });
  server.on("/connecttest.txt", [this]() { handleRoot(); });

  // Sondes Android : redirection 302 vers l'accueil du portail
  server.on("/generate_204", [this]() {
    server.sendHeader("Location", "http://192.168.4.1/", true);
    server.send(302, "text/plain", "Redirecting to captive portal");
  });
  server.on("/gen_204", [this]() {
    server.sendHeader("Location", "http://192.168.4.1/", true);
    server.send(302, "text/plain", "Redirecting to captive portal");
  });

  // Endpoints REST API (JSON)
  server.on("/api/config", HTTP_GET, [this]() { handleApiConfig(); });
  server.on("/api/status", HTTP_GET, [this]() { handleApiStatus(); });
  server.on("/api/pods", HTTP_GET, [this]() { handleApiPods(); });
  server.on("/api/logs", HTTP_GET, [this]() { handleApiLogs(); });
  server.on("/api/history", HTTP_GET, [this]() { handleApiHistory(); });
  server.on("/api/export", HTTP_GET, [this]() { handleApiExport(); });
  server.on("/api/game", HTTP_POST, [this]() { handleApiGameControl(); });
  server.on("/api/manual", HTTP_POST, [this]() { handleApiManualControl(); });
  server.on("/api/test", HTTP_POST, [this]() { handleApiTest(); });
  server.on("/api/settings", HTTP_POST, [this]() { handleApiSettings(); });

  server.onNotFound([this]() { handleNotFound(); });

  routesRegistered = true;
  apActive = false;
  DEBUG_PRINT(DEBUG_INFO, "Routes Web initialisées. SoftAP en attente du rôle Master.");
}

void WebConfigManager::startSoftAP() {
  if (apActive) return;
  DEBUG_PRINT(DEBUG_INFO, "Démarrage du SoftAP Master 'LightTrainer'...");

  WiFi.mode(WIFI_AP_STA);
  IPAddress apIP(192, 168, 4, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(apIP, apIP, subnet, IPAddress(192, 168, 4, 2), apIP);
  WiFi.softAP("LightTrainer", "", 1);

  dnsServer.start(53, "*", apIP);
  server.begin();
  apActive = true;

  // Verrouillage du canal WiFi sur le canal 1 pour ESP-NOW en mode mixte
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);

  Game.addLog("SoftAP Master activé sur 192.168.4.1 (Canal 1)", "WIFI");
}

void WebConfigManager::stopSoftAP() {
  if (!apActive) return;
  DEBUG_PRINT(DEBUG_INFO, "Arrêt du SoftAP (Passage en WIFI_STA pur)...");

  server.stop();
  dnsServer.stop();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  apActive = false;

  // Verrouillage du canal WiFi sur le canal 1
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);

  Game.addLog("SoftAP désactivé (Rôle Esclave, STA pur)", "WIFI");
}

void WebConfigManager::update() {
  if (!apActive) return;
  for (int i = 0; i < 3; i++) {
    dnsServer.processNextRequest();
  }
  server.handleClient();
}

void WebConfigManager::handleNotFound() {
  if (server.uri().startsWith("/api/")) {
    server.send(404, "application/json", "{\"error\":\"Not found\"}");
    return;
  }
  server.sendHeader("Location", "http://192.168.4.1/", true);
  server.send(302, "text/plain", "Redirecting to Light Trainer Dashboard...");
}

void WebConfigManager::handleApiConfig() {
  String json = "{\"version\":\"" FIRMWARE_VERSION "\",\"brightness\":";
  json += String(currentBrightness) + ",\"modes\":[";
  for (int i = 0; i < 5; i++) {
    if (i > 0) json += ",";
    char c1[10], c2[10];
    snprintf(c1, sizeof(c1), "#%06X", (unsigned int)Game.getGameMode(i).color1);
    snprintf(c2, sizeof(c2), "#%06X", (unsigned int)Game.getGameMode(i).color2);
    json += "{\"id\":" + String(i) + ",\"name\":\"" + String(Game.getModeDescription(i)) + "\",\"c1\":\"" + String(c1) + "\",\"c2\":\"" + String(c2) + "\",\"dmin\":" + String(Game.getGameMode(i).delayMin) + ",\"dmax\":" + String(Game.getGameMode(i).delayMax) + "}";
  }
  json += "],\"batHistory\":[";
#if ENABLE_BATTERY_MANAGEMENT
  const auto& bHist = Hardware.getBatteryHistory();
  for (size_t i = 0; i < bHist.size(); i++) {
    if (i > 0) json += ",";
    json += "{\"t\":" + String(bHist[i].timestampSec) + ",\"pct\":" + String(bHist[i].percentage) + ",\"v\":" + String(bHist[i].voltage, 2) + ",\"chg\":" + String(bHist[i].isCharging ? "true" : "false") + "}";
  }
#endif
  json += "]}";
  server.send(200, "application/json", json);
}

void WebConfigManager::handleApiStatus() {
  float vBat = Hardware.getBatteryVoltage();
  int batPct = Hardware.getBatteryPercentage();
  bool charging = Hardware.isCharging();

  char c1Hex[10], c2Hex[10];
  snprintf(c1Hex, sizeof(c1Hex), "#%06X", (unsigned int)Game.getGameMode(Game.getCurrentModeIndex()).color1);
  snprintf(c2Hex, sizeof(c2Hex), "#%06X", (unsigned int)Game.getGameMode(Game.getCurrentModeIndex()).color2);

  char json[768];
  snprintf(json, sizeof(json),
    "{\"state\":\"%s\",\"mode\":%d,\"modeDesc\":\"%s\",\"running\":%s,\"paused\":%s,\"manual\":%s,"
    "\"time\":%lu,\"hits\":%lu,\"lastRt\":%lu,\"bestRt\":%lu,\"avgRt\":%.1f,\"cadence\":%.1f,"
    "\"c1Col\":\"%s\",\"c2Col\":\"%s\",\"c1Hits\":%lu,\"c1BestRt\":%lu,\"c1AvgRt\":%.1f,"
    "\"c2Hits\":%lu,\"c2BestRt\":%lu,\"c2AvgRt\":%.1f,\"vBat\":%.2f,\"batPct\":%d,\"charging\":%s,"
    "\"tofDist\":%d,\"simonState\":\"%s\",\"simonRound\":%lu,\"simonBestRound\":%lu,"
    "\"simonStep\":%d,\"simonTotalSteps\":%d,\"simonSpeed\":%lu}",
    stateToString(Game.getCurrentState()),
    Game.getCurrentModeIndex(),
    Game.getModeDescription(Game.getCurrentModeIndex()),
    Game.isRunning() ? "true" : "false",
    Game.isPaused() ? "true" : "false",
    Game.isManualActive() ? "true" : "false",
    (unsigned long)Game.getElapsedTimeMs(),
    (unsigned long)Game.getTotalHits(),
    (unsigned long)Game.getLastReactionTimeMs(),
    (unsigned long)Game.getBestReactionTimeMs(),
    Game.getAvgReactionTimeMs(),
    Game.getCadenceBpm(),
    c1Hex, c2Hex,
    (unsigned long)Game.getC1Hits(),
    (unsigned long)Game.getC1BestRt(),
    Game.getC1AvgRt(),
    (unsigned long)Game.getC2Hits(),
    (unsigned long)Game.getC2BestRt(),
    Game.getC2AvgRt(),
    vBat, batPct,
    charging ? "true" : "false",
    Hardware.getLastValidDistance(),
    Game.getSimonStateString(),
    (unsigned long)Game.getSimonRound(),
    (unsigned long)Game.getSimonBestRound(),
    (int)(Game.getSimonInputStep() + 1),
    (int)Game.getSimonTotalSteps(),
    (unsigned long)Game.getSimonStepDurationMs()
  );

  server.send(200, "application/json", json);
}

void WebConfigManager::handleApiPods() {
  std::vector<NodeInfo> nodes = GameNetwork.getNodesInfo();

  String json = "{\"pods\":[";
  // Pod Master (toujours en ligne)
  json += "{";
  json += "\"id\":\"" + String(GameNetwork.getMyId()) + "\",";
  json += "\"name\":\"Pod 1 (Master)\",";
  json += "\"isMaster\":true,";
  json += "\"isTarget\":" + String(Game.getCurrentState() == CONTROLLER_TARGET ? "true" : "false") + ",";
  json += "\"rssi\":0,";
  json += "\"vBat\":" + String(Hardware.getBatteryVoltage(), 2) + ",";
  json += "\"batPct\":" + String(Hardware.getBatteryPercentage()) + ",";
  json += "\"charging\":" + String(Hardware.isCharging() ? "true" : "false") + ",";
  json += "\"online\":true,";
  json += "\"lastSeenSec\":0,";
  json += "\"mac\":\"" + GameNetwork.formatId(GameNetwork.getMyId()) + "\"";
  json += "}";

  // Pods esclaves
  int index = 2;
  unsigned long now = millis();
  for (const auto& node : nodes) {
    bool isOnline = (now - node.lastSeen <= 12000);
    uint32_t lastSeenSec = (now - node.lastSeen) / 1000;

    json += ",{";
    json += "\"id\":\"" + String(node.id) + "\",";
    json += "\"name\":\"Pod " + String(index++) + "\",";
    json += "\"isMaster\":false,";
    json += "\"isTarget\":" + String(node.isTarget ? "true" : "false") + ",";
    json += "\"rssi\":" + String(node.rssi) + ",";
    json += "\"vBat\":" + String(node.voltage, 2) + ",";
    json += "\"batPct\":" + String(node.batteryPct) + ",";
    json += "\"charging\":" + String(node.isCharging ? "true" : "false") + ",";
    json += "\"online\":" + String(isOnline ? "true" : "false") + ",";
    json += "\"lastSeenSec\":" + String(lastSeenSec) + ",";
    json += "\"mac\":\"" + GameNetwork.formatId(node.id) + "\"";
    json += "}";
  }
  json += "]}";

  server.send(200, "application/json", json);
}

void WebConfigManager::handleApiLogs() {
  const std::vector<LogEntry>& logs = Game.getLogs();
  String json = "{\"logs\":[";
  for (size_t i = 0; i < logs.size(); i++) {
    if (i > 0) json += ",";
    json += "{";
    json += "\"t\":" + String(logs[i].timestampMs) + ",";
    json += "\"lvl\":\"" + logs[i].level + "\",";
    json += "\"msg\":\"" + logs[i].message + "\"";
    json += "}";
  }
  json += "]}";
  server.send(200, "application/json", json);
}

void WebConfigManager::handleApiHistory() {
  const std::vector<HitRecord>& history = Game.getHitHistory();
  String json = "{\"hits\":[";
  for (size_t i = 0; i < history.size(); i++) {
    if (i > 0) json += ",";
    char colHex[10];
    snprintf(colHex, sizeof(colHex), "#%06X", (unsigned int)history[i].color);
    json += "{";
    json += "\"num\":" + String(history[i].hitNum) + ",";
    json += "\"pod\":\"" + GameNetwork.formatId(history[i].podId) + "\",";
    json += "\"col\":\"" + String(colHex) + "\",";
    json += "\"rt\":" + String(history[i].reactionTimeMs) + ",";
    json += "\"t\":" + String(history[i].timestampMs);
    json += "}";
  }
  json += "]}";
  server.send(200, "application/json", json);
}

void WebConfigManager::handleApiExport() {
  String csv = Game.generateCsvReport();
  server.sendHeader("Content-Disposition", "attachment; filename=\"LightTrainer_Session.csv\"");
  server.send(200, "text/csv", csv);
}

void WebConfigManager::handleApiGameControl() {
  if (server.hasArg("plain") == false) {
    server.send(400, "text/plain", "Body missing");
    return;
  }
  String body = server.arg("plain");

  if (body.indexOf("START") >= 0) Game.startGame();
  else if (body.indexOf("PAUSE") >= 0) Game.pauseGame();
  else if (body.indexOf("RESUME") >= 0) Game.resumeGame();
  else if (body.indexOf("STOP") >= 0) Game.stopGame();
  else if (body.indexOf("CLEAR_HISTORY") >= 0) Game.clearHitHistory();
  else if (body.indexOf("SET_MODE") >= 0) {
    int idx = body.indexOf("\"mode\":");
    if (idx >= 0) {
      int mode = body.substring(idx + 7).toInt();
      Game.setMode(mode);
    }
  }

  server.send(200, "application/json", "{\"status\":\"OK\"}");
}

void WebConfigManager::handleApiManualControl() {
  if (server.hasArg("plain") == false) {
    server.send(400, "text/plain", "Body missing");
    return;
  }
  String body = server.arg("plain");

  if (body.indexOf("ALL_OFF") >= 0) {
    Game.turnOffAllPods();
  } else if (body.indexOf("ALL_ON") >= 0) {
    int cIdx = body.indexOf("\"color\":\"");
    uint32_t col = 0x00FF00;
    if (cIdx >= 0) {
      String colStr = body.substring(cIdx + 9, body.indexOf("\"", cIdx + 9));
      col = strtoul(colStr.c_str(), NULL, 16);
    }
    Game.setAllPodsColor(col);
  } else if (body.indexOf("SET_POD") >= 0) {
    int idIdx = body.indexOf("\"podId\":\"");
    int cIdx = body.indexOf("\"color\":\"");
    if (idIdx >= 0 && cIdx >= 0) {
      String idStr = body.substring(idIdx + 9, body.indexOf("\"", idIdx + 9));
      String colStr = body.substring(cIdx + 9, body.indexOf("\"", cIdx + 9));
      uint64_t targetId = strtoull(idStr.c_str(), NULL, 10);
      uint32_t col = strtoul(colStr.c_str(), NULL, 16);
      Game.setManualPod(targetId, col);
    }
  }

  server.send(200, "application/json", "{\"status\":\"OK\"}");
}

void WebConfigManager::handleApiTest() {
  if (server.hasArg("plain") == false) {
    server.send(400, "text/plain", "Body missing");
    return;
  }
  String body = server.arg("plain");

  if (body.indexOf("LED") >= 0) {
    Hardware.rainbowBlink(3, 80);
    Game.addLog("Test visuel LED exécuté", "TEST");
  } else if (body.indexOf("PING") >= 0) {
    Game.clearPingResults();
    unsigned long pingStart = millis();
    Game.setPingInProgress(true, pingStart);
    GameNetwork.broadcast(CMD_PING, 1);
    Game.addLog("Test Ping Flotte initié...", "TEST");

    // Attente brève non-bloquante pour récolter les réponses
    unsigned long waitStart = millis();
    while (millis() - waitStart < 300) {
      Game.processNetwork();
      delay(10);
    }
    Game.setPingInProgress(false, 0);

    const auto& results = Game.getPingResults();
    String json = "{\"status\":\"OK\",\"count\":" + String(results.size()) + ",\"pods\":[";
    for (size_t i = 0; i < results.size(); i++) {
      if (i > 0) json += ",";
      json += "{";
      json += "\"id\":\"" + String(results[i].podId) + "\",";
      json += "\"name\":\"Pod " + String(i + 2) + "\",";
      json += "\"mac\":\"" + GameNetwork.formatId(results[i].podId) + "\",";
      json += "\"rtt\":" + String(results[i].rttMs) + ",";
      json += "\"rssi\":" + String(results[i].rssi) + ",";
      json += "\"batPct\":" + String(results[i].batteryPct);
      json += "}";
    }
    json += "]}";
    server.send(200, "application/json", json);
    return;
  } else if (body.indexOf("CLEAR_LOGS") >= 0) {
    Game.clearLogs();
  }

  server.send(200, "application/json", "{\"status\":\"OK\"}");
}

void WebConfigManager::handleApiSettings() {
  if (server.hasArg("plain") == false) {
    server.send(400, "text/plain", "Body missing");
    return;
  }
  String body = server.arg("plain");

  // Commande de redémarrage du pod
  if (body.indexOf("RESTART") >= 0) {
    Game.addLog("Redémarrage du pod demandé via l'interface...", "SYSTEM");
    server.send(200, "application/json", "{\"status\":\"RESTARTING\"}");
    delay(500);
    ESP.restart();
    return;
  }

  if (!preferences.begin("trainer-cfg", false)) {
    server.send(500, "text/plain", "NVS Error");
    return;
  }

  // Luminosité
  int bIdx = body.indexOf("\"brightness\":");
  if (bIdx >= 0) {
    uint8_t newBright = body.substring(bIdx + 13).toInt();
    newBright = constrain(newBright, 25, 255);
    currentBrightness = newBright;
    Hardware.setBrightness(currentBrightness);
    preferences.putUInt(NVS_KEY_BRIGHTNESS, currentBrightness);
    Game.addLog("Luminosité enregistrée : " + String(currentBrightness), "CONFIG");
  }

  // Configuration d'un mode de jeu
  int mIdx = body.indexOf("\"mode\":");
  if (mIdx >= 0 && gameModesRef != nullptr) {
    int modeId = body.substring(mIdx + 7).toInt();
    if (modeId >= 0 && modeId < 5) {
      int c1Idx = body.indexOf("\"c1\":\"", mIdx);
      int c2Idx = body.indexOf("\"c2\":\"", mIdx);
      int dminIdx = body.indexOf("\"dmin\":", mIdx);
      int dmaxIdx = body.indexOf("\"dmax\":", mIdx);

      if (c1Idx >= 0) {
        String c1Str = body.substring(c1Idx + 6, body.indexOf("\"", c1Idx + 6));
        if (c1Str.startsWith("#")) c1Str = c1Str.substring(1);
        gameModesRef[modeId].color1 = strtoul(c1Str.c_str(), NULL, 16);
        preferences.putUInt(("c1_" + String(modeId)).c_str(), gameModesRef[modeId].color1);
      }
      if (c2Idx >= 0) {
        String c2Str = body.substring(c2Idx + 6, body.indexOf("\"", c2Idx + 6));
        if (c2Str.startsWith("#")) c2Str = c2Str.substring(1);
        gameModesRef[modeId].color2 = strtoul(c2Str.c_str(), NULL, 16);
        preferences.putUInt(("c2_" + String(modeId)).c_str(), gameModesRef[modeId].color2);
      }
      if (dminIdx >= 0) {
        int dmin = body.substring(dminIdx + 7).toInt();
        if (modeId == 4) {
          gameModesRef[modeId].delayMin = constrain(dmin, 200, 2000); // Base tempo ms
        } else {
          gameModesRef[modeId].delayMin = constrain(dmin, 0, 30);      // Sec
        }
        preferences.putUInt(("dmin_" + String(modeId)).c_str(), gameModesRef[modeId].delayMin);
      }
      if (dmaxIdx >= 0) {
        int dmax = body.substring(dmaxIdx + 7).toInt();
        if (modeId == 4) {
          gameModesRef[modeId].delayMax = constrain(dmax, 1, 25);       // % accélération par manche
        } else {
          gameModesRef[modeId].delayMax = constrain(dmax, (int)gameModesRef[modeId].delayMin, 30);
        }
        preferences.putUInt(("dmax_" + String(modeId)).c_str(), gameModesRef[modeId].delayMax);
      }
      Game.addLog("Configuration Mode " + String(modeId) + " enregistrée en NVS", "CONFIG");
    }
  }

  preferences.end();
  server.send(200, "application/json", "{\"status\":\"OK\"}");
}

// -----------------------------------------------------------------------------------------
// GESTION DU DASHBOARD WEB (COMPRESSION GZIP PROGMEM - DashboardHtml.h)
// -----------------------------------------------------------------------------------------
void WebConfigManager::handleRoot() {
  server.sendHeader("Content-Encoding", "gzip");
  server.sendHeader("Cache-Control", "public, max-age=3600");
  server.send_P(200, "text/html", (const char*)DASHBOARD_HTML_GZ, DASHBOARD_HTML_GZ_LEN);
}
