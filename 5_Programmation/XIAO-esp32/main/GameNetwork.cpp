#include "GameNetwork.h"
#include <algorithm> // For std::find

GameNetworkManager GameNetwork;

// Helper to convert uint64_t to MAC address bytes (ensuring correct order)
void uint64ToMac(uint64_t id, uint8_t mac[6]) {
  mac[5] = (id >> 0) & 0xFF;
  mac[4] = (id >> 8) & 0xFF;
  mac[3] = (id >> 16) & 0xFF;
  mac[2] = (id >> 24) & 0xFF;
  mac[1] = (id >> 32) & 0xFF;
  mac[0] = (id >> 40) & 0xFF;
}

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
  if (GameNetwork.lastMessage.senderID == GameNetwork.getMyId()) { // Use getter
    DEBUG_PRINT(DEBUG_VERBOSE, "Ignoring own message (ID: %llu).", GameNetwork.getMyId()); // Use getter
    return;
  }

  // Ajouter l'expéditeur automatiquement si inconnu
  if (GameNetwork.addNode(GameNetwork.lastMessage.senderID)) { // addNode now returns bool
    DEBUG_PRINT(DEBUG_INFO, "Node %llu discovered and added.", GameNetwork.lastMessage.senderID);
  } else {
    DEBUG_PRINT(DEBUG_VERBOSE, "Node %llu already known or max nodes reached.", GameNetwork.lastMessage.senderID);
  }
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
  if (esp_now_add_peer(&peerInfo) != ESP_OK) { // Check return value
    DEBUG_PRINT(DEBUG_ERROR, "Failed to add broadcast peer.");
  }

  myId = ESP.getEfuseMac(); // Initialize private myId
  numKnownNodes = 0; // Ensure count is reset
  DEBUG_PRINT(DEBUG_INFO, "ESP-NOW initialized successfully. My ID: %llu", myId);
}

bool GameNetworkManager::addNode(uint64_t id) { // Returns bool now
  DEBUG_PRINT(DEBUG_VERBOSE, "Attempting to add node %llu.", id);
  // Vérifier si déjà connu
  for (size_t i = 0; i < numKnownNodes; ++i) { // Iterate over actual known nodes
    if (knownNodes[i] == id) {
      DEBUG_PRINT(DEBUG_VERBOSE, "Node %llu already known.", id);
      return false; // Node already known
    }
  }

  if (numKnownNodes >= MAX_NODES) { // Use numKnownNodes
    DEBUG_PRINT(DEBUG_WARNING, "Max nodes (%d) reached. Cannot add node %llu.", MAX_NODES, id);
    return false; // Max nodes reached
  }

  knownNodes[numKnownNodes++] = id; // Add to array and increment count
  DEBUG_PRINT(DEBUG_INFO, "Node %llu added to knownNodes. Total: %d", id, numKnownNodes);

  // Ajouter au niveau ESP-NOW pour pouvoir lui parler en direct
  esp_now_peer_info_t peer = {};
  uint8_t macAddr[6]; // Use helper function for conversion
  uint64ToMac(id, macAddr);
  memcpy(peer.peer_addr, macAddr, 6); // Copy converted MAC
  peer.channel = 1;
  peer.encrypt = false;

  if (!esp_now_is_peer_exist(peer.peer_addr)) {
    if (esp_now_add_peer(&peer) != ESP_OK) { // Check return value
      DEBUG_PRINT(DEBUG_ERROR, "Failed to add ESP-NOW peer %llu.", id);
      return false; // Failed to add peer
    }
    DEBUG_PRINT(DEBUG_VERBOSE, "Added ESP-NOW peer %llu.", id);
  }
  return true; // Node added successfully
}

void GameNetworkManager::broadcast(uint8_t type, const uint32_t col) { // Added const
  struct_message msg;
  msg.msgType = type;
  msg.color = col;
  msg.senderID = getMyId(); // Use getter
  msg.targetID = 0;  // 0 = Broadcast target

  esp_now_send(broadcastAddr, (uint8_t *)&msg, sizeof(msg));
  DEBUG_PRINT(DEBUG_VERBOSE, "Broadcasting message type %d, color %X. Sender: %llu", type, col, getMyId()); // Use getter
}

void GameNetworkManager::sendTo(uint64_t target, uint8_t type, const uint32_t col) { // Added const
  struct_message msg;
  msg.msgType = type;
  msg.color = col;
  msg.senderID = getMyId(); // Use getter
  msg.targetID = target;

  uint8_t targetMac[6];
  uint64ToMac(target, targetMac); // Use helper function for conversion

  esp_now_send(targetMac, (uint8_t *)&msg, sizeof(msg));
  DEBUG_PRINT(DEBUG_VERBOSE, "Sending message type %d, color %X to target %llu. Sender: %llu", type, col, target, getMyId()); // Use getter
}

std::vector<uint64_t> GameNetworkManager::getKnownNodes() const {
  std::vector<uint64_t> nodesVec;
  for (size_t i = 0; i < numKnownNodes; ++i) {
    nodesVec.push_back(knownNodes[i]);
  }
  return nodesVec;
}