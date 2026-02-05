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

// Enum for non-blocking startGame sequence
enum StartGameSequenceState {
  START_GAME_IDLE,
  START_GAME_WAIT_COLOR1_BROADCAST,
  START_GAME_WAIT_COLOR2_BROADCAST
};

class GameEngine {
public:
  GameEngine(); // Constructor declaration
  void init();
  void loop();

  // Public getter for game modes
  const GameMode& getGameMode(int modeIndex) const {
    // Add bounds checking if modeIndex can be out of range
    if (modeIndex >= 0 && modeIndex < 4) {
      return modes[modeIndex];
    }
    // Return a default/error mode or throw an exception if index is invalid
    // For now, return modes[0] as a fallback.
    return modes[0]; 
  }

  // New public getter for the entire modes array
  const GameMode (&getAllGameModes() const)[4] {
    return modes;
  }

private:
  GameMode modes[4] = { // Made private
    { 0x00FF00, 0x000000, 0, 0 },   // Mode 0: Vert (1 couleur)
    { 0xFF0000, 0x00FF00, 0, 0 },   // Mode 1: Rouge/Vert (2 couleurs, sans délai)
    { 0x0000FF, 0x000000, 0, 10 },  // Mode 2: Bleu (1 couleur, délai)
    { 0xFFFF00, 0xFF00FF, 0, 10 }   // Mode 3: Jaune/Magenta (2 couleurs, délai)
  };

private:
  State currentState = MENU;
  int currentMode = 0;

  // Menu return trigger variables
  unsigned long menuTriggerStartTime = 0;
  bool isMenuTriggerActive = false;

  // Non-blocking startGame sequence variables
  StartGameSequenceState startGameSequenceState = START_GAME_IDLE;
  unsigned long startGameTimer = 0;

  // Liste des cibles actuellement allumées {ID, Couleur}
  std::vector<std::pair<uint64_t, uint32_t>> activeTargets;

  // Liste des cibles en attente d'allumage (gestion des délais indépendants)
  std::vector<ScheduledSpawn> pendingSpawns;

  // Helpers
  void handleInputs();
  void handleNetwork();
  void startGame();
  void returnToMenu();
  void processStartGameSequence(); // New helper to handle the non-blocking sequence

  // Planifie le prochain tour pour une couleur spécifique
  void scheduleNextTurn(uint32_t color);

  // Tente d'allumer un pod libre avec la couleur donnée
  void activateRandomPod(uint32_t color);

  // Gestion du hit
  void onHitReceived(uint64_t senderId);
};

extern GameEngine Game;

#endif