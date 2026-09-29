#ifndef GAMENETWORK_H
#define GAMENETWORK_H

#include "Config.h"
#include <WiFi.h>
#include <esp_now.h>
#include <array>
#include <vector>
#include <queue>
#include <freertos/FreeRTOS.h>

// Structure pour la gestion des messages à relayer avec délai aléatoire (anti-collision)
struct QueuedRelay {
  struct_message msg;
  unsigned long sendTime;
};

// Structure détaillée sur l'état d'un pod distant
struct NodeInfo {
  uint64_t id;
  int8_t rssi;
  uint8_t batteryPct;
  float voltage;
  bool isCharging;
  unsigned long lastSeen;
  bool isTarget;
};

class GameNetworkManager {
  // Callback ESP-NOW déclaré comme ami pour accéder aux membres de debug
  friend void OnDataRecv(const esp_now_recv_info *recv_info, const uint8_t *data, int len);

public:
  void init();
  void update();
  void broadcast(uint8_t type, uint32_t col);
  void sendTo(uint64_t target, uint8_t type, uint32_t col);
  bool addNode(uint64_t id, int8_t rssi = 0);
  void updateNodeTelemetry(uint64_t id, uint8_t batPct, float voltage = 0.0f, bool isCharging = false);

  uint64_t getMyId() const { return myId; }
  std::vector<uint64_t> getKnownNodes();
  std::vector<NodeInfo> getNodesInfo();

  bool isMessageSeen(uint64_t senderId, uint32_t msgId);
  void markMessageSeen(uint64_t senderId, uint32_t msgId);

  // File de réception thread-safe (utilisée par GameEngine)
  bool getNextRxMessage(struct_message &outMsg);
  void pushRxMessage(const struct_message &msg);
  void recordRxMessage(const struct_message &msg);

  // File de relais avec délai anti-collision (Jitter)
  void queueRelay(const struct_message &msg);

  // Debug : Suivi des derniers messages RX et TX
  bool hasLastRx() const { return hasRxRecord; }
  struct_message getLastRxMessage() const { return lastRxMsg; }
  unsigned long getLastRxTime() const { return lastRxTime; }

  bool hasLastTx() const { return hasTxRecord; }
  struct_message getLastTxMessage() const { return lastTxMsg; }
  unsigned long getLastTxTime() const { return lastTxTime; }

  static String formatId(uint64_t id);

private:
  uint64_t myId = 0;
  std::array<NodeInfo, MAX_NODES> knownNodes;
  size_t numKnownNodes = 0;

  uint8_t broadcastAddr[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

  // File de messages reçus prêts à être traités par le GameEngine
  std::queue<struct_message> rxQueue;

  // Liste des messages en attente d'être relayés
  std::vector<QueuedRelay> relayList;

  // Buffer circulaire pour retenir les derniers msgId vus
  struct SeenMessage {
    uint64_t senderId;
    uint32_t msgId;
  };
  static const int MAX_SEEN_MESSAGES = 64;
  SeenMessage seenMessages[MAX_SEEN_MESSAGES];
  int seenMessagesIndex = 0;
  uint32_t localMessageCounter = 0;

  // Debug : Historique des derniers messages
  struct_message lastRxMsg = {};
  unsigned long lastRxTime = 0;
  bool hasRxRecord = false;

  struct_message lastTxMsg = {};
  unsigned long lastTxTime = 0;
  bool hasTxRecord = false;

  // Section critique FreeRTOS pour la thread-safety
  portMUX_TYPE networkMux = portMUX_INITIALIZER_UNLOCKED;
};

extern GameNetworkManager GameNetwork;

#endif