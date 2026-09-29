#include "GameNetwork.h"
#include "WebConfig.h"
#include "GameEngine.h"
#include "Hardware.h"
#include <algorithm>
#include <esp_wifi.h>

GameNetworkManager GameNetwork;

// Callbacks globaux obligatoires pour ESP-NOW
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  DEBUG_PRINT(DEBUG_VERBOSE, "ESP-NOW data sent status: %d", status);
}

void OnDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len) {
  if (len != sizeof(struct_message)) {
    DEBUG_PRINT(DEBUG_WARNING, "Received ESP-NOW message of incorrect length: %d (expected %d)", len, sizeof(struct_message));
    return;
  }

  struct_message incomingMsg;
  memcpy(&incomingMsg, data, sizeof(struct_message));

  int8_t rssi = 0;
  if (recv_info && recv_info->rx_ctrl != NULL) {
    rssi = recv_info->rx_ctrl->rssi;
  }
#else
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  DEBUG_PRINT(DEBUG_VERBOSE, "ESP-NOW data sent status: %d", status);
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int len) {
  if (len != sizeof(struct_message)) {
    DEBUG_PRINT(DEBUG_WARNING, "Received ESP-NOW message of incorrect length: %d (expected %d)", len, sizeof(struct_message));
    return;
  }

  struct_message incomingMsg;
  memcpy(&incomingMsg, data, sizeof(struct_message));

  int8_t rssi = 0;
#endif

  // 1. Ignorer ses propres messages
  if (incomingMsg.senderID == GameNetwork.getMyId()) {
    return;
  }

  // 2. Filtrer les messages déjà vus (anti-boucle mesh)
  if (GameNetwork.isMessageSeen(incomingMsg.senderID, incomingMsg.msgId)) {
    return;
  }
  
  // 3. Marquer le message comme vu
  GameNetwork.markMessageSeen(incomingMsg.senderID, incomingMsg.msgId);

  // 4. Découverte automatique : ajouter ou mettre à jour l'expéditeur et son RSSI
  GameNetwork.addNode(incomingMsg.senderID, rssi);

  if (incomingMsg.msgType == CMD_POD_TELEMETRY) {
    uint8_t pct = incomingMsg.color & 0xFF;
    float volt = ((incomingMsg.color >> 8) & 0xFFFF) / 100.0f;
    bool chg = (incomingMsg.color >> 24) & 0x01;
    GameNetwork.updateNodeTelemetry(incomingMsg.senderID, pct, volt, chg);
  }

  if (incomingMsg.msgType == CMD_HEARTBEAT) {
    GameNetwork.lastMasterHeartbeat = millis();
    GameNetwork.currentMasterId = incomingMsg.senderID;

    // Arbitrage en cas de split-brain (si deux pods se croient Master)
    if (GameNetwork.isFleetMaster && incomingMsg.senderID != GameNetwork.getMyId()) {
      if (incomingMsg.senderID < GameNetwork.getMyId()) {
        DEBUG_PRINT(DEBUG_WARNING, "Conflit Master : Pod %llu a priorité. Cession du rôle.", incomingMsg.senderID);
        GameNetwork.demoteToSlave();
      }
    }
  }

  if (incomingMsg.msgType == CMD_CLAIM_MASTER) {
    GameNetwork.currentMasterId = incomingMsg.senderID;
    GameNetwork.lastMasterHeartbeat = millis();

    if (GameNetwork.isFleetMaster && incomingMsg.senderID != GameNetwork.getMyId()) {
      DEBUG_PRINT(DEBUG_INFO, "Pod %llu réclame le rôle Master. Cession du rôle.", incomingMsg.senderID);
      GameNetwork.demoteToSlave();
    }
  }

  DEBUG_PRINT(DEBUG_VERBOSE, "RX msgId %lu, type %d, sender %llu, target %llu, ttl %d, RSSI %d",
              incomingMsg.msgId, incomingMsg.msgType, incomingMsg.senderID, incomingMsg.targetID, incomingMsg.ttl, rssi);

  // 5. Si le message est un broadcast (targetID == 0) OU s'il m'est personnellement destiné
  if (incomingMsg.targetID == 0 || incomingMsg.targetID == GameNetwork.getMyId()) {
    GameNetwork.pushRxMessage(incomingMsg);
  }

  // Enregistrement debug du dernier message reçu
  GameNetwork.recordRxMessage(incomingMsg);

  // 6. Relais Mesh (Multi-hop) : Si le message n'est pas UNIQUEMENT pour moi (broadcast ou destiné à un autre pod)
  // et qu'il lui reste du TTL (> 1)
  if (incomingMsg.ttl > 1 && incomingMsg.targetID != GameNetwork.getMyId()) {
    struct_message relayMsg = incomingMsg;
    relayMsg.ttl--; // Décrémentation du nombre de sauts
    GameNetwork.queueRelay(relayMsg);
  }
}

