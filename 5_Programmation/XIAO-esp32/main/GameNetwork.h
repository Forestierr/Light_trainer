#ifndef GAMENETWORK_H
#define GAMENETWORK_H

#include "Config.h"
#include <WiFi.h>
#include <esp_now.h>
#include <array> // Use std::array
#include <vector> // Add this include

class GameNetworkManager {
public:
  void init();
  void broadcast(uint8_t type, uint32_t col);
  void sendTo(uint64_t target, uint8_t type, uint32_t col);
  bool addNode(uint64_t id); // Returns true if node was added, false otherwise

  uint64_t getMyId() const { return myId; } // Public getter for myId
  std::vector<uint64_t> getKnownNodes() const; // New public getter

  // Buffer pour le message reçu (accessible par le GameEngine)
  struct_message lastMessage;
  bool messageReceived = false;

private:
  uint64_t myId; // Made private
  std::array<uint64_t, MAX_NODES> knownNodes; // Replaced std::vector with std::array
  size_t numKnownNodes = 0; // To track current number of known nodes

  uint8_t broadcastAddr[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
};

extern GameNetworkManager GameNetwork;

#endif