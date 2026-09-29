#include "WebConfig.h"
#include "GameEngine.h"
#include "Hardware.h"
#include "GameNetwork.h"

WebConfigManager WebConfig;

WebConfigManager::WebConfigManager() :
  server(80),
  currentBrightness(DEFAULT_BRIGHTNESS),
  gameModesRef(nullptr)
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
  DEBUG_PRINT(DEBUG_INFO, "Starting Web Dashboard & Captive Portal (Apple Design UI)...");

  IPAddress apIP(192, 168, 4, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(apIP, apIP, subnet, IPAddress(192, 168, 4, 2), apIP);
  WiFi.softAP("LightTrainer", "", 1);

  // Forcer le DNS à répondre NoError (192.168.4.1) à TOUTES les requêtes (y compris IPv6/HTTPS)
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(53, "*", apIP);

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

  server.begin();
  Game.addLog("Dashboard Web démarré sur 192.168.4.1", "INFO");
}

void WebConfigManager::update() {
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

void WebConfigManager::handleApiStatus() {
  float vBat = Hardware.getBatteryVoltage();
  int batPct = Hardware.getBatteryPercentage();
  bool charging = Hardware.isCharging();

  char c1Hex[10], c2Hex[10];
  snprintf(c1Hex, sizeof(c1Hex), "#%06X", (unsigned int)Game.getGameMode(Game.getCurrentModeIndex()).color1);
  snprintf(c2Hex, sizeof(c2Hex), "#%06X", (unsigned int)Game.getGameMode(Game.getCurrentModeIndex()).color2);

  String json = "{";
  json += "\"state\":\"" + String(stateToString(Game.getCurrentState())) + "\",";
  json += "\"mode\":" + String(Game.getCurrentModeIndex()) + ",";
  json += "\"modeDesc\":\"" + String(Game.getModeDescription(Game.getCurrentModeIndex())) + "\",";
  json += "\"running\":" + String(Game.isRunning() ? "true" : "false") + ",";
  json += "\"paused\":" + String(Game.isPaused() ? "true" : "false") + ",";
  json += "\"manual\":" + String(Game.isManualActive() ? "true" : "false") + ",";
  json += "\"time\":" + String(Game.getElapsedTimeMs()) + ",";
  json += "\"hits\":" + String(Game.getTotalHits()) + ",";
  json += "\"lastRt\":" + String(Game.getLastReactionTimeMs()) + ",";
  json += "\"bestRt\":" + String(Game.getBestReactionTimeMs()) + ",";
  json += "\"avgRt\":" + String(Game.getAvgReactionTimeMs(), 1) + ",";
  json += "\"cadence\":" + String(Game.getCadenceBpm(), 1) + ",";
  json += "\"c1Col\":\"" + String(c1Hex) + "\",";
  json += "\"c2Col\":\"" + String(c2Hex) + "\",";
  json += "\"c1Hits\":" + String(Game.getC1Hits()) + ",";
  json += "\"c1BestRt\":" + String(Game.getC1BestRt()) + ",";
  json += "\"c1AvgRt\":" + String(Game.getC1AvgRt(), 1) + ",";
  json += "\"c2Hits\":" + String(Game.getC2Hits()) + ",";
  json += "\"c2BestRt\":" + String(Game.getC2BestRt()) + ",";
  json += "\"c2AvgRt\":" + String(Game.getC2AvgRt(), 1) + ",";
  json += "\"vBat\":" + String(vBat, 2) + ",";
  json += "\"batPct\":" + String(batPct) + ",";
  json += "\"charging\":" + String(charging ? "true" : "false") + ",";
  json += "\"tofDist\":" + String(Hardware.getLastValidDistance()) + ",";

  // Statuts spécifiques au Jeu Simon (Mode 4)
  json += "\"simonState\":\"" + String(Game.getSimonStateString()) + "\",";
  json += "\"simonRound\":" + String(Game.getSimonRound()) + ",";
  json += "\"simonBestRound\":" + String(Game.getSimonBestRound()) + ",";
  json += "\"simonStep\":" + String(Game.getSimonInputStep() + 1) + ",";
  json += "\"simonTotalSteps\":" + String(Game.getSimonTotalSteps()) + ",";
  json += "\"simonSpeed\":" + String(Game.getSimonStepDurationMs()) + ",";

  // Configuration actuelle des 5 modes de jeu
  json += "\"modes\":[";
  for (int i = 0; i < 5; i++) {
    if (i > 0) json += ",";
    char c1[10], c2[10];
    snprintf(c1, sizeof(c1), "#%06X", (unsigned int)Game.getGameMode(i).color1);
    snprintf(c2, sizeof(c2), "#%06X", (unsigned int)Game.getGameMode(i).color2);
    json += "{";
    json += "\"id\":" + String(i) + ",";
    json += "\"name\":\"" + String(Game.getModeDescription(i)) + "\",";
    json += "\"c1\":\"" + String(c1) + "\",";
    json += "\"c2\":\"" + String(c2) + "\",";
    json += "\"dmin\":" + String(Game.getGameMode(i).delayMin) + ",";
    json += "\"dmax\":" + String(Game.getGameMode(i).delayMax);
    json += "}";
  }
  json += "],";

  // Historique de télémétrie batterie style iOS
  json += "\"batHistory\":[";
#if ENABLE_BATTERY_MANAGEMENT
  const auto& bHist = Hardware.getBatteryHistory();
  for (size_t i = 0; i < bHist.size(); i++) {
    if (i > 0) json += ",";
    json += "{\"t\":" + String(bHist[i].timestampSec) + ",";
    json += "\"pct\":" + String(bHist[i].percentage) + ",";
    json += "\"v\":" + String(bHist[i].voltage, 2) + ",";
    json += "\"chg\":" + String(bHist[i].isCharging ? "true" : "false") + "}";
  }
#endif
  json += "]";

  json += "}";

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
// CODE HTML5 / CSS3 / JAVASCRIPT — SYNTHÈSE ATHLÉTIQUE HAUTE PRÉCISION (PROGMEM FLASH)
// -----------------------------------------------------------------------------------------
static const char DASHBOARD_HTML[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <meta name="apple-mobile-web-app-capable" content="yes">
  <meta name="apple-mobile-web-app-status-bar-style" content="black-translucent">
  <title>Light Trainer</title>
  <style>
    :root {
      --bg: #000000;
      --surface-1: #1c1c1e;
      --surface-2: #2c2c2e;
      --surface-3: #3a3a3c;
      --separator: rgba(84, 84, 88, 0.4);
      --border-light: rgba(255, 255, 255, 0.08);
      --accent-blue: #0a84ff;
      --accent-green: #30d158;
      --accent-orange: #ff9f0a;
      --accent-yellow: #ffd60a;
      --accent-red: #ff453a;
      --accent-purple: #bf5af2;
      --accent-teal: #30b0c7;
      --accent-gray: #8e8e93;
      --text-1: #ffffff;
      --text-2: rgba(235, 235, 245, 0.6);
      --text-3: rgba(235, 235, 245, 0.35);
    }
    * { box-sizing: border-box; margin: 0; padding: 0; -webkit-tap-highlight-color: transparent; }
    body {
      background-color: var(--bg);
      color: var(--text-1);
      font-family: -apple-system, BlinkMacSystemFont, "SF Pro Text", "SF Pro Display", system-ui, sans-serif;
      padding-bottom: 40px;
      min-height: 100vh;
      overflow-x: hidden;
    }

    header {
      position: sticky; top: 0; z-index: 200; padding: 12px 18px;
      background: rgba(0, 0, 0, 0.85); backdrop-filter: blur(20px);
      -webkit-backdrop-filter: blur(20px);
      border-bottom: 0.5px solid var(--separator);
      display: flex; justify-content: space-between; align-items: center;
    }
    .brand { display: flex; align-items: center; gap: 9px; font-weight: 600; font-size: 1.05rem; letter-spacing: -0.02em; }
    .brand-icon { width: 26px; height: 26px; background: var(--accent-blue); border-radius: 6px; display: flex; align-items: center; justify-content: center; font-size: 0.75rem; font-weight: 700; color: #ffffff; }
    .status-capsule { display: flex; gap: 6px; align-items: center; background: var(--surface-1); padding: 5px 10px; border-radius: 20px; border: 1px solid var(--border-light); font-size: 0.75rem; font-weight: 500; color: var(--text-2); }
    .status-dot { width: 7px; height: 7px; border-radius: 50%; background: var(--accent-green); }

    .segmented-nav { margin: 12px 16px 14px; background: rgba(118, 118, 128, 0.24); padding: 2px; border-radius: 9px; display: flex; }
    .segment-btn { flex: 1; padding: 8px 4px; text-align: center; font-size: 0.82rem; font-weight: 500; color: var(--text-2); border-radius: 7px; cursor: pointer; border: none; background: transparent; transition: all 0.15s ease-out; }
    .segment-btn:active { transform: scale(0.98); }
    .segment-btn.active { background: rgba(255, 255, 255, 0.16); color: var(--text-1); font-weight: 600; box-shadow: 0 3px 8px rgba(0,0,0,0.15); }

    .container { padding: 0 16px; max-width: 580px; margin: 0 auto; }
    .tab-content { display: none; }
    .tab-content.active { display: block; animation: fadeIn 0.2s ease-out; }
    @keyframes fadeIn { from { opacity: 0; } to { opacity: 1; } }

    .card { background: var(--surface-1); border: 1px solid var(--border-light); border-radius: 14px; padding: 16px; margin-bottom: 14px; }
    .card-header { display: flex; justify-content: space-between; align-items: center; margin-bottom: 12px; }
    .card-title { font-size: 0.75rem; font-weight: 600; color: var(--text-3); text-transform: uppercase; letter-spacing: 0.04em; }
    .card-badge { font-size: 0.72rem; font-weight: 600; color: var(--accent-yellow); background: rgba(255, 214, 10, 0.12); padding: 3px 8px; border-radius: 6px; }

    .timer-hero { text-align: center; padding: 8px 0 14px; }
    .timer-digits { font-size: 3.6rem; font-weight: 700; letter-spacing: -0.03em; font-variant-numeric: tabular-nums; line-height: 1; color: var(--text-1); }
    .timer-sub { font-size: 0.82rem; font-weight: 500; color: var(--text-2); margin-top: 6px; }

    .button-stack { display: flex; gap: 8px; margin-top: 6px; }
    .btn { flex: 1; padding: 14px; border-radius: 12px; font-size: 0.95rem; font-weight: 600; cursor: pointer; border: 1px solid transparent; display: flex; align-items: center; justify-content: center; gap: 6px; transition: transform 100ms ease-out, opacity 100ms ease-out; }
    .btn:active { transform: scale(0.97); opacity: 0.85; }
    .btn-primary { background: var(--accent-green); color: #000000; font-weight: 700; }
    .btn-pause { background: var(--surface-2); color: var(--accent-yellow); border: 1px solid var(--border-light); }
    .btn-stop { background: var(--surface-2); color: var(--accent-red); border: 1px solid var(--border-light); }
    .btn-pill { padding: 5px 12px; border-radius: 14px; font-size: 0.72rem; font-weight: 600; background: var(--surface-2); color: var(--text-1); border: 1px solid var(--border-light); cursor: pointer; transition: transform 100ms ease-out; }
    .btn-pill:active { transform: scale(0.96); }

    .select-wrap select { width: 100%; padding: 12px 14px; background: var(--surface-2); border: 1px solid var(--border-light); border-radius: 10px; color: var(--text-1); font-size: 0.9rem; font-weight: 500; outline: none; cursor: pointer; }

    /* Performance Metrics Grid */
    .metrics-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 8px; margin-top: 4px; }
    .metric-cell { background: var(--surface-2); border: 1px solid var(--border-light); border-radius: 12px; padding: 12px; text-align: left; }
    .metric-label { font-size: 0.68rem; font-weight: 600; color: var(--text-3); text-transform: uppercase; letter-spacing: 0.03em; }
    .metric-value { font-size: 1.5rem; font-weight: 700; letter-spacing: -0.02em; margin-top: 3px; color: var(--text-1); font-variant-numeric: tabular-nums; }
    .metric-unit { font-size: 0.75rem; font-weight: 500; color: var(--text-2); margin-left: 2px; }

    /* Dual Player Score Card (Modes 1 & 3) */
    .duel-card { background: var(--surface-2); border: 1px solid var(--border-light); border-radius: 12px; padding: 12px; margin-bottom: 10px; }
    .duel-cols { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; }
    .duel-col { padding: 10px; border-radius: 9px; background: rgba(0,0,0,0.2); border-left: 3.5px solid var(--border-light); }
    .duel-name { font-size: 0.75rem; font-weight: 600; text-transform: uppercase; display: flex; align-items: center; gap: 6px; }
    .duel-dot { width: 8px; height: 8px; border-radius: 50%; }
    .duel-hits { font-size: 1.7rem; font-weight: 700; font-variant-numeric: tabular-nums; margin: 3px 0 2px; }
    .duel-meta { font-size: 0.72rem; color: var(--text-2); }
    .duel-bar-wrap { width: 100%; height: 6px; background: rgba(255,255,255,0.1); border-radius: 3px; overflow: hidden; margin-top: 10px; display: flex; }
    .duel-bar-c1 { height: 100%; background: var(--accent-red); transition: width 0.3s; }
    .duel-bar-c2 { height: 100%; background: var(--accent-green); transition: width 0.3s; }

    /* High Precision Chart Container */
    .chart-wrapper {
      position: relative;
      width: 100%;
      height: 180px;
      background: rgba(0, 0, 0, 0.4);
      border-radius: 12px;
      margin-top: 12px;
      padding: 10px 10px 10px 48px;
      border: 1px solid var(--border-light);
    }
    .chart-y-axis {
      position: absolute;
      left: 6px;
      top: 10px;
      bottom: 24px;
      width: 38px;
      display: flex;
      flex-direction: column;
      justify-content: space-between;
      font-size: 0.62rem;
      font-weight: 500;
      color: var(--text-3);
      text-align: right;
    }
    .chart-canvas-box { width: 100%; height: 100%; position: relative; }
    canvas { width: 100%; height: 100%; }
    .chart-legend { display: flex; justify-content: center; gap: 16px; font-size: 0.7rem; font-weight: 500; color: var(--text-2); margin-top: 8px; }
    .legend-item { display: flex; align-items: center; gap: 5px; }

    /* Palette de couleurs */
    .palette { display: flex; justify-content: space-between; padding: 4px 6px; margin-bottom: 6px; }
    .color-chip { width: 36px; height: 36px; border-radius: 50%; border: 3px solid transparent; cursor: pointer; transition: transform 0.15s ease-out; }
    .color-chip:active { transform: scale(0.95); }
    .color-chip.selected { border-color: #ffffff; transform: scale(1.15); }

    .pods-fleet-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 8px; margin-top: 10px; }
    .pod-card { background: var(--surface-2); border: 1.5px solid var(--border-light); border-radius: 14px; padding: 14px 10px; text-align: center; cursor: pointer; transition: transform 100ms ease-out; }
    .pod-card:active { transform: scale(0.97); }
    .pod-card.is-active-target { border-color: var(--accent-green); background: rgba(48, 209, 88, 0.12); }
    .pod-avatar { width: 40px; height: 40px; border-radius: 50%; margin: 0 auto 8px; background: var(--surface-3); display: flex; align-items: center; justify-content: center; font-weight: 700; font-size: 1rem; border: 1px solid var(--border-light); }
    .pod-title { font-size: 0.88rem; font-weight: 600; }
    .pod-subtitle { font-size: 0.7rem; color: var(--text-2); margin-top: 2px; }

    /* iOS Inset Grouped Settings Hierarchy */
    .ios-section-header { font-size: 0.75rem; font-weight: 400; color: var(--accent-gray); text-transform: uppercase; letter-spacing: -0.01em; padding: 0 16px 6px; }
    .ios-group { background: var(--surface-1); border: 1px solid var(--border-light); border-radius: 12px; margin-bottom: 20px; overflow: hidden; }
    .ios-row { display: flex; align-items: center; justify-content: space-between; padding: 11px 16px; border-bottom: 0.5px solid var(--separator); cursor: pointer; transition: background 0.1s; color: inherit; text-decoration: none; }
    .ios-row:active { background: rgba(255, 255, 255, 0.06); }
    .ios-row:last-child { border-bottom: none; }
    .ios-row-left { display: flex; align-items: center; gap: 12px; flex: 1; }
    .ios-icon-box { width: 29px; height: 29px; border-radius: 7px; display: flex; align-items: center; justify-content: center; color: #ffffff; flex-shrink: 0; }
    .ios-row-text { display: flex; flex-direction: column; }
    .ios-row-title { font-size: 0.92rem; font-weight: 500; color: var(--text-1); }
    .ios-row-subtitle { font-size: 0.72rem; color: var(--text-2); margin-top: 1px; }
    .ios-row-right { display: flex; align-items: center; gap: 6px; color: var(--text-3); font-size: 0.85rem; font-weight: 400; }
    .ios-chevron { font-size: 1.15rem; color: rgba(235, 235, 245, 0.25); font-weight: 300; }
    .ios-back-btn { display: inline-flex; align-items: center; gap: 4px; color: var(--accent-blue); font-size: 0.95rem; font-weight: 500; cursor: pointer; border: none; background: transparent; padding: 4px 0; margin-bottom: 12px; }
    .ios-back-btn:active { opacity: 0.7; }

    /* Apple iOS Battery Monitor Card */
    .ios-battery-card { background: var(--surface-1); border: 1px solid var(--border-light); border-radius: 14px; padding: 16px; margin-bottom: 18px; }
    .ios-battery-header { font-size: 0.75rem; font-weight: 600; color: var(--accent-gray); text-transform: uppercase; letter-spacing: 0.04em; margin-bottom: 8px; }
    .ios-bat-canvas-container { position: relative; width: 100%; height: 160px; }
    .ios-bat-y-labels { position: absolute; right: 0; top: 4px; bottom: 24px; width: 34px; display: flex; flex-direction: column; justify-content: space-between; font-size: 0.65rem; font-weight: 500; color: var(--accent-gray); text-align: right; }

    .console-term { background: #000000; border: 1px solid var(--separator); border-radius: 10px; padding: 12px; font-family: "SF Mono", Monaco, Consolas, monospace; font-size: 0.72rem; height: 180px; overflow-y: auto; line-height: 1.45; color: #e0e0e0; }
    .log-tag { font-weight: 600; border-radius: 3px; padding: 1px 4px; margin-right: 4px; }
    .tag-INFO { color: var(--accent-blue); }
    .tag-HIT { color: var(--accent-green); }
    .tag-GAME { color: var(--accent-yellow); }
    .tag-MANUAL { color: var(--accent-purple); }
    .tag-ERROR { color: var(--accent-red); font-weight: 700; }
    .tag-SYSTEM { color: var(--accent-teal); }

    input[type=range] { width: 100%; height: 6px; background: var(--surface-3); border-radius: 4px; outline: none; -webkit-appearance: none; margin: 10px 0; }
    input[type=range]::-webkit-slider-thumb { -webkit-appearance: none; width: 24px; height: 24px; border-radius: 50%; background: #FFFFFF; box-shadow: 0 2px 6px rgba(0,0,0,0.4); cursor: pointer; }
  </style>
</head>
<body>

<header>
  <div class="brand">
    <div class="brand-icon">LT</div>
    <div>Light Trainer</div>
  </div>
  <div class="status-capsule">
    <div class="status-dot"></div>
    <span id="headerFleet">1 Pod</span>
    <span style="color:var(--separator)">|</span>
    <span id="headerBat">--%</span>
  </div>
</header>

<div class="segmented-nav">
  <button class="segment-btn active" onclick="showTab(0)">Match</button>
  <button class="segment-btn" onclick="showTab(1)">Manuel</button>
  <button class="segment-btn" onclick="showTab(2)">Réglages</button>
</div>

<div class="container">

  <!-- TAB 0: LIVE GAME -->
  <div id="tab0" class="tab-content active">
    <div class="card">
      <div class="card-header">
        <span class="card-title">Configuration Match</span>
        <span class="card-badge" id="badgeState">MENU</span>
      </div>
      <div class="select-wrap">
        <select id="selMode" onchange="sendGameMode(this.value)">
          <option value="0">Mode 0 — Vitesse Solo (1 Couleur, sans délai)</option>
          <option value="1">Mode 1 — Duel Bicolore (2 Couleurs, sans délai)</option>
          <option value="2">Mode 2 — Réflexe Aléatoire (1 Couleur, avec délai)</option>
          <option value="3">Mode 3 — Agilité Cognitive (2 Couleurs, avec délai)</option>
          <option value="4">Mode 4 — Jeu Simon (Mémoire séquentielle & accélération)</option>
        </select>
      </div>
    </div>

    <div class="card">
      <div class="card-header">
        <span class="card-title">Chronomètre de Séance</span>
      </div>
      <div class="timer-hero">
        <div class="timer-digits" id="txtTimer">00:00.00</div>
        <div class="timer-sub" id="txtModeDesc">Prêt à démarrer</div>
      </div>
      <div class="button-stack">
        <button class="btn btn-primary" id="btnMainStart" onclick="apiGame('START')">Démarrer</button>
        <button class="btn btn-pause" id="btnMainPause" onclick="togglePauseState()">Pause</button>
        <button class="btn btn-stop" onclick="apiGame('STOP')">Arrêter</button>
      </div>
    </div>

    <!-- SYNTHESE ENTRAINEMENT (ADAPTATIVE SELON LE MODE) -->
    <div class="card">
      <div class="card-header">
        <span class="card-title">Synthèse des Performances</span>
        <span class="card-badge" id="badgeLeader" style="display:none;">ÉGALITÉ</span>
      </div>

      <!-- VUE 1 : DUEL BICOLORE (MODES 1 ET 3) -->
      <div id="viewDuel" class="duel-card" style="display:none;">
        <div class="duel-cols">
          <div class="duel-col" id="colDuel1" style="border-left-color: #ff453a;">
            <div class="duel-name"><div class="duel-dot" id="dotC1" style="background:#ff453a"></div> <span id="lblC1">Couleur 1</span></div>
            <div class="duel-hits" id="valC1Hits">0</div>
            <div class="duel-meta">Moy: <span id="valC1Avg">--</span> ms | Min: <span id="valC1Best">--</span> ms</div>
          </div>
          <div class="duel-col" id="colDuel2" style="border-left-color: #30d158;">
            <div class="duel-name"><div class="duel-dot" id="dotC2" style="background:#30d158"></div> <span id="lblC2">Couleur 2</span></div>
            <div class="duel-hits" id="valC2Hits">0</div>
            <div class="duel-meta">Moy: <span id="valC2Avg">--</span> ms | Min: <span id="valC2Best">--</span> ms</div>
          </div>
        </div>
        <div class="duel-bar-wrap">
          <div class="duel-bar-c1" id="barC1" style="width:50%;"></div>
          <div class="duel-bar-c2" id="barC2" style="width:50%;"></div>
        </div>
      </div>

      <!-- VUE 2 : JEU SIMON (MODE 4) -->
      <div id="viewSimon" class="duel-card" style="display:none; background:rgba(0,0,0,0.25); border:1px solid var(--border-light); border-radius:12px; padding:12px; margin-bottom:10px;">
        <div style="display:flex; justify-content:space-between; align-items:center; margin-bottom:10px;">
          <span style="font-size:0.75rem; font-weight:600; text-transform:uppercase; color:var(--text-3); letter-spacing:0.03em;">Séquence Simon</span>
          <span class="card-badge" id="badgeSimonState" style="color:var(--accent-purple); background:rgba(191,90,242,0.15);">ATTENTE</span>
        </div>
        <div class="metrics-grid" style="margin-top:0; margin-bottom:10px;">
          <div class="metric-cell" style="background:rgba(255,255,255,0.04);">
            <div class="metric-label">Tour / Longueur</div>
            <div class="metric-value" style="color:var(--accent-purple);"><span id="valSimonRound">1</span><span class="metric-unit" id="valSimonSteps">/ 1</span></div>
          </div>
          <div class="metric-cell" style="background:rgba(255,255,255,0.04);">
            <div class="metric-label">Record Session</div>
            <div class="metric-value" style="color:var(--accent-green);"><span id="valSimonBest">0</span><span class="metric-unit">tours</span></div>
          </div>
          <div class="metric-cell" style="background:rgba(255,255,255,0.04);">
            <div class="metric-label">Étape Joueur</div>
            <div class="metric-value" style="color:var(--accent-blue);"><span id="valSimonCurStep">0</span><span class="metric-unit" id="valSimonMaxStep">/ 1</span></div>
          </div>
          <div class="metric-cell" style="background:rgba(255,255,255,0.04);">
            <div class="metric-label">Vitesse Démo</div>
            <div class="metric-value" style="color:var(--accent-orange);"><span id="valSimonTempo">800</span><span class="metric-unit">ms</span></div>
          </div>
        </div>
        <div class="duel-bar-wrap" style="height:6px; background:rgba(255,255,255,0.1); border-radius:3px; overflow:hidden;">
          <div id="barSimonProg" style="height:100%; width:0%; background:var(--accent-purple); transition:width 0.2s ease-out;"></div>
        </div>
      </div>

      <!-- VUE 2 : SYNTHESE GLOBALE ATHLETE (MODES 0, 2 OU SYNTHESE COMMUNE) -->
      <div class="metrics-grid">
        <div class="metric-cell">
          <div class="metric-label">Total Touches</div>
          <div class="metric-value" id="valHits">0</div>
        </div>
        <div class="metric-cell">
          <div class="metric-label">Cadence</div>
          <div class="metric-value"><span id="valCadence">0.0</span><span class="metric-unit">bpm</span></div>
        </div>
        <div class="metric-cell">
          <div class="metric-label">Temps Moyen</div>
          <div class="metric-value"><span id="valAvgRt">--</span><span class="metric-unit">ms</span></div>
        </div>
        <div class="metric-cell">
          <div class="metric-label">Meilleur Record</div>
          <div class="metric-value" style="color:var(--accent-green)"><span id="valBestRt">--</span><span class="metric-unit">ms</span></div>
        </div>
      </div>

      <!-- GRAPHIQUE HAUTE PRECISION AUTO-ADAPTATIF (ECHELLE DYNAMIQUE ms) -->
      <div class="chart-wrapper">
        <div class="chart-y-axis" id="chartYAxis">
          <span>500ms</span>
          <span>375ms</span>
          <span>250ms</span>
          <span>125ms</span>
          <span>0ms</span>
        </div>
        <div class="chart-canvas-box">
          <canvas id="liveChart"></canvas>
        </div>
      </div>
      <div class="chart-legend">
        <div class="legend-item"><div class="duel-dot" style="background:var(--accent-teal)"></div> Réactivité (ms)</div>
        <div class="legend-item"><div class="duel-dot" style="background:var(--accent-yellow)"></div> Ligne Moyenne</div>
      </div>
    </div>
  </div>

  <!-- TAB 1: MANUEL COACH -->
  <div id="tab1" class="tab-content">
    <div class="card">
      <div class="card-header">
        <span class="card-title">Palette de Couleurs</span>
      </div>
      <div class="palette">
        <div class="color-chip selected" style="background:#30d158" onclick="setManualColor('00FF00', this)"></div>
        <div class="color-chip" style="background:#ff453a" onclick="setManualColor('FF0000', this)"></div>
        <div class="color-chip" style="background:#0a84ff" onclick="setManualColor('0000FF', this)"></div>
        <div class="color-chip" style="background:#ffd60a" onclick="setManualColor('FFFF00', this)"></div>
        <div class="color-chip" style="background:#bf5af2" onclick="setManualColor('FF00FF', this)"></div>
        <div class="color-chip" style="background:#ffffff" onclick="setManualColor('FFFFFF', this)"></div>
      </div>
    </div>

    <div class="card">
      <div class="card-header">
        <span class="card-title">Flotte de Pods</span>
        <button class="btn-pill" onclick="apiManual({action:'ALL_OFF'})">Tout Éteindre</button>
      </div>
      <div class="pods-fleet-grid" id="gridPodsFleet">
        <!-- Rempli dynamiquement -->
      </div>
    </div>
  </div>

  <!-- TAB 2: REGLAGES HIERARCHIQUES STYLE APPLE IOS -->
  <div id="tab2" class="tab-content">

    <!-- 1. VUE RACINE DES REGLAGES (SECTIONS IOS) -->
    <div id="settingsRootView">
      <div class="ios-section-header">Modes d'entraînement</div>
      <div class="ios-group">
        <div class="ios-row" onclick="openModeDetail(0)">
          <div class="ios-row-left">
            <div class="ios-icon-box" style="background:#30d158;">
              <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="#ffffff" stroke-width="2.3" stroke-linecap="round" stroke-linejoin="round"><polygon points="13 2 3 14 12 14 11 22 21 10 12 10 13 2"/></svg>
            </div>
            <div class="ios-row-text">
              <div class="ios-row-title">Mode 0 — Vitesse Solo</div>
              <div class="ios-row-subtitle" id="subMode0">1 Couleur • Sans délai</div>
            </div>
          </div>
          <div class="ios-row-right">
            <div class="duel-dot" id="dotCfg0_1" style="background:#30d158;"></div>
            <span class="ios-chevron">›</span>
          </div>
        </div>

        <div class="ios-row" onclick="openModeDetail(1)">
          <div class="ios-row-left">
            <div class="ios-icon-box" style="background:#ff9f0a;">
              <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="#ffffff" stroke-width="2.3" stroke-linecap="round" stroke-linejoin="round"><circle cx="7" cy="12" r="4"/><circle cx="17" cy="12" r="4"/></svg>
            </div>
            <div class="ios-row-text">
              <div class="ios-row-title">Mode 1 — Duel Bicolore</div>
              <div class="ios-row-subtitle" id="subMode1">2 Couleurs • Sans délai</div>
            </div>
          </div>
          <div class="ios-row-right">
            <div class="duel-dot" id="dotCfg1_1" style="background:#ff453a;"></div>
            <div class="duel-dot" id="dotCfg1_2" style="background:#30d158;"></div>
            <span class="ios-chevron">›</span>
          </div>
        </div>

        <div class="ios-row" onclick="openModeDetail(2)">
          <div class="ios-row-left">
            <div class="ios-icon-box" style="background:#0a84ff;">
              <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="#ffffff" stroke-width="2.3" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="13" r="8"/><polyline points="12 9 12 13 15 15"/><line x1="12" y1="2" x2="12" y2="5"/><line x1="10" y1="2" x2="14" y2="2"/></svg>
            </div>
            <div class="ios-row-text">
              <div class="ios-row-title">Mode 2 — Réflexe Aléatoire</div>
              <div class="ios-row-subtitle" id="subMode2">1 Couleur • Délai variable</div>
            </div>
          </div>
          <div class="ios-row-right">
            <div class="duel-dot" id="dotCfg2_1" style="background:#0a84ff;"></div>
            <span class="ios-chevron">›</span>
          </div>
        </div>

        <div class="ios-row" onclick="openModeDetail(3)">
          <div class="ios-row-left">
            <div class="ios-icon-box" style="background:#bf5af2;">
              <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="#ffffff" stroke-width="2.3" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="3" width="7" height="7" rx="1.5"/><rect x="14" y="3" width="7" height="7" rx="1.5"/><rect x="14" y="14" width="7" height="7" rx="1.5"/><rect x="3" y="14" width="7" height="7" rx="1.5"/></svg>
            </div>
            <div class="ios-row-text">
              <div class="ios-row-title">Mode 3 — Agilité Cognitive</div>
              <div class="ios-row-subtitle" id="subMode3">2 Couleurs • Délai variable</div>
            </div>
          </div>
          <div class="ios-row-right">
            <div class="duel-dot" id="dotCfg3_1" style="background:#ffd60a;"></div>
            <div class="duel-dot" id="dotCfg3_2" style="background:#bf5af2;"></div>
            <span class="ios-chevron">›</span>
          </div>
        </div>

        <div class="ios-row" onclick="openModeDetail(4)">
          <div class="ios-row-left">
            <div class="ios-icon-box" style="background:#ff2d55;">
              <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="#ffffff" stroke-width="2.3" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"/><path d="M12 2a10 10 0 0 1 10 10"/><path d="M12 12 2 12"/><path d="M12 12v10"/></svg>
            </div>
            <div class="ios-row-text">
              <div class="ios-row-title">Mode 4 — Jeu Simon</div>
              <div class="ios-row-subtitle" id="subMode4">Mémoire • 800ms • +6%/tour</div>
            </div>
          </div>
          <div class="ios-row-right">
            <div class="duel-dot" id="dotCfg4_1" style="background:#ff2d55;"></div>
            <span class="ios-chevron">›</span>
          </div>
        </div>
      </div>

      <div class="ios-section-header">Énergie & Santé</div>
      <div class="ios-group">
        <div class="ios-row" onclick="openSubpage('subpageBatteryDetail', 'Batterie & Consommation')">
          <div class="ios-row-left">
            <div class="ios-icon-box" style="background:#30d158;">
              <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="#ffffff" stroke-width="2.3" stroke-linecap="round" stroke-linejoin="round"><rect x="2" y="7" width="16" height="10" rx="2.5"/><line x1="22" y1="11" x2="22" y2="13"/></svg>
            </div>
            <div class="ios-row-text">
              <div class="ios-row-title">Batterie & Consommation</div>
              <div class="ios-row-subtitle" id="subBatRow">--% • Sur Batterie</div>
            </div>
          </div>
          <div class="ios-row-right">
            <span id="txtBatRightPct">--%</span>
            <span class="ios-chevron">›</span>
          </div>
        </div>
      </div>

      <div class="ios-section-header">Matériel & Tests</div>
      <div class="ios-group">
        <div class="ios-row" onclick="openSubpage('subpageDiagDetail', 'Diagnostics Matériels')">
          <div class="ios-row-left">
            <div class="ios-icon-box" style="background:#30b0c7;">
              <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="#ffffff" stroke-width="2.3" stroke-linecap="round" stroke-linejoin="round"><path d="M14.7 6.3a1 1 0 0 0 0 1.4l1.6 1.6a1 1 0 0 0 1.4 0l3.77-3.77a6 6 0 0 1-7.94 7.94l-6.91 6.91a2.12 2.12 0 0 1-3-3l6.91-6.91a6 6 0 0 1 7.94-7.94l-3.76 3.76z"/></svg>
            </div>
            <div class="ios-row-text">
              <div class="ios-row-title">Diagnostics Matériels</div>
              <div class="ios-row-subtitle">ToF Laser, ESP-NOW Mesh, LEDs</div>
            </div>
          </div>
          <div class="ios-row-right">
            <span class="ios-chevron">›</span>
          </div>
        </div>
      </div>

      <div class="ios-section-header">Système & Console</div>
      <div class="ios-group">
        <div class="ios-row" style="cursor:default;">
          <div class="ios-row-left">
            <div class="ios-icon-box" style="background:#ffd60a;">
              <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="#000000" stroke-width="2.3" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="4"/><line x1="12" y1="2" x2="12" y2="4"/><line x1="12" y1="20" x2="12" y2="22"/><line x1="4.93" y1="4.93" x2="6.34" y2="6.34"/><line x1="17.66" y1="17.66" x2="19.07" y2="19.07"/><line x1="2" y1="12" x2="4" y2="12"/><line x1="20" y1="12" x2="22" y2="12"/><line x1="6.34" y1="17.66" x2="4.93" y2="19.07"/><line x1="19.07" y1="4.93" x2="17.66" y2="6.34"/></svg>
            </div>
            <div class="ios-row-text">
              <div class="ios-row-title">Luminosité des LEDs</div>
            </div>
          </div>
        </div>
        <div style="padding: 0 16px 14px;">
          <input type="range" min="25" max="255" value="80" id="sliderBright" onchange="apiSettings({brightness:parseInt(this.value)})">
        </div>

        <div class="ios-row" onclick="openSubpage('subpageLogsDetail', 'Journal Console')">
          <div class="ios-row-left">
            <div class="ios-icon-box" style="background:#636366;">
              <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="#ffffff" stroke-width="2.3" stroke-linecap="round" stroke-linejoin="round"><polyline points="4 17 10 11 4 5"/><line x1="12" y1="19" x2="20" y2="19"/></svg>
            </div>
            <div class="ios-row-text">
              <div class="ios-row-title">Journal Console Système</div>
              <div class="ios-row-subtitle">Événements radio et frappes</div>
            </div>
          </div>
          <div class="ios-row-right">
            <span class="ios-chevron">›</span>
          </div>
        </div>

        <a href="/api/export" class="ios-row">
          <div class="ios-row-left">
            <div class="ios-icon-box" style="background:#0a84ff;">
              <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="#ffffff" stroke-width="2.3" stroke-linecap="round" stroke-linejoin="round"><path d="M4 12v8a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2v-8"/><polyline points="16 6 12 2 8 6"/><line x1="12" y1="2" x2="12" y2="15"/></svg>
            </div>
            <div class="ios-row-text">
              <div class="ios-row-title">Exporter la Séance (CSV)</div>
              <div class="ios-row-subtitle">Tableau des temps de réaction</div>
            </div>
          </div>
          <div class="ios-row-right">
            <span class="ios-chevron">›</span>
          </div>
        </a>

        <div class="ios-row" onclick="restartPod()">
          <div class="ios-row-left">
            <div class="ios-icon-box" style="background:#ff453a;">
              <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="#ffffff" stroke-width="2.3" stroke-linecap="round" stroke-linejoin="round"><path d="M21.5 2v6h-6"/><path d="M21.34 15.57a10 10 0 1 1-.57-8.38l5.67-5.67"/></svg>
            </div>
            <div class="ios-row-text">
              <div class="ios-row-title" style="color:var(--accent-red);">Redémarrer le Pod Master</div>
            </div>
          </div>
        </div>
      </div>
    </div>

    <!-- 2. VUE SOUS-PAGES DETAILLEES (STYLE IOS AVEC RETOUR) -->
    <div id="settingsSubpageView" style="display:none;">
      <button class="ios-back-btn" onclick="backToSettingsRoot()">
        <span style="font-size:1.1rem; line-height:1; margin-right:2px;">‹</span> Réglages
      </button>

      <!-- SOUS-PAGE A : PERSONNALISATION D'UN MODE -->
      <div id="subpageModeDetail" class="subpage-pane" style="display:none;">
        <div class="card">
          <div class="card-header">
            <span class="card-title" id="txtCfgModeTitle">Configuration Mode</span>
          </div>
          <p id="txtCfgModeDesc" style="font-size:0.82rem; color:var(--text-2); margin-bottom:16px;">Description du mode</p>

          <div style="margin-bottom:16px;">
            <label style="font-size:0.75rem; font-weight:700; color:var(--text-3); text-transform:uppercase; display:block; margin-bottom:8px;">Couleur Principale (Cible 1)</label>
            <div class="palette" id="palCfgC1">
              <div class="color-chip" style="background:#30d158" onclick="selectCfgColor('C1', '00FF00', this)"></div>
              <div class="color-chip" style="background:#ff453a" onclick="selectCfgColor('C1', 'FF0000', this)"></div>
              <div class="color-chip" style="background:#0a84ff" onclick="selectCfgColor('C1', '0000FF', this)"></div>
              <div class="color-chip" style="background:#ffd60a" onclick="selectCfgColor('C1', 'FFFF00', this)"></div>
              <div class="color-chip" style="background:#bf5af2" onclick="selectCfgColor('C1', 'FF00FF', this)"></div>
              <div class="color-chip" style="background:#30b0c7" onclick="selectCfgColor('C1', '00FFFF', this)"></div>
              <div class="color-chip" style="background:#ffffff" onclick="selectCfgColor('C1', 'FFFFFF', this)"></div>
            </div>
          </div>

          <div id="grpCfgC2" style="margin-bottom:16px; display:none;">
            <label style="font-size:0.75rem; font-weight:700; color:var(--text-3); text-transform:uppercase; display:block; margin-bottom:8px;">Deuxième Couleur (Cible 2 / Joueur 2)</label>
            <div class="palette" id="palCfgC2">
              <div class="color-chip" style="background:#30d158" onclick="selectCfgColor('C2', '00FF00', this)"></div>
              <div class="color-chip" style="background:#ff453a" onclick="selectCfgColor('C2', 'FF0000', this)"></div>
              <div class="color-chip" style="background:#0a84ff" onclick="selectCfgColor('C2', '0000FF', this)"></div>
              <div class="color-chip" style="background:#ffd60a" onclick="selectCfgColor('C2', 'FFFF00', this)"></div>
              <div class="color-chip" style="background:#bf5af2" onclick="selectCfgColor('C2', 'FF00FF', this)"></div>
              <div class="color-chip" style="background:#30b0c7" onclick="selectCfgColor('C2', '00FFFF', this)"></div>
              <div class="color-chip" style="background:#ffffff" onclick="selectCfgColor('C2', 'FFFFFF', this)"></div>
            </div>
          </div>

          <div id="grpCfgDelays" style="margin-bottom:16px; display:none;">
            <div style="margin-bottom:12px;">
              <div style="display:flex; justify-content:space-between; font-size:0.8rem; font-weight:600;">
                <span>Délai Minimum</span>
                <span id="lblCfgDmin" style="color:var(--accent-cyan);">1 s</span>
              </div>
              <input type="range" min="0" max="10" value="1" id="sliderCfgDmin" oninput="document.getElementById('lblCfgDmin').innerText = this.value + ' s'">
            </div>
            <div>
              <div style="display:flex; justify-content:space-between; font-size:0.8rem; font-weight:600;">
                <span>Délai Maximum</span>
                <span id="lblCfgDmax" style="color:var(--accent-cyan);">5 s</span>
              </div>
              <input type="range" min="1" max="30" value="5" id="sliderCfgDmax" oninput="document.getElementById('lblCfgDmax').innerText = this.value + ' s'">
            </div>
          </div>

          <div id="grpCfgSimon" style="margin-bottom:16px; display:none;">
            <div style="margin-bottom:12px;">
              <div style="display:flex; justify-content:space-between; font-size:0.8rem; font-weight:600;">
                <span>Tempo Initial de Démonstration</span>
                <span id="lblCfgSimonTempo" style="color:var(--accent-blue);">800 ms</span>
              </div>
              <input type="range" min="200" max="2000" step="50" value="800" id="sliderCfgSimonTempo" oninput="document.getElementById('lblCfgSimonTempo').innerText = this.value + ' ms'">
            </div>
            <div>
              <div style="display:flex; justify-content:space-between; font-size:0.8rem; font-weight:600;">
                <span>Accélération par Tour</span>
                <span id="lblCfgSimonAccel" style="color:var(--accent-orange);">6 %</span>
              </div>
              <input type="range" min="1" max="25" step="1" value="6" id="sliderCfgSimonAccel" oninput="document.getElementById('lblCfgSimonAccel').innerText = this.value + ' %'">
            </div>
            <p style="font-size:0.75rem; color:var(--text-3); margin-top:8px;">Chaque pod possède sa couleur signature. À chaque tour réussi, la séquence s'allonge et la vitesse de démonstration s'accélère.</p>
          </div>

          <button class="btn btn-primary" onclick="saveCurrentModeConfig()" style="margin-top:10px;">Enregistrer la Configuration</button>
        </div>
      </div>

      <!-- SOUS-PAGE B : BATTERIE & GRAPHIQUE OFFICIEL APPLE IOS -->
      <div id="subpageBatteryDetail" class="subpage-pane" style="display:none;">
        <div class="ios-battery-card">
          <div class="ios-battery-header">BATTERY LEVEL</div>
          <div class="ios-bat-canvas-container">
            <canvas id="batteryChart"></canvas>
            <div class="ios-bat-y-labels">
              <span>100%</span>
              <span>50%</span>
              <span>0%</span>
            </div>
          </div>
        </div>

        <div class="ios-section-header">INFORMATIONS POD MASTER</div>
        <div class="ios-group">
          <div class="ios-row" style="cursor:default;">
            <div class="ios-row-left"><div class="ios-row-title">Niveau Actuel</div></div>
            <div class="ios-row-right" id="txtBatSubPct" style="font-weight:700; color:var(--text-1);">--%</div>
          </div>
          <div class="ios-row" style="cursor:default;">
            <div class="ios-row-left"><div class="ios-row-title">Tension Batterie ADC</div></div>
            <div class="ios-row-right" id="txtBatSubVolt">-- V</div>
          </div>
          <div class="ios-row" style="cursor:default;">
            <div class="ios-row-left"><div class="ios-row-title">Source d'Alimentation</div></div>
            <div class="ios-row-right" id="txtBatSubSource">--</div>
          </div>
          <div class="ios-row" style="cursor:default;">
            <div class="ios-row-left"><div class="ios-row-title">État de Santé</div></div>
            <div class="ios-row-right" style="color:var(--accent-green);">Normal (3.0V - 4.2V)</div>
          </div>
        </div>

        <div class="ios-section-header" style="margin-top:20px;">BATTERIE DE TOUTE LA FLOTTE</div>
        <div class="ios-group" id="fleetBatteryList">
          <div style="padding:16px; text-align:center; color:var(--text-3); font-size:0.8rem;">Recherche des pods...</div>
        </div>
      </div>

      <!-- SOUS-PAGE C : DIAGNOSTICS MATERIELS -->
      <div id="subpageDiagDetail" class="subpage-pane" style="display:none;">
        <div class="ios-section-header">TESTS DES CAPTEURS & ACTIONNEURS</div>
        <div class="ios-group">
          <div class="ios-row" style="cursor:default;">
            <div class="ios-row-left">
              <div class="ios-row-text">
                <div class="ios-row-title">Capteur ToF Laser (VL53L0X)</div>
                <div class="ios-row-subtitle">Distance temps réel : <span id="diagSubTof" style="color:var(--accent-cyan); font-weight:700;">-- mm</span></div>
              </div>
            </div>
          </div>

          <div class="ios-row">
            <div class="ios-row-left">
              <div class="ios-row-text">
                <div class="ios-row-title">Liaison Radio ESP-NOW / Mesh</div>
                <div class="ios-row-subtitle" id="diagPingStatus">Ping de découverte et latence</div>
              </div>
            </div>
            <button class="btn-pill" id="btnPingFleet" onclick="runPingTest()">Ping Flotte</button>
          </div>
          <div id="diagPingBox" style="display:none; padding:10px 16px; background:var(--surface-2); border-bottom:0.5px solid var(--separator);">
            <div id="diagPingSummary" style="font-weight:600; font-size:0.8rem; margin-bottom:6px; color:var(--text-1);"></div>
            <div id="diagPingList" style="font-size:0.75rem; color:var(--text-2); display:flex; flex-direction:column; gap:6px;"></div>
          </div>

          <div class="ios-row">
            <div class="ios-row-left">
              <div class="ios-row-text">
                <div class="ios-row-title">Anneau 7 LEDs RVB WS2812B</div>
                <div class="ios-row-subtitle">Animation test des couleurs</div>
              </div>
            </div>
            <button class="btn-pill" onclick="apiTest({type:'LED'})">Flasher</button>
          </div>
        </div>
      </div>

      <!-- SOUS-PAGE D : JOURNAL CONSOLE -->
      <div id="subpageLogsDetail" class="subpage-pane" style="display:none;">
        <div class="card">
          <div class="card-header">
            <span class="card-title">Console Système en Direct</span>
            <button class="btn-pill" onclick="apiTest({type:'CLEAR_LOGS'})">Effacer</button>
          </div>
          <div class="console-term" id="consoleLogs"></div>
        </div>
      </div>

    </div>

  </div>

</div>

<script>
  let currentColor = '00FF00';
  let isPausedState = false;
  let historyRaw = [];
  let batHistoryRaw = [];
  let modesData = [
    { id:0, name:"Vitesse Solo", c1:"#00FF00", c2:"#000000", dmin:0, dmax:0 },
    { id:1, name:"Duel Bicolore", c1:"#FF0000", c2:"#00FF00", dmin:0, dmax:0 },
    { id:2, name:"Réflexe Aléatoire", c1:"#0000FF", c2:"#000000", dmin:1, dmax:5 },
    { id:3, name:"Agilité Cognitive", c1:"#FFFF00", c2:"#FF00FF", dmin:1, dmax:5 },
    { id:4, name:"Jeu Simon", c1:"#FF2D55", c2:"#000000", dmin:800, dmax:6 }
  ];
  let editingModeId = 0;
  let selectedC1 = '00FF00';
  let selectedC2 = '0000FF';

  function showTab(idx) {
    document.querySelectorAll('.segment-btn').forEach((b, i) => b.classList.toggle('active', i === idx));
    document.querySelectorAll('.tab-content').forEach((c, i) => c.classList.toggle('active', i === idx));
    if (idx === 0) renderHighPrecisionChart();
    if (idx === 2) {
      const batPane = document.getElementById('subpageBatteryDetail');
      if (batPane && batPane.style.display !== 'none') renderBatteryChart();
    }
  }

  function setManualColor(hex, el) {
    currentColor = hex;
    document.querySelectorAll('#tab1 .color-chip').forEach(c => c.classList.remove('selected'));
    el.classList.add('selected');
  }

  function apiGame(action) {
    fetch('/api/game', { method: 'POST', body: JSON.stringify({ action: action }) });
  }

  function togglePauseState() {
    isPausedState = !isPausedState;
    apiGame(isPausedState ? 'PAUSE' : 'RESUME');
  }

  function sendGameMode(m) {
    fetch('/api/game', { method: 'POST', body: JSON.stringify({ action: 'SET_MODE', mode: parseInt(m) }) });
  }

  function apiManual(obj) {
    fetch('/api/manual', { method: 'POST', body: JSON.stringify(obj) });
  }

  function apiTest(obj) {
    fetch('/api/test', { method: 'POST', body: JSON.stringify(obj) });
  }

  async function runPingTest() {
    const btn = document.getElementById('btnPingFleet');
    const box = document.getElementById('diagPingBox');
    const summary = document.getElementById('diagPingSummary');
    const list = document.getElementById('diagPingList');
    const statusText = document.getElementById('diagPingStatus');

    if (btn) { btn.innerText = "Ping..."; btn.disabled = true; }
    if (statusText) statusText.innerText = "Envoi du ping à la flotte...";

    try {
      const res = await fetch('/api/test', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ type: 'PING' })
      });
      const data = await res.json();

      if (btn) { btn.innerText = "Ping Flotte"; btn.disabled = false; }

      if (box && summary && list) {
        box.style.display = 'block';
        list.innerHTML = '';

        if (!data.pods || data.pods.length === 0) {
          summary.innerHTML = "<span style='color:var(--accent-red);'>⚠️ Aucun pod distant n'a répondu.</span>";
          if (statusText) statusText.innerText = "0 réponse (aucun pod distant en ligne)";
        } else {
          summary.innerHTML = `<span style='color:var(--accent-green);'>✅ ${data.count} pod(s) ont répondu :</span>`;
          if (statusText) statusText.innerText = `${data.count} pod(s) connecté(s)`;

          data.pods.forEach(p => {
            const row = document.createElement('div');
            row.style.display = 'flex';
            row.style.justifyContent = 'space-between';
            row.style.alignItems = 'center';
            row.style.padding = '4px 0';
            row.style.borderBottom = '1px solid rgba(255,255,255,0.05)';
            row.innerHTML = `
              <span><strong>${p.name || 'Pod'}</strong> <span style="opacity:0.7;">(${p.mac})</span></span>
              <span>
                <span style="color:var(--accent-cyan); font-weight:600;">${p.rtt} ms</span> • 
                <span style="color:var(--text-3);">${p.rssi} dBm</span> • 
                <span style="color:var(--accent-yellow); font-weight:600;">${p.batPct}% 🔋</span>
              </span>
            `;
            list.appendChild(row);
          });
        }
      }
    } catch (err) {
      if (btn) { btn.innerText = "Ping Flotte"; btn.disabled = false; }
      if (statusText) statusText.innerText = "Erreur de communication";
      console.error(err);
    }
  }

  function apiSettings(obj) {
    fetch('/api/settings', { method: 'POST', body: JSON.stringify(obj) });
  }

  function restartPod() {
    if (confirm("Voulez-vous vraiment redémarrer le pod ?")) {
      apiSettings({ action: 'RESTART' });
      alert("Redémarrage en cours... Reconnexion dans 5 secondes.");
    }
  }

  function openSubpage(subId, title) {
    document.getElementById('settingsRootView').style.display = 'none';
    document.getElementById('settingsSubpageView').style.display = 'block';
    document.querySelectorAll('.subpage-pane').forEach(p => p.style.display = 'none');
    const target = document.getElementById(subId);
    if (target) target.style.display = 'block';
    if (subId === 'subpageBatteryDetail') {
      setTimeout(renderBatteryChart, 50);
      pollPods();
    }
  }

  function backToSettingsRoot() {
    document.getElementById('settingsSubpageView').style.display = 'none';
    document.getElementById('settingsRootView').style.display = 'block';
  }

  function openModeDetail(modeId) {
    editingModeId = modeId;
    openSubpage('subpageModeDetail', 'Mode ' + modeId);

    const m = modesData[modeId] || modesData[0];
    document.getElementById('txtCfgModeTitle').innerText = 'Mode ' + modeId + ' — ' + (m.name || 'Jeu Simon');

    let desc = "1 Pod actif sans aucun délai entre les touches.";
    if (modeId === 1) desc = "Duel 1 contre 1 : 2 couleurs distinctes actives sans délai.";
    else if (modeId === 2) desc = "1 Couleur avec tempo et délais d'apparition aléatoires.";
    else if (modeId === 3) desc = "Agilité cognitive : 2 couleurs avec apparition aléatoire.";
    else if (modeId === 4) desc = "Mémorisez la séquence lumineuse et reproduisez-la sans faute. La vitesse accélère à chaque tour !";
    document.getElementById('txtCfgModeDesc').innerText = desc;

    selectedC1 = (m.c1 || '#00FF00').replace('#', '');
    selectedC2 = (m.c2 || '#FF0000').replace('#', '');

    // Palettes
    highlightChip('palCfgC1', selectedC1);
    highlightChip('palCfgC2', selectedC2);

    const isSimon = (modeId === 4);
    const hasC1 = !isSimon;
    const hasC2 = (modeId === 1 || modeId === 3);
    const hasDelays = (modeId === 2 || modeId === 3);

    document.getElementById('palCfgC1').parentElement.style.display = hasC1 ? 'block' : 'none';
    document.getElementById('grpCfgC2').style.display = hasC2 ? 'block' : 'none';
    document.getElementById('grpCfgDelays').style.display = hasDelays ? 'block' : 'none';
    document.getElementById('grpCfgSimon').style.display = isSimon ? 'block' : 'none';

    if (hasDelays) {
      document.getElementById('sliderCfgDmin').value = m.dmin || 1;
      document.getElementById('lblCfgDmin').innerText = (m.dmin || 1) + ' s';
      document.getElementById('sliderCfgDmax').value = m.dmax || 5;
      document.getElementById('lblCfgDmax').innerText = (m.dmax || 5) + ' s';
    }

    if (isSimon) {
      const tempo = m.dmin || 800;
      const accel = m.dmax || 6;
      document.getElementById('sliderCfgSimonTempo').value = tempo;
      document.getElementById('lblCfgSimonTempo').innerText = tempo + ' ms';
      document.getElementById('sliderCfgSimonAccel').value = accel;
      document.getElementById('lblCfgSimonAccel').innerText = accel + ' %';
    }
  }

  function selectCfgColor(target, hex, el) {
    if (target === 'C1') selectedC1 = hex;
    else selectedC2 = hex;
    el.parentElement.querySelectorAll('.color-chip').forEach(c => c.classList.remove('selected'));
    el.classList.add('selected');
  }

  function highlightChip(containerId, hex) {
    const parent = document.getElementById(containerId);
    if (!parent) return;
    hex = hex.toUpperCase();
    parent.querySelectorAll('.color-chip').forEach(chip => {
      chip.classList.remove('selected');
      const onclickStr = chip.getAttribute('onclick') || '';
      if (onclickStr.toUpperCase().includes(hex)) {
        chip.classList.add('selected');
      }
    });
  }

  function saveCurrentModeConfig() {
    let dmin = 0;
    let dmax = 0;

    if (editingModeId === 4) {
      dmin = parseInt(document.getElementById('sliderCfgSimonTempo').value) || 800;
      dmax = parseInt(document.getElementById('sliderCfgSimonAccel').value) || 6;
    } else {
      dmin = parseInt(document.getElementById('sliderCfgDmin').value) || 0;
      dmax = parseInt(document.getElementById('sliderCfgDmax').value) || 0;
    }

    apiSettings({
      mode: editingModeId,
      c1: selectedC1,
      c2: selectedC2,
      dmin: dmin,
      dmax: dmax
    });

    modesData[editingModeId].c1 = '#' + selectedC1;
    modesData[editingModeId].c2 = '#' + selectedC2;
    modesData[editingModeId].dmin = dmin;
    modesData[editingModeId].dmax = dmax;

    updateModeSubtitles();
    alert("Configuration du Mode " + editingModeId + " enregistrée avec succès !");
    backToSettingsRoot();
  }

  function updateModeSubtitles() {
    modesData.forEach((m, idx) => {
      const dot1 = document.getElementById('dotCfg' + idx + '_1');
      const dot2 = document.getElementById('dotCfg' + idx + '_2');
      if (dot1) dot1.style.background = m.c1;
      if (dot2) dot2.style.background = m.c2;

      const sub = document.getElementById('subMode' + idx);
      if (sub) {
        if (idx === 0) sub.innerText = `1 Couleur • Sans délai`;
        else if (idx === 1) sub.innerText = `2 Couleurs • Sans délai`;
        else if (idx === 2) sub.innerText = `1 Couleur • Délai ${m.dmin}-${m.dmax}s`;
        else if (idx === 3) sub.innerText = `2 Couleurs • Délai ${m.dmin}-${m.dmax}s`;
        else if (idx === 4) sub.innerText = `Mémoire • ${m.dmin || 800}ms • +${m.dmax || 6}%/tour`;
      }
    });
  }

  function fmtMs(ms) {
    let s = Math.floor(ms / 1000);
    let m = Math.floor(s / 60);
    let sec = s % 60;
    let cs = Math.floor((ms % 1000) / 10);
    return `${m < 10 ? '0' : ''}${m}:${sec < 10 ? '0' : ''}${sec}.${cs < 10 ? '0' : ''}${cs}`;
  }

  function pollStatus() {
    fetch('/api/status')
      .then(r => r.json())
      .then(d => {
        document.getElementById('txtTimer').innerText = fmtMs(d.time);
        document.getElementById('badgeState').innerText = d.state;
        document.getElementById('txtModeDesc').innerText = d.modeDesc;
        document.getElementById('valHits').innerText = d.hits;
        document.getElementById('valCadence').innerText = d.cadence;
        document.getElementById('valAvgRt').innerText = d.avgRt > 0 ? d.avgRt : '--';
        document.getElementById('valBestRt').innerText = d.bestRt > 0 ? d.bestRt : '--';
        document.getElementById('headerBat').innerText = `${d.charging ? '⚡ ' : ''}${d.batPct}%`;

        // Mise à jour de la sous-page diagnostics
        const dToF = document.getElementById('diagSubTof');
        if (dToF) dToF.innerText = d.tofDist >= 8000 ? 'Hors portée' : `${d.tofDist} mm`;

        // Mise à jour de la ligne Batterie dans les réglages
        const subBatRow = document.getElementById('subBatRow');
        const txtBatRightPct = document.getElementById('txtBatRightPct');
        if (subBatRow) subBatRow.innerText = `${d.batPct}% • ${d.charging ? 'En charge (USB)' : 'Sur Batterie'}`;
        if (txtBatRightPct) txtBatRightPct.innerText = `${d.batPct}%`;

        // Mise à jour sous-page batterie
        const txtBatSubPct = document.getElementById('txtBatSubPct');
        const txtBatSubVolt = document.getElementById('txtBatSubVolt');
        const txtBatSubSource = document.getElementById('txtBatSubSource');
        if (txtBatSubPct) txtBatSubPct.innerText = `${d.batPct}%`;
        if (txtBatSubVolt) txtBatSubVolt.innerText = `${d.vBat} V`;
        if (txtBatSubSource) txtBatSubSource.innerText = d.charging ? 'USB-C 5V (En charge)' : 'LiPo 3.7V (Sur Batterie)';

        if (d.modes && d.modes.length === 5) {
          modesData = d.modes;
          updateModeSubtitles();
        }

        if (d.batHistory && d.batHistory.length > 0) {
          batHistoryRaw = d.batHistory;
          const batPane = document.getElementById('subpageBatteryDetail');
          if (batPane && batPane.style.display !== 'none') renderBatteryChart();
        }

        // Vue Duel Bicolore (Modes 1 & 3)
        const isDual = (d.mode === 1 || d.mode === 3);
        const duelView = document.getElementById('viewDuel');
        const badgeLeader = document.getElementById('badgeLeader');

        if (isDual) {
          duelView.style.display = 'block';
          badgeLeader.style.display = 'inline-block';

          document.getElementById('dotC1').style.background = d.c1Col;
          document.getElementById('colDuel1').style.borderLeftColor = d.c1Col;
          document.getElementById('barC1').style.background = d.c1Col;
          document.getElementById('valC1Hits').innerText = d.c1Hits;
          document.getElementById('valC1Avg').innerText = d.c1AvgRt > 0 ? d.c1AvgRt : '--';
          document.getElementById('valC1Best').innerText = d.c1BestRt > 0 ? d.c1BestRt : '--';

          document.getElementById('dotC2').style.background = d.c2Col;
          document.getElementById('colDuel2').style.borderLeftColor = d.c2Col;
          document.getElementById('barC2').style.background = d.c2Col;
          document.getElementById('valC2Hits').innerText = d.c2Hits;
          document.getElementById('valC2Avg').innerText = d.c2AvgRt > 0 ? d.c2AvgRt : '--';
          document.getElementById('valC2Best').innerText = d.c2BestRt > 0 ? d.c2BestRt : '--';

          const totalDuel = d.c1Hits + d.c2Hits;
          if (totalDuel > 0) {
            const p1 = (d.c1Hits / totalDuel) * 100;
            document.getElementById('barC1').style.width = p1 + '%';
            document.getElementById('barC2').style.width = (100 - p1) + '%';
          }

          if (d.c1Hits > d.c2Hits) {
            badgeLeader.innerText = 'JOUEUR 1 EN TÊTE';
            badgeLeader.style.color = d.c1Col;
          } else if (d.c2Hits > d.c1Hits) {
            badgeLeader.innerText = 'JOUEUR 2 EN TÊTE';
            badgeLeader.style.color = d.c2Col;
          } else {
            badgeLeader.innerText = 'ÉGALITÉ';
            badgeLeader.style.color = 'var(--accent-yellow)';
          }
        } else {
          duelView.style.display = 'none';
          badgeLeader.style.display = 'none';
        }

        // Vue Mode 4 : Jeu Simon
        const isSimon = (d.mode === 4);
        const simonView = document.getElementById('viewSimon');
        if (simonView) {
          if (isSimon) {
            simonView.style.display = 'block';
            const bSim = document.getElementById('badgeSimonState');
            if (bSim) {
              bSim.innerText = d.simonState || 'ATTENTE';
              if (d.simonState === 'DÉMONSTRATION') {
                bSim.style.color = 'var(--accent-orange)';
                bSim.style.background = 'rgba(255, 159, 10, 0.15)';
              } else if (d.simonState === 'À VOUS !') {
                bSim.style.color = 'var(--accent-green)';
                bSim.style.background = 'rgba(48, 209, 88, 0.15)';
              } else if (d.simonState === 'SUCCÈS !') {
                bSim.style.color = 'var(--accent-teal)';
                bSim.style.background = 'rgba(48, 176, 199, 0.15)';
              } else if (d.simonState === 'GAME OVER') {
                bSim.style.color = 'var(--accent-red)';
                bSim.style.background = 'rgba(255, 69, 58, 0.15)';
              } else {
                bSim.style.color = 'var(--accent-purple)';
                bSim.style.background = 'rgba(191, 90, 242, 0.15)';
              }
            }
            document.getElementById('valSimonRound').innerText = d.simonRound || 1;
            document.getElementById('valSimonSteps').innerText = `/ ${d.simonTotalSteps || 1}`;
            document.getElementById('valSimonBest').innerText = d.simonBestRound || 0;
            document.getElementById('valSimonCurStep').innerText = (d.simonState === 'DÉMONSTRATION' ? d.simonStep : (d.simonState === 'À VOUS !' ? (d.simonStep - 1) : d.simonStep));
            document.getElementById('valSimonMaxStep').innerText = `/ ${d.simonTotalSteps || 1}`;
            document.getElementById('valSimonTempo').innerText = d.simonSpeed || 800;

            const total = Math.max(1, d.simonTotalSteps || 1);
            const cur = Math.max(0, (d.simonState === 'À VOUS !' ? (d.simonStep - 1) : d.simonStep));
            const pct = Math.min(100, Math.round((cur / total) * 100));
            const pBar = document.getElementById('barSimonProg');
            if (pBar) pBar.style.width = pct + '%';
          } else {
            simonView.style.display = 'none';
          }
        }

        // Sync select dropdown
        const sel = document.getElementById('selMode');
        if (sel && document.activeElement !== sel) {
          sel.value = d.mode;
        }

        isPausedState = d.paused;
        document.getElementById('btnMainPause').innerText = d.paused ? 'Reprendre' : 'Pause';
      }).catch(()=>{});
  }

  function pollPods() {
    fetch('/api/pods')
      .then(r => r.json())
      .then(d => {
        const onlinePods = d.pods.filter(p => p.online !== false);
        const offlineCount = d.pods.length - onlinePods.length;

        if (offlineCount > 0) {
          document.getElementById('headerFleet').innerText = `${onlinePods.length}/${d.pods.length} Pods`;
        } else {
          document.getElementById('headerFleet').innerText = `${onlinePods.length} Pod${onlinePods.length > 1 ? 's' : ''}`;
        }

        let html = '';
        d.pods.forEach(p => {
          const isOnline = (p.online !== false);
          const chgIcon = p.charging ? '⚡ ' : '';
          const batWarn = p.batPct <= 20 ? 'style="color:var(--accent-red); font-weight:700;"' : '';
          const numStr = p.name.replace('Pod ', '').replace(' (Master)', '');

          if (isOnline) {
            html += `
              <div class="pod-card ${p.isTarget ? 'is-active-target' : ''}" onclick="apiManual({action:'SET_POD', podId:'${p.id}', color:'${currentColor}'})">
                <div class="pod-avatar" style="${p.isTarget ? 'background:#30d158; border-color:#30d158; color:#000;' : ''}">${numStr}</div>
                <div class="pod-title">${p.name}</div>
                <div class="pod-subtitle" ${batWarn}>${chgIcon}${p.batPct}% (${p.vBat}V) ${p.isMaster ? '• Master' : '• ' + p.rssi + 'dBm'}</div>
              </div>`;
          } else {
            html += `
              <div class="pod-card is-offline" style="opacity:0.38; cursor:not-allowed;" title="Ce pod est en veille ou éteint">
                <div class="pod-avatar" style="background:rgba(255,255,255,0.06); color:var(--text-3); border-style:dashed;">${numStr}</div>
                <div class="pod-title" style="color:var(--text-3);">${p.name}</div>
                <div class="pod-subtitle" style="color:var(--accent-orange);">💤 En veille (${p.lastSeenSec || '?'}s)</div>
              </div>`;
          }
        });
        document.getElementById('gridPodsFleet').innerHTML = html;
        renderFleetBatteryList(d.pods);
      }).catch(()=>{});
  }

  function renderFleetBatteryList(pods) {
    const listEl = document.getElementById('fleetBatteryList');
    if (!listEl) return;
    if (!pods || pods.length === 0) {
      listEl.innerHTML = '<div style="padding:16px; text-align:center; color:var(--text-3); font-size:0.8rem;">Aucun pod détecté</div>';
      return;
    }
    let html = '';
    pods.forEach(p => {
      const isOnline = (p.online !== false);
      let batColor = isOnline ? 'var(--accent-green)' : 'var(--text-3)';
      if (isOnline) {
        if (p.batPct <= 20) batColor = 'var(--accent-red)';
        else if (p.batPct <= 45) batColor = 'var(--accent-yellow)';
      }

      let statusDesc = '';
      if (p.isMaster) {
        statusDesc = p.charging ? 'En charge (USB)' : 'Pod Principal';
      } else if (!isOnline) {
        statusDesc = `<span style="color:var(--accent-orange);">💤 En veille (${p.lastSeenSec || '?'}s)</span>`;
      } else {
        statusDesc = `${p.charging ? 'En charge • ' : ''}${p.rssi} dBm`;
      }

      const numStr = p.name.replace('Pod ', '').replace(' (Master)', '');

      html += `
        <div class="ios-row" style="cursor:default; ${!isOnline ? 'opacity:0.6;' : ''}">
          <div class="ios-row-left">
            <div class="ios-icon-box" style="background:${p.isMaster ? 'var(--accent-blue)' : (isOnline ? 'var(--surface-3)' : 'rgba(255,255,255,0.06)')}; font-weight:700; font-size:0.85rem; color:${isOnline ? '#fff' : 'var(--text-3)'};">
              ${numStr}
            </div>
            <div class="ios-row-text">
              <div class="ios-row-title" style="${!isOnline ? 'color:var(--text-2);' : ''}">${p.name} ${!isOnline ? '<span style="font-size:0.68rem; color:var(--accent-orange); font-weight:600;">(VEILLE)</span>' : ''}</div>
              <div class="ios-row-subtitle">${p.mac || p.id} • ${statusDesc}</div>
            </div>
          </div>
          <div class="ios-row-right">
            <div style="display:flex; flex-direction:column; align-items:flex-end; gap:3px; min-width:75px;">
              <div style="font-weight:700; font-size:0.92rem; color:${batColor}; display:flex; align-items:center; gap:3px;">
                ${p.charging ? '<span style="font-size:0.75rem;">⚡</span>' : ''}${p.batPct}%
              </div>
              <div style="width:68px; height:5px; background:rgba(255,255,255,0.1); border-radius:3px; overflow:hidden;">
                <div style="width:${Math.min(100, Math.max(0, p.batPct))}%; height:100%; background:${batColor}; border-radius:3px;"></div>
              </div>
              <div style="font-size:0.68rem; color:var(--text-3);">${p.vBat} V</div>
            </div>
          </div>
        </div>`;
    });
    listEl.innerHTML = html;
  }

  function pollLogs() {
    fetch('/api/logs')
      .then(r => r.json())
      .then(d => {
        let html = '';
        d.logs.forEach(l => {
          html += `<div><span class="log-tag tag-${l.lvl}">[${l.lvl}]</span> ${l.msg}</div>`;
        });
        const term = document.getElementById('consoleLogs');
        if (term) {
          term.innerHTML = html;
          term.scrollTop = term.scrollHeight;
        }
      }).catch(()=>{});
  }

  function pollHistory() {
    fetch('/api/history')
      .then(r => r.json())
      .then(d => {
        historyRaw = d.hits;
        renderHighPrecisionChart();
      }).catch(()=>{});
  }

  // Graphique de réactivité dynamique avec auto-adaptation de l'échelle (>500ms supporté)
  function renderHighPrecisionChart() {
    const canvas = document.getElementById('liveChart');
    if (!canvas) return;
    const ctx = canvas.getContext('2d');
    canvas.width = canvas.parentElement.clientWidth;
    canvas.height = canvas.parentElement.clientHeight;

    const w = canvas.width;
    const h = canvas.height;
    ctx.clearRect(0, 0, w, h);

    let maxVal = 400;
    if (historyRaw && historyRaw.length > 0) {
      maxVal = Math.max(...historyRaw.map(hit => hit.rt), 350);
    }
    let maxY = Math.ceil((maxVal * 1.15) / 100) * 100;
    if (maxY < 400) maxY = 400;

    const yAxisEl = document.getElementById('chartYAxis');
    if (yAxisEl) {
      yAxisEl.innerHTML = `
        <span>${maxY}ms</span>
        <span>${Math.round(maxY * 0.75)}ms</span>
        <span>${Math.round(maxY * 0.50)}ms</span>
        <span>${Math.round(maxY * 0.25)}ms</span>
        <span>0ms</span>
      `;
    }

    ctx.strokeStyle = 'rgba(255, 255, 255, 0.08)';
    ctx.lineWidth = 1;
    [0.25, 0.50, 0.75].forEach(ratio => {
      const y = h - (ratio * (h - 24)) - 12;
      ctx.beginPath();
      ctx.moveTo(0, y);
      ctx.lineTo(w, y);
      ctx.stroke();
    });

    if (historyRaw.length < 1) return;

    const sum = historyRaw.reduce((acc, cur) => acc + cur.rt, 0);
    const avg = sum / historyRaw.length;
    const avgY = h - (Math.min(avg, maxY) / maxY * (h - 24)) - 12;

    ctx.strokeStyle = 'rgba(255, 214, 10, 0.55)';
    ctx.lineWidth = 1.5;
    ctx.setLineDash([4, 4]);
    ctx.beginPath();
    ctx.moveTo(0, avgY);
    ctx.lineTo(w, avgY);
    ctx.stroke();
    ctx.setLineDash([]);

    if (historyRaw.length < 2) {
      const y = h - (Math.min(historyRaw[0].rt, maxY) / maxY * (h - 24)) - 12;
      ctx.fillStyle = historyRaw[0].col || '#0a84ff';
      ctx.beginPath();
      ctx.arc(w / 2, y, 4.5, 0, Math.PI * 2);
      ctx.fill();
      ctx.strokeStyle = '#FFFFFF';
      ctx.lineWidth = 1.5;
      ctx.stroke();
      return;
    }

    const step = w / (historyRaw.length - 1);
    const grad = ctx.createLinearGradient(0, 0, 0, h);
    grad.addColorStop(0, 'rgba(10, 132, 255, 0.15)');
    grad.addColorStop(1, 'rgba(10, 132, 255, 0.0)');

    ctx.fillStyle = grad;
    ctx.beginPath();
    ctx.moveTo(0, h - 12);
    historyRaw.forEach((hit, i) => {
      const x = i * step;
      const y = h - (Math.min(hit.rt, maxY) / maxY * (h - 24)) - 12;
      if (i === 0) ctx.lineTo(x, y);
      else {
        const prevX = (i - 1) * step;
        const prevY = h - (Math.min(historyRaw[i - 1].rt, maxY) / maxY * (h - 24)) - 12;
        const midX = (prevX + x) / 2;
        ctx.bezierCurveTo(midX, prevY, midX, y, x, y);
      }
    });
    ctx.lineTo(w, h - 12);
    ctx.closePath();
    ctx.fill();

    ctx.strokeStyle = '#0a84ff';
    ctx.lineWidth = 2.5;
    ctx.beginPath();
    historyRaw.forEach((hit, i) => {
      const x = i * step;
      const y = h - (Math.min(hit.rt, maxY) / maxY * (h - 24)) - 12;
      if (i === 0) ctx.moveTo(x, y);
      else {
        const prevX = (i - 1) * step;
        const prevY = h - (Math.min(historyRaw[i - 1].rt, maxY) / maxY * (h - 24)) - 12;
        const midX = (prevX + x) / 2;
        ctx.bezierCurveTo(midX, prevY, midX, y, x, y);
      }
    });
    ctx.stroke();

    historyRaw.forEach((hit, i) => {
      const x = i * step;
      const y = h - (Math.min(hit.rt, maxY) / maxY * (h - 24)) - 12;
      ctx.fillStyle = hit.col || '#0a84ff';
      ctx.beginPath();
      ctx.arc(x, y, 4, 0, Math.PI * 2);
      ctx.fill();
      ctx.strokeStyle = '#FFFFFF';
      ctx.lineWidth = 1.5;
      ctx.stroke();
    });
  }

  // Graphique de niveau et charge batterie — REPLIQUE EXACTE APPLE IOS
  function renderBatteryChart() {
    const canvas = document.getElementById('batteryChart');
    if (!canvas) return;
    const ctx = canvas.getContext('2d');
    canvas.width = canvas.parentElement.clientWidth;
    canvas.height = canvas.parentElement.clientHeight;

    const w = canvas.width;
    const h = canvas.height;
    ctx.clearRect(0, 0, w, h);

    const rightMargin = 38;
    const bottomMargin = 26;
    const gw = w - rightMargin;
    const gh = h - bottomMargin;
    const topY = 6;

    // 1. Lignes horizontales de repère (100%, 50%, 0%)
    ctx.strokeStyle = 'rgba(255, 255, 255, 0.12)';
    ctx.lineWidth = 1;
    [0, 0.5, 1].forEach(ratio => {
      const y = topY + (ratio * gh);
      ctx.beginPath();
      ctx.moveTo(0, y);
      ctx.lineTo(gw, y);
      ctx.stroke();
    });

    // 2. Lignes verticales pointillées et repères horaires
    ctx.strokeStyle = 'rgba(255, 255, 255, 0.15)';
    ctx.lineWidth = 1;
    ctx.setLineDash([2, 3]);

    const numTimeSlots = 6;
    const timeLabels = ['-60m', '-45m', '-30m', '-15m', '-5m', 'Maintenant'];

    for (let i = 0; i < numTimeSlots; i++) {
      const x = (i / (numTimeSlots - 1)) * gw;
      ctx.beginPath();
      ctx.moveTo(x, topY);
      ctx.lineTo(x, topY + gh);
      ctx.stroke();

      // Label temporel
      ctx.fillStyle = '#8e8e93';
      ctx.font = '9px -apple-system, sans-serif';
      ctx.textAlign = (i === 0) ? 'left' : (i === numTimeSlots - 1 ? 'right' : 'center');
      ctx.fillText(timeLabels[i], x, topY + gh + 14);
    }
    ctx.setLineDash([]);

    if (!batHistoryRaw || batHistoryRaw.length === 0) return;

    // Échantillons pour le tracé en barres fines
    const maxBars = 42;
    let samples = [...batHistoryRaw];
    while (samples.length < maxBars) {
      samples.push(samples[samples.length - 1]);
    }
    if (samples.length > maxBars) {
      samples = samples.slice(samples.length - maxBars);
    }

    const n = samples.length;
    const barWidth = 3.5;
    const step = gw / n;

    // 3. Zones translucides de charge en arrière-plan
    let inCharge = false;
    let chargeStartIdx = 0;

    for (let i = 0; i <= n; i++) {
      const chg = (i < n) ? samples[i].chg : false;
      if (chg && !inCharge) {
        inCharge = true;
        chargeStartIdx = i;
      } else if (!chg && inCharge) {
        inCharge = false;
        const x1 = chargeStartIdx * step;
        const x2 = i * step;
        ctx.fillStyle = 'rgba(52, 199, 89, 0.22)';
        ctx.fillRect(x1, topY, (x2 - x1), gh);

        // Ligne verte & symbole d'éclair vectoriel sur l'axe en bas
        ctx.fillStyle = '#34c759';
        const pillY = topY + gh + 18;
        ctx.beginPath();
        ctx.roundRect(x1, pillY, (x2 - x1), 3.5, 2);
        ctx.fill();

        const cx = (x1 + x2) / 2;
        const cy = pillY - 7;
        ctx.beginPath();
        ctx.moveTo(cx + 1, cy - 5);
        ctx.lineTo(cx - 3, cy);
        ctx.lineTo(cx, cy);
        ctx.lineTo(cx - 1, cy + 5);
        ctx.lineTo(cx + 3, cy);
        ctx.lineTo(cx, cy);
        ctx.closePath();
        ctx.fill();
      }
    }

    // 4. Barres verticales de niveau de batterie (Vert iOS #34c759)
    samples.forEach((s, i) => {
      const x = i * step + (step - barWidth) / 2;
      const barH = (Math.max(4, s.pct) / 100) * gh;
      const y = topY + gh - barH;

      ctx.fillStyle = s.pct <= 20 && !s.chg ? '#ff453a' : '#34c759';
      ctx.beginPath();
      ctx.roundRect(x, y, barWidth, barH, [2, 2, 0, 0]);
      ctx.fill();
    });
  }

  setInterval(pollStatus, 300);
  setInterval(pollPods, 1200);
  setInterval(pollLogs, 2000);
  setInterval(pollHistory, 1000);
</script>

</body>
</html>)rawliteral";

void WebConfigManager::handleRoot() {
  server.send_P(200, "text/html", DASHBOARD_HTML, sizeof(DASHBOARD_HTML) - 1);
}