void GameNetworkManager::init() {
  DEBUG_PRINT(DEBUG_INFO, "Initializing GameNetwork in pure WIFI_STA mode...");
  WiFi.mode(WIFI_STA); // Mode Station pur par défaut (SoftAP uniquement sur Master)
  WiFi.setSleep(false);   // Désactive la mise en veille du WiFi (crucial pour ESP-NOW sur C3)
  
  // Verrouillage du canal WiFi sur le canal 1
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);

  uint8_t channel;
  wifi_second_chan_t second;
  esp_wifi_get_channel(&channel, &second);
  DEBUG_PRINT(DEBUG_INFO, "WiFi Channel locked to: %d", channel);

  if (esp_now_init() != ESP_OK) {
    DEBUG_PRINT(DEBUG_ERROR, "ESP-NOW init failed. Restarting...");
    ESP.restart();
  }

  // Register Callbacks
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  // Register Broadcast Peer (seul pair requis pour le Mesh Flooding)
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddr, 6);
  peerInfo.channel = 1;
  peerInfo.encrypt = false;
  peerInfo.ifidx = WIFI_IF_STA; // Indispensable pour ESP32-C3
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    DEBUG_PRINT(DEBUG_ERROR, "Failed to add broadcast peer.");
  }

  // Use WiFi MAC address directly
  uint8_t mac[6];
  WiFi.macAddress(mac);
  myId = 0;
  for (int i = 0; i < 6; i++) {
    myId = (myId << 8) | mac[i];
  }
  
  // Clear buffers & queues
  portENTER_CRITICAL(&networkMux);
  memset(seenMessages, 0, sizeof(seenMessages));
  seenMessagesIndex = 0;
  numKnownNodes = 0;
  while (!rxQueue.empty()) rxQueue.pop();
  relayList.clear();
  portEXIT_CRITICAL(&networkMux);

  isFleetMaster = false;
  currentMasterId = 0;
  lastMasterHeartbeat = millis();
  bootTimestamp = millis();
  initialElectionDone = false;
  lastHeartbeatSent = 0;

  DEBUG_PRINT(DEBUG_INFO, "ESP-NOW initialized successfully (WIFI_STA). My ID: %llu", myId);
}

void GameNetworkManager::claimMasterRole(bool userGesture) {
  if (isFleetMaster && WebConfig.isApActive()) return;

  isFleetMaster = true;
  currentMasterId = myId;
  lastMasterHeartbeat = millis();

  // Démarrer SoftAP + DNS + Serveur Web
  WebConfig.startSoftAP();

  // Diffuser la prise de contrôle sur le réseau ESP-NOW
  broadcast(CMD_CLAIM_MASTER, userGesture ? 1 : 0);
  Game.addLog("Rôle MASTER endossé (SoftAP actif)", "NETWORK");
}

void GameNetworkManager::demoteToSlave() {
  if (!isFleetMaster) return;

  isFleetMaster = false;

  // Couper immédiatement le SoftAP pour libérer le spectre RF
  WebConfig.stopSoftAP();
  Game.addLog("Rétrogradation en rôle ESCLAVE (SoftAP coupé)", "NETWORK");
}

