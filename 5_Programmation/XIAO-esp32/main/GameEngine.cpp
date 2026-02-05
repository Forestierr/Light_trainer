#include "GameEngine.h"

GameEngine Game;

void GameEngine::init() {
  currentState = MENU;
  randomSeed(ESP.getEfuseMac());
  DEBUG_PRINT(DEBUG_INFO, "GameEngine initialized. Current state: MENU");
}

void GameEngine::loop() {
  handleNetwork();
  handleInputs();

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
      Hardware.setLight(0);
      DEBUG_PRINT(DEBUG_INFO, "CMD_START received. Setting state to IDLE_LISTENER.");
      break;

    case CMD_ACTIVATE:
      DEBUG_PRINT(DEBUG_INFO, "CMD_ACTIVATE received for target ID %llu. My ID: %llu", msg.targetID, GameNetwork.myId);
      // Si on me demande de m'allumer
      if (currentState == IDLE_LISTENER && msg.targetID == GameNetwork.myId) {
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
      Hardware.triggerHit = true;
      DEBUG_PRINT(DEBUG_INFO, "Serial command 'h' received. Triggering hit.");
    }
    if (cmd >= '0' && cmd <= '3') {
      currentMode = cmd - '0';
      DEBUG_PRINT(DEBUG_INFO, "Serial command '%c' received. Setting mode to %d.", cmd, currentMode);
    }
  }

  if (currentState == MENU) {
    if (Hardware.triggerMode != -1) {
      currentMode = Hardware.triggerMode;
      DEBUG_PRINT(DEBUG_INFO, "Hardware trigger mode %d detected. Passing color %lx to blink.", currentMode, (unsigned long)modes[currentMode].color1);
      Hardware.blink(modes[currentMode].color1, 1);
    }
    if (Hardware.triggerStart) {
      startGame();
      DEBUG_PRINT(DEBUG_INFO, "Hardware trigger START detected.");
    }
  } else if (currentState == ACTIVE_TARGET || currentState == CONTROLLER_TARGET) {
    if (Hardware.triggerHit) {
      DEBUG_PRINT(DEBUG_INFO, "Hardware HIT trigger detected.");
      Hardware.setLight(0);

      if (currentState == CONTROLLER_TARGET) {
        currentState = CONTROLLER;
        onHitReceived(GameNetwork.myId);  // Je suis le chef et j'ai touché
        DEBUG_PRINT(DEBUG_INFO, "Controller is target, self-hit confirmed.");
      } else {
        currentState = IDLE_LISTENER;
        GameNetwork.broadcast(CMD_HIT, 0);  // Je suis esclave
        DEBUG_PRINT(DEBUG_INFO, "Slave hit confirmed. Broadcasting CMD_HIT.");
      }
    }
  }
}

void GameEngine::startGame() {
  currentState = CONTROLLER;
  GameNetwork.broadcast(CMD_START, 0);
  delay(500);
  DEBUG_PRINT(DEBUG_INFO, "Starting new game. Setting state to CONTROLLER.");

  // Nettoyage
  activeTargets.clear();
  pendingSpawns.clear();
  DEBUG_PRINT(DEBUG_VERBOSE, "Active targets and pending spawns cleared.");

  // Lancement des boucles de couleur
  // 1. Toujours lancer la couleur 1
  scheduleNextTurn(modes[currentMode].color1);
  DEBUG_PRINT(DEBUG_VERBOSE, "Scheduling initial turn for color %X.", modes[currentMode].color1);

  // 2. Si le mode a une 2ème couleur (Mode 1 ou 3), on lance sa boucle aussi
  if (modes[currentMode].color2 != 0) {
    // Petit décalage pour éviter que les deux s'allument exactement en même temps au start
    delay(100);
    scheduleNextTurn(modes[currentMode].color2);
    DEBUG_PRINT(DEBUG_VERBOSE, "Scheduling second initial turn for color %X.", modes[currentMode].color2);
  }
}

void GameEngine::scheduleNextTurn(uint32_t color) {
  ScheduledSpawn spawn;
  spawn.color = color;
  spawn.activationTime = millis();  // Par défaut : tout de suite

  // Ajout du délai si nécessaire (Mode 2 et 3)
  if (modes[currentMode].delayMax > 0) {
    uint32_t delayMs = random(modes[currentMode].delayMin, modes[currentMode].delayMax) * 1000;
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
  std::vector<uint64_t> candidates = GameNetwork.knownNodes;
  candidates.push_back(GameNetwork.myId);

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
  if (chosenID == GameNetwork.myId) {
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
    DEBUG_PRINT(DEBUG_WARNING, "Hit received from unknown active target %llu. Rescheduling main color %X.", senderId, modes[currentMode].color1);
    // Cas rare : un message Hit reçu d'un pod qu'on ne pensait pas actif (lag ?)
    // Dans le doute, on relance la couleur principale
    scheduleNextTurn(modes[currentMode].color1);
  }
}