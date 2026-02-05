#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include "Config.h"
#include "GameNetwork.h"
#include "Hardware.h"
#include <vector>

// Structure pour gérer les réapparitions futures
struct ScheduledSpawn {
  unsigned long activationTime;  // Heure à laquelle allumer (millis)
  uint32_t color;                // Couleur à allumer
};

class GameEngine {
public:
  void init();
  void loop();

  GameMode modes[4] = {
    { 0x00FF00, 0x000000, 0, 0 },   // Mode 0: Vert (1 couleur)
    { 0xFF0000, 0x00FF00, 0, 0 },   // Mode 1: Rouge/Vert (2 couleurs, sans délai)
    { 0x0000FF, 0x000000, 0, 10 },  // Mode 2: Bleu (1 couleur, délai)
    { 0xFFFF00, 0xFF00FF, 0, 10 }   // Mode 3: Jaune/Magenta (2 couleurs, délai)
  };

private:
  State currentState = MENU;
  int currentMode = 0;

  // Liste des cibles actuellement allumées {ID, Couleur}
  std::vector<std::pair<uint64_t, uint32_t>> activeTargets;

  // Liste des cibles en attente d'allumage (gestion des délais indépendants)
  std::vector<ScheduledSpawn> pendingSpawns;

  // Helpers
  void handleInputs();
  void handleNetwork();
  void startGame();

  // Planifie le prochain tour pour une couleur spécifique
  void scheduleNextTurn(uint32_t color);

  // Tente d'allumer un pod libre avec la couleur donnée
  void activateRandomPod(uint32_t color);

  // Gestion du hit
  void onHitReceived(uint64_t senderId);
};

extern GameEngine Game;

#endif