void GameNetworkManager::checkMasterWatchdog() {
  unsigned long now = millis();

  // 1. Si nous sommes le Master actif : diffuser le heartbeat périodique
  if (isFleetMaster) {
    if (now - lastHeartbeatSent >= MASTER_HEARTBEAT_INTERVAL_MS) {
      lastHeartbeatSent = now;
      uint8_t bat = 85;
#if ENABLE_BATTERY_MANAGEMENT
      bat = (uint8_t)Hardware.getBatteryPercentage();
#endif
      uint32_t payload = (Game.getCurrentModeIndex() & 0xFF) |
                         ((Game.isRunning() ? 1 : 0) << 8) |
                         ((Game.isPaused() ? 1 : 0) << 9) |
                         ((Game.isManualActive() ? 1 : 0) << 10) |
                         (((uint32_t)bat) << 16);
      broadcast(CMD_HEARTBEAT, payload);
    }
    return;
  }

  // 2. Élection initiale au démarrage (après une fenêtre d'écoute sans heartbeat)
  if (!initialElectionDone && (now - bootTimestamp >= BOOT_ELECTION_WINDOW_MS)) {
    initialElectionDone = true;
    if (currentMasterId == 0) {
      uint64_t bestCandidate = myId;
      portENTER_CRITICAL(&networkMux);
      for (size_t i = 0; i < numKnownNodes; ++i) {
        if (knownNodes[i].id != 0 && (now - knownNodes[i].lastSeen < 3500)) {
          if (knownNodes[i].id < bestCandidate) {
            bestCandidate = knownNodes[i].id;
          }
        }
      }
      portEXIT_CRITICAL(&networkMux);

      if (bestCandidate == myId) {
        DEBUG_PRINT(DEBUG_INFO, "Élection initiale : ce pod devient Master");
        claimMasterRole(false);
      }
    }
  }

  // 3. Watchdog de perte du Master (aucun heartbeat depuis > 3000 ms)
  if (currentMasterId != 0 && (now - lastMasterHeartbeat > MASTER_TIMEOUT_MS)) {
    DEBUG_PRINT(DEBUG_WARNING, "Master perdu (timeout %d ms) ! Déclenchement ré-élection...", MASTER_TIMEOUT_MS);
    currentMasterId = 0;

    uint64_t bestCandidate = myId;
    portENTER_CRITICAL(&networkMux);
    for (size_t i = 0; i < numKnownNodes; ++i) {
      if (knownNodes[i].id != 0 && (now - knownNodes[i].lastSeen < 4000)) {
        if (knownNodes[i].id < bestCandidate) {
          bestCandidate = knownNodes[i].id;
        }
      }
    }
    portEXIT_CRITICAL(&networkMux);

    if (bestCandidate == myId) {
      DEBUG_PRINT(DEBUG_INFO, "Ré-élection : ce pod prend la relève en tant que Master !");
      claimMasterRole(false);
    }
  }
}

void GameNetworkManager::update() {
  checkMasterWatchdog();

  unsigned long now = millis();
  std::vector<struct_message> toSend;

  // Récupérer les messages prêts à être relayés (délai de jitter échu)
  portENTER_CRITICAL(&networkMux);
  for (auto it = relayList.begin(); it != relayList.end(); ) {
    if (now >= it->sendTime) {
      toSend.push_back(it->msg);
      it = relayList.erase(it);
    } else {
      ++it;
    }
  }
  portEXIT_CRITICAL(&networkMux);

  // Envoi hors de la section critique
  for (const auto &msg : toSend) {
    esp_now_send(broadcastAddr, (const uint8_t *)&msg, sizeof(msg));
    DEBUG_PRINT(DEBUG_VERBOSE, "Relayed mesh msgId %lu (type %d, target %llu, ttl %d)",
                msg.msgId, msg.msgType, msg.targetID, msg.ttl);
  }

  // Nettoyage périodique des nœuds hors ligne / en veille prolongée (> 60s)
  static unsigned long lastPrune = 0;
  if (now - lastPrune > 5000) {
    lastPrune = now;
    portENTER_CRITICAL(&networkMux);
    for (size_t i = 0; i < numKnownNodes; ) {
      if (now - knownNodes[i].lastSeen > 60000) {
        for (size_t j = i; j < numKnownNodes - 1; ++j) {
          knownNodes[j] = knownNodes[j + 1];
        }
        numKnownNodes--;
      } else {
        ++i;
      }
    }
    portEXIT_CRITICAL(&networkMux);
  }
}

