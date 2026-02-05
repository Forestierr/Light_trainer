#ifndef GAMENETWORK_H
#define GAMENETWORK_H

#include "Config.h"
#include <WiFi.h>
#include <esp_now.h>
#include <vector>

class GameNetworkManager {
public:
  void init();
  void broadcast(uint8_t type, uint32_t col);
  void sendTo(uint64_t target, uint8_t type, uint32_t col);
  void addNode(uint64_t id);

  uint64_t myId;
  // Utilisation d'un vecteur ici, mais on surveille la taille
  std::vector<uint64_t> knownNodes;

  // Buffer pour le message reçu (accessible par le GameEngine)
  struct_message lastMessage;
  bool messageReceived = false;

private:
  uint8_t broadcastAddr[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
};

extern GameNetworkManager GameNetwork;

#endif