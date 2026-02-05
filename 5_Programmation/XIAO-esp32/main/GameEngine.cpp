#include "GameEngine.h"

GameEngine Game;

GameEngine::GameEngine() :
  currentState(MENU),
  currentMode(0),
  menuTriggerStartTime(0),
  isMenuTriggerActive(false),
  startGameSequenceState(START_GAME_IDLE),
  startGameTimer(0)
{
  // No hardware-dependent initialization here
}

void GameEngine::init() {
  // Moved to constructor: currentState = MENU;
  randomSeed(ESP.getEfuseMac());
  DEBUG_PRINT(DEBUG_INFO, "GameEngine initialized. Current state: MENU");
}

void GameEngine::loop() {
  handleNetwork();
  handleInputs();
  processStartGameSequence(); // Call the new non-blocking sequence handler

  // Gestion des délais multiples (multitâche)
  if (!pendingSpawns.empty()) {
    unsigned long now = millis();
    // On parcourt la liste à l'envers pour pouvoir supprimer des éléments facilement
    for (int i = pendingSpawns.size() - 1; i >= 0; i--) {
      if (now >= pendingSpawns[i].activationTime) {
        DEBUG_PRINT(DEBUG_VERBOSE, "Processing pending spawn for color %X at %lu", pendingSpawns[i].color, pendingSpawns[i].activationTime);
        // Le délai est écoulé, on active !
        activateRandomPod(pendingSpawns[i].color);

        // On retire cet événement de la liste
        pendingSpawns.erase(pendingSpawns.begin() + i);
        DEBUG_PRINT(DEBUG_VERBOSE, "Pending spawn activated, removing from list.");
      }
    }
  }
}

void GameEngine::handleNetwork() {
  if (!GameNetwork.messageReceived) return;
  GameNetwork.messageReceived = false;
  struct_message msg = GameNetwork.lastMessage;

  DEBUG_PRINT(DEBUG_VERBOSE, "Handling network message, type: %d", msg.msgType);

  switch (msg.msgType) {
    case CMD_START:
      currentState = IDLE_LISTENER;
      Hardware.setLight(msg.color);
      DEBUG_PRINT(DEBUG_INFO, "CMD_START received. Setting state to IDLE_LISTENER and color %X.", msg.color);
      break;

    case CMD_ACTIVATE:
      DEBUG_PRINT(DEBUG_INFO, "CMD_ACTIVATE received for target ID %llu. My ID: %llu", msg.targetID, GameNetwork.getMyId());
      // Si on me demande de m'allumer
      if (currentState == IDLE_LISTENER && msg.targetID == GameNetwork.getMyId()) {
        currentState = ACTIVE_TARGET;
        Hardware.setLight(msg.color);
        GameNetwork.broadcast(CMD_ACK, 0);
        DEBUG_PRINT(DEBUG_INFO, "Activated as TARGET with color %X", msg.color);
      }
      break;

    case CMD_HIT:
      DEBUG_PRINT(DEBUG_INFO, "CMD_HIT received from sender ID %llu.", msg.senderID);
      if (currentState == CONTROLLER) {
        onHitReceived(msg.senderID);
      }
      break;

    case CMD_PING:
      DEBUG_PRINT(DEBUG_VERBOSE, "CMD_PING received. Sending ACK.");
      GameNetwork.broadcast(CMD_ACK, 0);
      break;

    case CMD_RETURN_TO_MENU:
      DEBUG_PRINT(DEBUG_INFO, "CMD_RETURN_TO_MENU received. Transitioning to MENU state.");
      currentState = MENU;
      Hardware.setLight(0); // Ensure all LEDs are off
      activeTargets.clear();
      pendingSpawns.clear();
      break;
  }
}