void GameNetworkManager::queueRelay(const struct_message &msg) {
  QueuedRelay qr;
  qr.msg = msg;
  // Jitter aléatoire entre 5 et 25 ms pour désynchroniser les relais et éviter les collisions radio
  qr.sendTime = millis() + random(5, 25);

  portENTER_CRITICAL(&networkMux);
  relayList.push_back(qr);
  portEXIT_CRITICAL(&networkMux);
}

void GameNetworkManager::pushRxMessage(const struct_message &msg) {
  portENTER_CRITICAL(&networkMux);
  rxQueue.push(msg);
  portEXIT_CRITICAL(&networkMux);
}

void GameNetworkManager::recordRxMessage(const struct_message &msg) {
  portENTER_CRITICAL(&networkMux);
  lastRxMsg = msg;
  lastRxTime = millis();
  hasRxRecord = true;
  portEXIT_CRITICAL(&networkMux);
}

bool GameNetworkManager::getNextRxMessage(struct_message &outMsg) {
  portENTER_CRITICAL(&networkMux);
  if (rxQueue.empty()) {
    portEXIT_CRITICAL(&networkMux);
    return false;
  }
  outMsg = rxQueue.front();
  rxQueue.pop();
  portEXIT_CRITICAL(&networkMux);
  return true;
}

bool GameNetworkManager::addNode(uint64_t id, int8_t rssi) {
  if (id == 0 || id == myId) return false;

  portENTER_CRITICAL(&networkMux);
  for (size_t i = 0; i < numKnownNodes; ++i) {
    if (knownNodes[i].id == id) {
      knownNodes[i].lastSeen = millis();
      if (rssi != 0) knownNodes[i].rssi = rssi;
      portEXIT_CRITICAL(&networkMux);
      return false; // Déjà connu, mise à jour effectuée
    }
  }

  if (numKnownNodes >= MAX_NODES) {
    portEXIT_CRITICAL(&networkMux);
    DEBUG_PRINT(DEBUG_WARNING, "Max nodes (%d) reached. Cannot add node %llu.", MAX_NODES, id);
    return false;
  }

  NodeInfo &newNode = knownNodes[numKnownNodes++];
  newNode.id = id;
  newNode.rssi = rssi;
  newNode.batteryPct = 85; // Valeur par défaut
  newNode.voltage = 3.9f;
  newNode.isCharging = false;
  newNode.lastSeen = millis();
  newNode.isTarget = false;

  size_t count = numKnownNodes;
  portEXIT_CRITICAL(&networkMux);

  DEBUG_PRINT(DEBUG_INFO, "Node %llu discovered (RSSI: %d dBm). Total: %d", id, rssi, count);
  return true;
}

void GameNetworkManager::updateNodeTelemetry(uint64_t id, uint8_t batPct, float volt, bool isCharging) {
  if (id == 0 || id == myId) return;
  portENTER_CRITICAL(&networkMux);
  for (size_t i = 0; i < numKnownNodes; ++i) {
    if (knownNodes[i].id == id) {
      if (batPct > 0) knownNodes[i].batteryPct = batPct;
      if (volt > 0.0f) knownNodes[i].voltage = volt;
      knownNodes[i].isCharging = isCharging;
      knownNodes[i].lastSeen = millis();
      break;
    }
  }
  portEXIT_CRITICAL(&networkMux);
}

