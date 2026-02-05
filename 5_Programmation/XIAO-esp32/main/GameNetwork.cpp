#include "GameNetwork.h"

GameNetworkManager GameNetwork;

// Callbacks globaux obligatoires pour ESP-NOW
void OnDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  DEBUG_PRINT(DEBUG_VERBOSE, "ESP-NOW data sent status: %d", status);
}

void OnDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len) {
  if (len != sizeof(struct_message)) {
    DEBUG_PRINT(DEBUG_WARNING, "Received ESP-NOW message of incorrect length: %d (expected %d)", len, sizeof(struct_message));
    return;
  }

  memcpy(&GameNetwork.lastMessage, data, sizeof(struct_message));

  // Ignore ses propres messages
  if (GameNetwork.lastMessage.senderID == GameNetwork.myId) {
    DEBUG_PRINT(DEBUG_VERBOSE, "Ignoring own message (ID: %llu).", GameNetwork.myId);
    return;
  }

  // Ajouter l'expéditeur automatiquement si inconnu
  GameNetwork.addNode(GameNetwork.lastMessage.senderID);
  DEBUG_PRINT(DEBUG_INFO, "Node %llu discovered and added.", GameNetwork.lastMessage.senderID);
  DEBUG_PRINT(DEBUG_VERBOSE, "ESP-NOW data received. Length: %d. Sender: %llu", len, GameNetwork.lastMessage.senderID);

  GameNetwork.messageReceived = true;
}

void GameNetworkManager::init() {
  DEBUG_PRINT(DEBUG_INFO, "Initializing GameNetwork...");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    DEBUG_PRINT(DEBUG_ERROR, "ESP-NOW init failed. Restarting...");
    ESP.restart();
  }

  // Register Callbacks
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  // Register Broadcast Peer
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddr, 6);
  peerInfo.channel = 1;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);

  myId = ESP.getEfuseMac();
  DEBUG_PRINT(DEBUG_INFO, "ESP-NOW initialized successfully. My ID: %llu", myId);
}

void GameNetworkManager::addNode(uint64_t id) {
  DEBUG_PRINT(DEBUG_VERBOSE, "Attempting to add node %llu.", id);
  // Vérifier si déjà connu
  for (auto node : knownNodes) {
    if (node == id) {
      DEBUG_PRINT(DEBUG_VERBOSE, "Node %llu already known.", id);
      return;
    }
  }

  if (knownNodes.size() >= MAX_NODES) {
    DEBUG_PRINT(DEBUG_WARNING, "Max nodes (%d) reached. Cannot add node %llu.", MAX_NODES, id);
    return;
  }

  knownNodes.push_back(id);
  DEBUG_PRINT(DEBUG_INFO, "Node %llu added to knownNodes. Total: %d", id, knownNodes.size());

  // Ajouter au niveau ESP-NOW pour pouvoir lui parler en direct
  esp_now_peer_info_t peer = {};
  // Conversion ID (uint64) vers Mac (uint8[6]) optimisée
  memcpy(peer.peer_addr, &id, 6);
  peer.channel = 1;
  peer.encrypt = false;

  if (!esp_now_is_peer_exist(peer.peer_addr)) {
    esp_now_add_peer(&peer);
    DEBUG_PRINT(DEBUG_VERBOSE, "Added ESP-NOW peer %llu.", id);
  }
}

void GameNetworkManager::broadcast(uint8_t type, uint32_t col) {
  struct_message msg;
  msg.msgType = type;
  msg.color = col;
  msg.senderID = myId;
  msg.targetID = 0;  // 0 = Broadcast target

  esp_now_send(broadcastAddr, (uint8_t *)&msg, sizeof(msg));
  DEBUG_PRINT(DEBUG_VERBOSE, "Broadcasting message type %d, color %X. Sender: %llu", type, col, myId);
}

void GameNetworkManager::sendTo(uint64_t target, uint8_t type, uint32_t col) {
  struct_message msg;
  msg.msgType = type;
  msg.color = col;
  msg.senderID = myId;
  msg.targetID = target;

  uint8_t targetMac[6];
  memcpy(targetMac, &target, 6);

  esp_now_send(targetMac, (uint8_t *)&msg, sizeof(msg));
  DEBUG_PRINT(DEBUG_VERBOSE, "Sending message type %d, color %X to target %llu. Sender: %llu", type, col, target, myId);
}