void GameEngine::handleInputs() {
  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd == 's') {
      startGame();
      DEBUG_PRINT(DEBUG_INFO, "Serial command 's' received. Starting game.");
    }
    if (cmd == 'h') {
      Hardware.setTriggerHit(true);
      DEBUG_PRINT(DEBUG_INFO, "Serial command 'h' received. Triggering hit.");
    }
    if (cmd >= '0' && cmd <= '3') {
      if (currentState == MENU) { // Only allow mode change in MENU state
        currentMode = cmd - '0';
        DEBUG_PRINT(DEBUG_INFO, "Serial command '%c' received. Setting mode to %d.", cmd, currentMode);
      } else {
        DEBUG_PRINT(DEBUG_WARNING, "Attempted to change mode via serial command while game is active (state: %d). Ignored.", currentState);
      }
    }
    if (cmd == 'r') {
      returnToMenu();
      DEBUG_PRINT(DEBUG_INFO, "Serial command 'r' received. Returning to menu.");
    }
  }

  if (currentState == MENU) {
    if (Hardware.getTriggerMode() != -1) {
      currentMode = Hardware.getTriggerMode();
      DEBUG_PRINT(DEBUG_INFO, "Hardware trigger mode %d detected. Setting light to color %#lx.", currentMode, (unsigned long)getGameMode(currentMode).color1);
      Hardware.setLight(getGameMode(currentMode).color1);
    }
    if (Hardware.getTriggerStart()) {
      startGame();
      DEBUG_PRINT(DEBUG_INFO, "Hardware trigger START detected.");
    }
  } else if (currentState == ACTIVE_TARGET || currentState == CONTROLLER_TARGET) {
    if (Hardware.getTriggerHit()) {
      DEBUG_PRINT(DEBUG_INFO, "Hardware HIT trigger detected.");
      Hardware.setLight(0);

      if (currentState == CONTROLLER_TARGET) {
        currentState = CONTROLLER;
        onHitReceived(GameNetwork.getMyId());  // Je suis le chef et j'ai touché
        DEBUG_PRINT(DEBUG_INFO, "Controller is target, self-hit confirmed.");
      } else {
        currentState = IDLE_LISTENER;
        GameNetwork.broadcast(CMD_HIT, 0);  // Je suis esclave
        DEBUG_PRINT(DEBUG_INFO, "Slave hit confirmed. Broadcasting CMD_HIT.");
      }
    }

    // --- Gestion du retour au menu par appui long ---
    if (Hardware.getTriggerStart() && (currentState == CONTROLLER || currentState == CONTROLLER_TARGET || currentState == IDLE_LISTENER || currentState == ACTIVE_TARGET)) {
      if (!isMenuTriggerActive) {
        isMenuTriggerActive = true;
        menuTriggerStartTime = millis();
        DEBUG_PRINT(DEBUG_INFO, "Menu return trigger initiated.");
      } else if (millis() - menuTriggerStartTime >= 3000) {
        DEBUG_PRINT(DEBUG_INFO, "Menu return trigger held for 3 seconds.");
        if (currentState == CONTROLLER || currentState == CONTROLLER_TARGET) {
          returnToMenu(); // Controller takes action directly
          DEBUG_PRINT(DEBUG_INFO, "Controller returning to menu.");
        } else {
          // Slave sends request to controller
          GameNetwork.broadcast(CMD_RETURN_TO_MENU, 0);
          DEBUG_PRINT(DEBUG_INFO, "Slave requesting return to menu from controller.");
        }
        isMenuTriggerActive = false; // Reset after trigger
      }
    } else {
      isMenuTriggerActive = false; // Reset if sensor not covered
    }
  }
}

void GameEngine::startGame() {
  Hardware.setLight(0);
  currentState = CONTROLLER;
  
  // Nettoyage
  activeTargets.clear();
  pendingSpawns.clear();
  DEBUG_PRINT(DEBUG_VERBOSE, "Active targets and pending spawns cleared.");

  // Start the non-blocking sequence
  startGameSequenceState = START_GAME_WAIT_COLOR1_BROADCAST;
  startGameTimer = millis(); // Set timer for first step
  DEBUG_PRINT(DEBUG_INFO, "Starting new game non-blocking sequence. Setting state to CONTROLLER.");
}

void GameEngine::processStartGameSequence() {
  unsigned long now = millis();
  switch (startGameSequenceState) {
    case START_GAME_IDLE:
      // Do nothing
      break;

    case START_GAME_WAIT_COLOR1_BROADCAST:
      // Broadcast CMD_START immediately, then wait for 500ms
      GameNetwork.broadcast(CMD_START, getGameMode(currentMode).color1);
      DEBUG_PRINT(DEBUG_VERBOSE, "Broadcasting CMD_START for color 1: %X.", modes[currentMode].color1);
      scheduleNextTurn(getGameMode(currentMode).color1); // Schedule the actual game turn
      
      startGameSequenceState = START_GAME_WAIT_COLOR2_BROADCAST; // Move to next step
      startGameTimer = now + 500; // Set timer for 500ms delay after broadcast
      break;

    case START_GAME_WAIT_COLOR2_BROADCAST:
      if (now >= startGameTimer) { // Wait 500ms after color1 broadcast
        if (getGameMode(currentMode).color2 != 0) {
          GameNetwork.broadcast(CMD_START, getGameMode(currentMode).color2);
          DEBUG_PRINT(DEBUG_VERBOSE, "Broadcasting CMD_START for color 2: %X.", modes[currentMode].color2);
          scheduleNextTurn(getGameMode(currentMode).color2); // Schedule the actual game turn
          startGameSequenceState = START_GAME_IDLE; // Sequence complete
        } else {
          startGameSequenceState = START_GAME_IDLE; // No color2, sequence complete
        }
      }
      break;
  }
}