void GameNetworkManager::broadcast(uint8_t type, const uint32_t col) {
  struct_message msg;
  msg.msgId = ++localMessageCounter;
  msg.senderID = getMyId();
  msg.targetID = 0;  // 0 = Broadcast target
  msg.msgType = type;
  msg.ttl = DEFAULT_MESH_TTL;
  msg.color = col;

  markMessageSeen(getMyId(), msg.msgId);

  // Enregistrement debug du dernier message émis
  lastTxMsg = msg;
  lastTxTime = millis();
  hasTxRecord = true;

  esp_err_t err = esp_now_send(broadcastAddr, (uint8_t *)&msg, sizeof(msg));
  if (err != ESP_OK) {
    delay(2);
    esp_now_send(broadcastAddr, (uint8_t *)&msg, sizeof(msg));
  }
  DEBUG_PRINT(DEBUG_VERBOSE, "TX Broadcast msgId %lu, type %d, color %X (err %d)", msg.msgId, type, col, err);
}

void GameNetworkManager::sendTo(uint64_t target, uint8_t type, const uint32_t col) {
  struct_message msg;
  msg.msgId = ++localMessageCounter;
  msg.senderID = getMyId();
  msg.targetID = target; // Target spécifique
  msg.msgType = type;
  msg.ttl = DEFAULT_MESH_TTL;
  msg.color = col;

  markMessageSeen(getMyId(), msg.msgId);

  // Enregistrement debug du dernier message émis
  lastTxMsg = msg;
  lastTxTime = millis();
  hasTxRecord = true;

  // En Mesh Flooding, on diffuse via l'adresse broadcast pour que les nœuds intermédiaires puissent relayer
  esp_err_t err = esp_now_send(broadcastAddr, (uint8_t *)&msg, sizeof(msg));
  if (err != ESP_OK) {
    delay(2);
    esp_now_send(broadcastAddr, (uint8_t *)&msg, sizeof(msg));
  }
  DEBUG_PRINT(DEBUG_VERBOSE, "TX Directed msgId %lu, target %llu, type %d, color %X (err %d)", msg.msgId, target, type, col, err);
}

String GameNetworkManager::formatId(uint64_t id) {
  if (id == 0) return "0 (BROADCAST)";
  char buf[24];
  uint8_t mac[6];
  mac[5] = (id >> 0) & 0xFF;
  mac[4] = (id >> 8) & 0xFF;
  mac[3] = (id >> 16) & 0xFF;
  mac[2] = (id >> 24) & 0xFF;
  mac[1] = (id >> 32) & 0xFF;
  mac[0] = (id >> 40) & 0xFF;
  snprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(buf);
}

std::vector<uint64_t> GameNetworkManager::getKnownNodes() {
  std::vector<uint64_t> nodesVec;
  unsigned long now = millis();
  portENTER_CRITICAL(&networkMux);
  for (size_t i = 0; i < numKnownNodes; ++i) {
    if (now - knownNodes[i].lastSeen <= 12000) { // Uniquement les nœuds en ligne (< 12s)
      nodesVec.push_back(knownNodes[i].id);
    }
  }
  portEXIT_CRITICAL(&networkMux);
  return nodesVec;
}

std::vector<NodeInfo> GameNetworkManager::getNodesInfo() {
  std::vector<NodeInfo> nodesVec;
  portENTER_CRITICAL(&networkMux);
  for (size_t i = 0; i < numKnownNodes; ++i) {
    nodesVec.push_back(knownNodes[i]);
  }
  portEXIT_CRITICAL(&networkMux);
  return nodesVec;
}

bool GameNetworkManager::isMessageSeen(uint64_t senderId, uint32_t msgId) {
  if (msgId == 0) return false;
  portENTER_CRITICAL(&networkMux);
  for (int i = 0; i < MAX_SEEN_MESSAGES; i++) {
    if (seenMessages[i].msgId == msgId && seenMessages[i].senderId == senderId) {
      portEXIT_CRITICAL(&networkMux);
      return true;
    }
  }
  portEXIT_CRITICAL(&networkMux);
  return false;
}

void GameNetworkManager::markMessageSeen(uint64_t senderId, uint32_t msgId) {
  if (msgId == 0) return;
  portENTER_CRITICAL(&networkMux);
  seenMessages[seenMessagesIndex].senderId = senderId;
  seenMessages[seenMessagesIndex].msgId = msgId;
  seenMessagesIndex = (seenMessagesIndex + 1) % MAX_SEEN_MESSAGES;
  portEXIT_CRITICAL(&networkMux);
}