void GameEngine::returnToMenu() {
  DEBUG_PRINT(DEBUG_INFO, "Returning to MENU state.");
  currentState = MENU;
  Hardware.setLight(0); // Turn off all LEDs
  activeTargets.clear();
  pendingSpawns.clear();
  GameNetwork.broadcast(CMD_RETURN_TO_MENU, 0); // Broadcast to all
  startGameSequenceState = START_GAME_IDLE; // Ensure sequence is reset
}


void GameEngine::scheduleNextTurn(uint32_t color) {
  ScheduledSpawn spawn;
  spawn.color = color;
  spawn.activationTime = millis();  // Par défaut : tout de suite

  // Ajout du délai si nécessaire (Mode 2 et 3)
  if (getGameMode(currentMode).delayMax > 0) {
    uint32_t delayMs = random(getGameMode(currentMode).delayMin, getGameMode(currentMode).delayMax) * 1000;
    spawn.activationTime += delayMs;
    DEBUG_PRINT(DEBUG_INFO, "Scheduling next turn for color %X. Delay: %lu ms.", color, delayMs);
  } else {
    DEBUG_PRINT(DEBUG_VERBOSE, "No delay for color %X, activating immediately.", color);
  }

  pendingSpawns.push_back(spawn);
}

void GameEngine::activateRandomPod(uint32_t color) {
  DEBUG_PRINT(DEBUG_INFO, "Activating random pod with color %X.", color);
  // Liste des candidats potentiels (tous les noeuds connus + moi)
  std::vector<uint64_t> candidates = GameNetwork.getKnownNodes();
  candidates.push_back(GameNetwork.getMyId());

  // Filtrer les candidats : retirer ceux qui sont déjà allumés
  // (pour éviter d'écraser une couleur existante)
  for (int i = candidates.size() - 1; i >= 0; i--) {
    for (auto active : activeTargets) {
      if (candidates[i] == active.first) {
        candidates.erase(candidates.begin() + i);
        break;
      }
    }
  }

  if (candidates.empty()) {
    DEBUG_PRINT(DEBUG_WARNING, "No free pods to activate. Retrying in 100ms for color %X.", color);
    // On remet dans la file d'attente pour réessayer très vite
    ScheduledSpawn retry;
    retry.color = color;
    retry.activationTime = millis() + 100;
    pendingSpawns.push_back(retry);
    return;
  }

  // Choix aléatoire parmi les LIBRES
  int idx = random(candidates.size());
  uint64_t chosenID = candidates[idx];
  DEBUG_PRINT(DEBUG_INFO, "Chosen pod ID: %llu for color %X.", chosenID, color);

  // Activation
  if (chosenID == GameNetwork.getMyId()) {
    currentState = CONTROLLER_TARGET;
    Hardware.setLight(color);
    DEBUG_PRINT(DEBUG_INFO, "Activating self as CONTROLLER_TARGET with color %X.", color);
  } else {
    GameNetwork.sendTo(chosenID, CMD_ACTIVATE, color);
    DEBUG_PRINT(DEBUG_INFO, "Sending CMD_ACTIVATE to %llu with color %X.", chosenID, color);
  }

  // Enregistrement
  activeTargets.push_back({ chosenID, color });
  DEBUG_PRINT(DEBUG_VERBOSE, "Pod %llu added to active targets.", chosenID);
}

void GameEngine::onHitReceived(uint64_t senderId) {
  DEBUG_PRINT(DEBUG_INFO, "Hit received from sender ID: %llu.", senderId);
  uint32_t hitColor = 0;

  // Retrouver la couleur du pod qui vient d'être touché
  for (auto it = activeTargets.begin(); it != activeTargets.end(); ++it) {
    if (it->first == senderId) {
      hitColor = it->second;
      activeTargets.erase(it);
      break;
    }
  }

  if (hitColor != 0) {
    DEBUG_PRINT(DEBUG_INFO, "Hit confirmed for color %X. Rescheduling same color.", hitColor);
    // Règle demandée : la prochaine est de la MEME couleur
    scheduleNextTurn(hitColor);
  } else {
    DEBUG_PRINT(DEBUG_WARNING, "Hit received from unknown active target %llu. Rescheduling main color %X.", senderId, getGameMode(currentMode).color1);
    // Cas rare : un message Hit reçu d'un pod qu'on ne pensait pas actif (lag ?)
    // Dans le doute, on relance la couleur principale
    scheduleNextTurn(getGameMode(currentMode).color1);
  }
}
