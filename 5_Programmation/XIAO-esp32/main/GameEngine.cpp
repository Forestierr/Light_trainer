#include "GameEngine.h"

GameEngine Game;

GameEngine::GameEngine() :
  currentState(MENU),
  currentMode(0),
  gameRunning(false),
  gamePaused(false),
  isManualMode(false),
  sessionStartTime(0),
  pauseStartTime(0),
  totalPausedDuration(0),
  sessionTotalHits(0),
  lastReactionTime(0),
  bestReactionTime(0),
  sumReactionTimes(0),
  startGameSequenceState(START_GAME_IDLE),
  startGameTimer(0)
{
}

void GameEngine::init() {
  randomSeed(ESP.getEfuseMac());
  currentState = MENU;
  gameRunning = false;
  gamePaused = false;
  isManualMode = false;
  Hardware.setLight(0); // Démarrage avec les LEDs éteintes
  addLog("GameEngine initialisé en mode MENU", "INFO");
}

void GameEngine::addLog(const String &msg, const String &level) {
  LogEntry entry;
  entry.timestampMs = millis();
  entry.level = level;
  entry.message = msg;

  systemLogs.push_back(entry);
  if (systemLogs.size() > 50) {
    systemLogs.erase(systemLogs.begin());
  }

  Serial.printf("[%s] %s\n", level.c_str(), msg.c_str());
}

void GameEngine::clearLogs() {
  systemLogs.clear();
  addLog("Journal de logs effacé", "INFO");
}

void GameEngine::clearHitHistory() {
  hitHistory.clear();
  sessionTotalHits = 0;
  lastReactionTime = 0;
  bestReactionTime = 0;
  sumReactionTimes = 0;
  totalPausedDuration = 0;
  addLog("Historique de session réinitialisé", "INFO");
}

void GameEngine::addPingResult(uint64_t id, int8_t rssi, uint16_t rttMs, uint8_t batPct) {
  for (auto &r : lastPingResults) {
    if (r.podId == id) {
      r.rttMs = rttMs;
      r.rssi = rssi;
      r.batteryPct = batPct;
      return;
    }
  }
  PingPodResult res;
  res.podId = id;
  res.rssi = rssi;
  res.rttMs = rttMs;
  res.batteryPct = batPct;
  lastPingResults.push_back(res);
}

uint32_t GameEngine::getElapsedTimeMs() const {
  if (!gameRunning) return 0;
  if (gamePaused) return (pauseStartTime - sessionStartTime) - totalPausedDuration;
  return (millis() - sessionStartTime) - totalPausedDuration;
}

float GameEngine::getAvgReactionTimeMs() const {
  if (sessionTotalHits == 0) return 0.0f;
  return (float)sumReactionTimes / (float)sessionTotalHits;
}

float GameEngine::getCadenceBpm() const {
  uint32_t elapsed = getElapsedTimeMs();
  if (elapsed < 2000 || sessionTotalHits == 0) return 0.0f;
  return ((float)sessionTotalHits / (float)elapsed) * 60000.0f;
}

void GameEngine::setMode(int mode) {
  if (mode >= 0 && mode < 5) {
    currentMode = mode;
    // Si une partie ou un mode manuel était en cours, on l'arrête proprement
    if (gameRunning || isManualMode || currentState != MENU) {
      returnToMenu();
    }
    // Affichage bref de la couleur du mode sélectionné
    Hardware.setLight(getGameMode(currentMode).color1);
    addLog("Mode de jeu défini : Mode " + String(mode) + " (" + getModeDescription(mode) + ")", "INFO");
  }
}

void GameEngine::startGame() {
  // Réinitialisation forcée préalable si un jeu était déjà actif
  if (currentState != MENU || gameRunning || isManualMode) {
    returnToMenu();
    delay(30);
  }

  if (currentMode == 4) {
    startSimonGame();
    return;
  }

  addLog("Démarrage d'une nouvelle séance de jeu...", "GAME");
  currentState = CONTROLLER;
  gameRunning = true;
  gamePaused = false;
  isManualMode = false;
  sessionStartTime = millis();
  totalPausedDuration = 0;
  sessionTotalHits = 0;
  lastReactionTime = 0;
  bestReactionTime = 0;
  sumReactionTimes = 0;
  c1Hits = 0;
  c1BestRt = 0;
  c1SumRt = 0;
  c2Hits = 0;
  c2BestRt = 0;
  c2SumRt = 0;
  hitHistory.clear();

  activeTargets.clear();
  pendingSpawns.clear();

  // Éteindre les lumières du Master
  Hardware.setLight(0);

  // Démarrer la séquence de diffusion non-bloquante
  startGameSequenceState = START_GAME_WAIT_COLOR1_BROADCAST;
  startGameTimer = millis();
}

void GameEngine::pauseGame() {
  if (gameRunning && !gamePaused) {
    gamePaused = true;
    pauseStartTime = millis();
    addLog("Séance mise en PAUSE", "GAME");
  }
}

void GameEngine::resumeGame() {
  if (gameRunning && gamePaused) {
    uint32_t pausedDelta = millis() - pauseStartTime;
    totalPausedDuration += pausedDelta;

    // Décaler les timestamps des cibles actives pour ne pas provoquer de timeout prématuré
    for (auto& target : activeTargets) {
      target.activationTime += pausedDelta;
    }
    for (auto& spawn : pendingSpawns) {
      spawn.activationTime += pausedDelta;
    }

    gamePaused = false;
    addLog("Séance REPRISE", "GAME");
  }
}

void GameEngine::stopGame() {
  addLog("Séance ARRÊTÉE. Retour au MENU.", "GAME");
  returnToMenu();
}

void GameEngine::returnToMenu() {
  currentState = MENU;
  gameRunning = false;
  gamePaused = false;
  isManualMode = false;

  Hardware.setLight(0); // Éteindre toutes les LEDs
  activeTargets.clear();
  pendingSpawns.clear();
  startGameSequenceState = START_GAME_IDLE;
  simonState = SIMON_STATE_IDLE;
  simonSequence.clear();

  // Propager l'ordre d'extinction et de retour au menu à tout le réseau
  GameNetwork.broadcast(CMD_RETURN_TO_MENU, 0);
  addLog("Retour au mode MENU (Tous les pods éteints)", "INFO");
}

void GameEngine::setManualPod(uint64_t podId, uint32_t color) {
  isManualMode = true;
  gameRunning = false;
  gamePaused = false;

  if (podId == GameNetwork.getMyId()) {
    if (color == 0) {
      Hardware.setLight(0);
      currentState = MENU;
    } else {
      Hardware.setLight(color);
      currentState = ACTIVE_TARGET;
    }
  } else {
    // Ordre adressé à un pod distant
    GameNetwork.sendTo(podId, CMD_MANUAL_SET, color);
  }

  addLog("Mode Manuel : Pod " + GameNetwork.formatId(podId) + " -> Couleur #" + String(color, HEX), "MANUAL");
}

void GameEngine::setAllPodsColor(uint32_t color) {
  isManualMode = true;
  gameRunning = false;
  gamePaused = false;

  Hardware.setLight(color);
  GameNetwork.broadcast(CMD_MANUAL_SET, color);
  addLog("Mode Manuel : Tous les pods -> Couleur #" + String(color, HEX), "MANUAL");
}

void GameEngine::turnOffAllPods() {
  Hardware.setLight(0);
  GameNetwork.broadcast(CMD_MANUAL_SET, 0);
  isManualMode = false;
  addLog("Mode Manuel : Tous les pods éteints", "MANUAL");
}

void GameEngine::loop() {
  handleNetwork();
  handleInputs();

  // En pause ou en mode manuel, on ne gère pas les délais et timeouts automatiques
  if (gamePaused || isManualMode || !gameRunning) return;

  // Si Mode Simon actif
  if (currentMode == 4) {
    processSimonLoop();
    return;
  }

  processStartGameSequence();

  // 1. Gestion des délais programmés (Spawns futurs pour Modes 0-3)
  if (!pendingSpawns.empty()) {
    unsigned long now = millis();
    for (int i = pendingSpawns.size() - 1; i >= 0; i--) {
      if (now >= pendingSpawns[i].activationTime) {
        uint32_t col = pendingSpawns[i].color;
        pendingSpawns.erase(pendingSpawns.begin() + i);
        activateRandomPod(col);
        break; // Traiter un seul spawn par tour de loop pour éviter les collisions radio ESP-NOW
      }
    }
  }

  // 2. Timeout des cibles actives (15s sans touche -> replanification)
  if (currentState == CONTROLLER || currentState == CONTROLLER_TARGET) {
    unsigned long now = millis();
    for (auto it = activeTargets.begin(); it != activeTargets.end(); ) {
      if (now - it->activationTime > 15000) {
        String err = "Timeout sur cible " + GameNetwork.formatId(it->id);
        logError(err);
        scheduleNextTurn(it->color);
        it = activeTargets.erase(it);
      } else {
        ++it;
      }
    }
  }
}

void GameEngine::handleNetwork() {
  struct_message msg;
  while (GameNetwork.getNextRxMessage(msg)) {
    switch (msg.msgType) {
      case CMD_START:
        // Sur réception de START, le pod esclave devient récepteur et synchronise son mode
        currentState = IDLE_LISTENER;
        currentMode = msg.color;
        gameRunning = true;
        gamePaused = false;
        Hardware.setLight(0);
        addLog("CMD_START reçu d'un Master (Mode " + String(currentMode) + ")", "RADIO");
        break;

      case CMD_ACTIVATE:
        // Allumer UNIQUEMENT si ce pod est la cible désignée ou en broadcast
        if (msg.targetID == GameNetwork.getMyId() || msg.targetID == 0) {
          if (msg.color == 0) {
            Hardware.setLight(0);
            currentState = (currentState == CONTROLLER || currentState == CONTROLLER_TARGET) ? CONTROLLER : IDLE_LISTENER;
          } else {
            currentState = (currentState == CONTROLLER || currentState == CONTROLLER_TARGET) ? CONTROLLER_TARGET : ACTIVE_TARGET;
            Hardware.setLight(msg.color);
            addLog("Je suis activé comme CIBLE (Couleur #" + String(msg.color, HEX) + ")", "GAME");
          }
        }
        break;

      case CMD_HIT:
        if ((currentState == CONTROLLER || currentState == CONTROLLER_TARGET) && gameRunning && !gamePaused) {
          onHitReceived(msg.senderID);
        }
        break;

      case CMD_MANUAL_SET:
        if (msg.targetID == 0 || msg.targetID == GameNetwork.getMyId()) {
          if (msg.color == 0) {
            Hardware.setLight(0);
            currentState = (currentState == CONTROLLER) ? CONTROLLER : IDLE_LISTENER;
          } else {
            Hardware.setLight(msg.color);
            currentState = ACTIVE_TARGET;
          }
          addLog("CMD_MANUAL_SET reçu (Couleur #" + String(msg.color, HEX) + ")", "MANUAL");
        }
        break;

      case CMD_TEST_LEDS:
        Hardware.rainbowBlink(2, 80);
        addLog("CMD_TEST_LEDS exécuté", "TEST");
        break;

      case CMD_PING:
        // Si ping actif (> 0), répondre avec un CMD_ACK incluant la batterie complète
        if (msg.color > 0) {
          uint8_t bat = 85;
          float volt = 3.9f;
          bool chg = false;
#if ENABLE_BATTERY_MANAGEMENT
          bat = (uint8_t)Hardware.getBatteryPercentage();
          volt = Hardware.getBatteryVoltage();
          chg = Hardware.isCharging();
#endif
          uint16_t vCentivolts = (uint16_t)(volt * 100.0f);
          uint32_t telem = (bat & 0xFF) | ((vCentivolts & 0xFFFF) << 8) | ((chg ? 1 : 0) << 24);
          GameNetwork.sendTo(msg.senderID, CMD_ACK, telem);
          addLog("CMD_PING reçu de " + GameNetwork.formatId(msg.senderID) + " -> ACK envoyé", "RADIO");
        }
        break;

      case CMD_RETURN_TO_MENU:
        currentState = MENU;
        gameRunning = false;
        gamePaused = false;
        isManualMode = false;
        Hardware.setLight(0);
        activeTargets.clear();
        pendingSpawns.clear();
        startGameSequenceState = START_GAME_IDLE;
        simonState = SIMON_STATE_IDLE;
        simonSequence.clear();
        addLog("CMD_RETURN_TO_MENU reçu (Tous les pods éteints)", "RADIO");
        break;

      case CMD_ACK:
        if (pingInProgress) {
          uint16_t rtt = (uint16_t)(millis() - pingStartTime);
          int8_t rssi = 0;
          for (const auto& node : GameNetwork.getNodesInfo()) {
            if (node.id == msg.senderID) {
              rssi = node.rssi;
              break;
            }
          }
          uint8_t batPct = (uint8_t)(msg.color & 0xFF);
          float volt = ((msg.color >> 8) & 0xFFFF) / 100.0f;
          bool chg = (msg.color >> 24) & 0x01;
          addPingResult(msg.senderID, rssi, rtt, batPct);
          GameNetwork.updateNodeTelemetry(msg.senderID, batPct, volt, chg);
          addLog("Ping réponse de " + GameNetwork.formatId(msg.senderID) + " : " + String(rtt) + "ms (" + String(rssi) + " dBm, Bat: " + String(batPct) + "%, " + String(volt, 2) + "V)", "TEST");
        }
        break;

      case CMD_POD_TELEMETRY: {
        uint8_t pct = msg.color & 0xFF;
        float volt = ((msg.color >> 8) & 0xFFFF) / 100.0f;
        bool chg = (msg.color >> 24) & 0x01;
        GameNetwork.updateNodeTelemetry(msg.senderID, pct, volt, chg);
        break;
      }
    }
  }
}

void GameEngine::handleInputs() {
  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd == 's') startGame();
    if (cmd == 'h') Hardware.setTriggerHit(true);
    if (cmd >= '0' && cmd <= '4') {
      if (currentState == MENU) setMode(cmd - '0');
    }
    if (cmd == 'r') returnToMenu();
    if (cmd == 'd' || cmd == 'D') printDashboard();
  }

  // Contrôles gestuels ToF
  if (currentState == MENU) {
    if (Hardware.getTriggerTap()) {
      currentMode = (currentMode + 1) % 5;
      Hardware.setLight(getGameMode(currentMode).color1);
      addLog("Tap gestuel : Mode " + String(currentMode) + " (" + getModeDescription(currentMode) + ")", "GESTURE");
    }
    if (Hardware.getTriggerStartHold()) {
      addLog("Hold gestuel (1.5s) : Démarrage du jeu !", "GESTURE");
      startGame();
    }
  } else {
    // Pendant le jeu ou mode manuel
    if (Hardware.getTriggerMenuHold()) {
      addLog("Hold gestuel (3s) : Retour au Menu", "GESTURE");
      returnToMenu();
    }
  }

  // Traitement d'un Hit sur ce pod
  if (Hardware.getTriggerHit()) {
    Hardware.setTriggerHit(false);

    // Ne valider le hit que si non en pause
    if (!gamePaused) {
      if (currentMode == 4) {
        if (currentState == CONTROLLER || currentState == CONTROLLER_TARGET) {
          if (simonState == SIMON_STATE_INPUT) {
            Hardware.flashWhite();
            handleSimonHit(GameNetwork.getMyId());
          }
        } else if (gameRunning) {
          // Pod esclave en Mode Simon
          Hardware.flashWhite();
          GameNetwork.broadcast(CMD_HIT, 0);
          addLog("Hit Simon envoyé par pod esclave", "HIT");
        }
      } else if (currentState == ACTIVE_TARGET || currentState == CONTROLLER_TARGET) {
        Hardware.flashWhite();

        if (currentState == CONTROLLER_TARGET) {
          currentState = CONTROLLER;
          onHitReceived(GameNetwork.getMyId());
        } else if (currentState == ACTIVE_TARGET) {
          currentState = IDLE_LISTENER;
          Hardware.setLight(0); // Éteindre immédiatement après le flash
          GameNetwork.broadcast(CMD_HIT, 0);
          addLog("Hit validé sur ce pod esclave", "HIT");
        }
      }
    }
  }
}

void GameEngine::onHitReceived(uint64_t senderId) {
  if (currentMode == 4) {
    handleSimonHit(senderId);
    return;
  }

  auto it = std::find_if(activeTargets.begin(), activeTargets.end(), [senderId](const ActiveTarget& target) {
    return target.id == senderId;
  });

  if (it != activeTargets.end()) {
    uint32_t reactionTime = millis() - it->activationTime;
    uint32_t color = it->color;
    activeTargets.erase(it);

    sessionTotalHits++;
    lastReactionTime = reactionTime;
    sumReactionTimes += reactionTime;
    if (bestReactionTime == 0 || reactionTime < bestReactionTime) {
      bestReactionTime = reactionTime;
    }

    // Différenciation bicolore
    if (color == getGameMode(currentMode).color1) {
      c1Hits++;
      c1SumRt += reactionTime;
      if (c1BestRt == 0 || reactionTime < c1BestRt) c1BestRt = reactionTime;
    } else if (color == getGameMode(currentMode).color2) {
      c2Hits++;
      c2SumRt += reactionTime;
      if (c2BestRt == 0 || reactionTime < c2BestRt) c2BestRt = reactionTime;
    }

    HitRecord record;
    record.hitNum = sessionTotalHits;
    record.podId = senderId;
    record.color = color;
    record.reactionTimeMs = reactionTime;
    record.timestampMs = getElapsedTimeMs();
    hitHistory.push_back(record);

    char colHex[10];
    snprintf(colHex, sizeof(colHex), "#%06X", (unsigned int)color);
    addLog("Hit #" + String(sessionTotalHits) + " (" + String(colHex) + ") sur " + GameNetwork.formatId(senderId) + " en " + String(reactionTime) + "ms", "HIT");
    
    // Programmer l'apparition de la prochaine cible
    scheduleNextTurn(color);
  }
}

void GameEngine::processStartGameSequence() {
  if (startGameSequenceState == START_GAME_WAIT_COLOR1_BROADCAST) {
    if (millis() - startGameTimer >= 50) {
      Hardware.setLight(0);
      // Diffuser CMD_START avec le mode actuel pour que tous les esclaves soient prêts
      GameNetwork.broadcast(CMD_START, currentMode);

      // Activer immédiatement la première cible
      activateRandomPod(getGameMode(currentMode).color1);

      if (getGameMode(currentMode).color2 != 0x000000) {
        startGameSequenceState = START_GAME_WAIT_COLOR2_BROADCAST;
        startGameTimer = millis();
      } else {
        startGameSequenceState = START_GAME_IDLE;
      }
    }
  } else if (startGameSequenceState == START_GAME_WAIT_COLOR2_BROADCAST) {
    if (millis() - startGameTimer >= 60) {
      activateRandomPod(getGameMode(currentMode).color2);
      startGameSequenceState = START_GAME_IDLE;
    }
  }
}

void GameEngine::scheduleNextTurn(uint32_t color) {
  if (!gameRunning || gamePaused) return;

  const GameMode& currentGM = getGameMode(currentMode);
  uint32_t delayMs = 0;
  if (currentGM.delayMax > 0) {
    delayMs = random(currentGM.delayMin * 1000, (currentGM.delayMax * 1000) + 1);
  } else {
    // Mode sans délai : délai de transition de 80ms pour laisser le flash blanc s'exécuter
    delayMs = HIT_FLASH_MS + 20;
  }

  unsigned long targetTime = millis() + delayMs;
  // Éviter la collision radio si un autre spawn est déjà programmé à un temps trop proche (< 60ms)
  for (const auto& sp : pendingSpawns) {
    if (abs((long)(targetTime - sp.activationTime)) < 60) {
      targetTime = sp.activationTime + 60;
    }
  }

  ScheduledSpawn spawn;
  spawn.activationTime = targetTime;
  spawn.color = color;
  pendingSpawns.push_back(spawn);
}

void GameEngine::activateRandomPod(uint32_t color) {
  if (!gameRunning || gamePaused) return;

  std::vector<uint64_t> availableNodes;
  // Le Master lui-même est candidat si pas déjà actif
  bool masterIsActive = false;
  for (const auto& target : activeTargets) {
    if (target.id == GameNetwork.getMyId()) {
      masterIsActive = true;
      break;
    }
  }
  if (!masterIsActive) {
    availableNodes.push_back(GameNetwork.getMyId());
  }

  // Candidats esclaves
  for (uint64_t node : GameNetwork.getKnownNodes()) {
    bool isAlreadyActive = false;
    for (const auto& target : activeTargets) {
      if (target.id == node) {
        isAlreadyActive = true;
        break;
      }
    }
    if (!isAlreadyActive) {
      availableNodes.push_back(node);
    }
  }

  if (availableNodes.empty()) {
    logError("Aucun pod disponible pour activation, réessai dans 200ms");
    ScheduledSpawn retrySpawn;
    retrySpawn.activationTime = millis() + 200;
    retrySpawn.color = color;
    pendingSpawns.push_back(retrySpawn);
    return;
  }

  int randomIndex = random(0, availableNodes.size());
  uint64_t chosenNode = availableNodes[randomIndex];

  ActiveTarget target;
  target.id = chosenNode;
  target.color = color;
  target.activationTime = millis();
  activeTargets.push_back(target);

  if (chosenNode == GameNetwork.getMyId()) {
    currentState = CONTROLLER_TARGET;
    Hardware.setLight(color);
  } else {
    GameNetwork.sendTo(chosenNode, CMD_ACTIVATE, color);
  }

  addLog("Nouvelle cible activée : " + GameNetwork.formatId(chosenNode), "GAME");
}

// -----------------------------------------------------------------------------------------
// LOGIQUE DU MODE 4 : JEU SIMON (MÉMOIRE & ACCÉLÉRATION)
// -----------------------------------------------------------------------------------------
uint32_t GameEngine::getPodColorForSimon(uint64_t podId, size_t colorIndexOffset) const {
  static const uint32_t simonColors[] = {
    0x00FF00, // Vert (Master)
    0xFF0000, // Rouge (Pod 2)
    0x0000FF, // Bleu (Pod 3)
    0xFFFF00, // Jaune (Pod 4)
    0xFF00FF, // Magenta (Pod 5)
    0x00FFFF, // Cyan (Pod 6)
    0xFF9F0A  // Orange (Pod 7)
  };
  const size_t numColors = sizeof(simonColors) / sizeof(simonColors[0]);

  // Si 1 seul pod (Master seul), on varie la couleur par étape
  if (GameNetwork.getKnownNodes().empty()) {
    return simonColors[colorIndexOffset % numColors];
  }

  if (podId == GameNetwork.getMyId()) {
    return simonColors[0];
  }

  const auto& nodes = GameNetwork.getKnownNodes();
  for (size_t i = 0; i < nodes.size(); i++) {
    if (nodes[i] == podId) {
      return simonColors[(i + 1) % numColors];
    }
  }
  return simonColors[0];
}

const char* GameEngine::getSimonStateString() const {
  switch (simonState) {
    case SIMON_STATE_DEMO_LIGHT_ON:
    case SIMON_STATE_DEMO_LIGHT_OFF: return "DEMO";
    case SIMON_STATE_INPUT:          return "INPUT";
    case SIMON_STATE_SUCCESS:        return "SUCCESS";
    case SIMON_STATE_GAMEOVER:       return "GAMEOVER";
    default:                         return "IDLE";
  }
}

void GameEngine::startSimonGame() {
  addLog("--- DÉMARRAGE DU JEU SIMON (MODE 4) ---", "GAME");
  currentState = CONTROLLER;
  gameRunning = true;
  gamePaused = false;
  isManualMode = false;
  sessionStartTime = millis();
  totalPausedDuration = 0;
  sessionTotalHits = 0;
  lastReactionTime = 0;
  bestReactionTime = 0;
  sumReactionTimes = 0;
  hitHistory.clear();
  activeTargets.clear();
  pendingSpawns.clear();
  startGameSequenceState = START_GAME_IDLE;

  simonRound = 1;
  simonSequence.clear();

  // Diffuser START aux esclaves avec mode 4 pour qu'ils éteignent leurs lumières et se synchronisent
  Hardware.setLight(0);
  GameNetwork.broadcast(CMD_START, 4);

  // Vitesse de départ (défaut 800ms)
  uint32_t baseSpeed = (modes[4].delayMin > 0) ? modes[4].delayMin : 800;
  simonStepDurationMs = baseSpeed;

  startSimonNextRound();
}

void GameEngine::startSimonNextRound() {
  // Choisir un pod aléatoire pour la nouvelle étape
  std::vector<uint64_t> candidatePods;
  candidatePods.push_back(GameNetwork.getMyId());
  for (uint64_t node : GameNetwork.getKnownNodes()) {
    candidatePods.push_back(node);
  }

  uint64_t chosenPod = candidatePods[random(0, candidatePods.size())];
  uint32_t chosenColor = getPodColorForSimon(chosenPod, simonSequence.size());

  SimonStep newStep;
  newStep.podId = chosenPod;
  newStep.color = chosenColor;
  simonSequence.push_back(newStep);

  // Calcul du tempo accéléré pour cette manche
  float accelRate = (modes[4].delayMax > 0) ? (float)modes[4].delayMax / 100.0f : 0.06f;
  uint32_t baseSpeed = (modes[4].delayMin > 0) ? modes[4].delayMin : 800;
  float speed = (float)baseSpeed * pow(1.0f - accelRate, (float)(simonRound - 1));
  simonStepDurationMs = constrain((uint32_t)speed, 180, 2000);

  addLog("Simon Manche " + String(simonRound) + " : Séquence de " + String(simonSequence.size()) + " étape(s) (Tempo: " + String(simonStepDurationMs) + "ms)", "GAME");

  // Début de la phase de démonstration (Lecture de la séquence)
  simonDemoIndex = 0;
  simonState = SIMON_STATE_DEMO_LIGHT_ON;
  simonStepTimer = millis();

  // Allumer le premier pod de la séquence
  if (simonSequence[0].podId == GameNetwork.getMyId()) {
    Hardware.setLight(simonSequence[0].color);
  } else {
    GameNetwork.sendTo(simonSequence[0].podId, CMD_ACTIVATE, simonSequence[0].color);
  }
}

void GameEngine::processSimonLoop() {
  unsigned long now = millis();

  if (simonState == SIMON_STATE_DEMO_LIGHT_ON) {
    if (now - simonStepTimer >= simonStepDurationMs) {
      // Éteindre le pod en cours
      if (simonSequence[simonDemoIndex].podId == GameNetwork.getMyId()) {
        Hardware.setLight(0);
      } else {
        GameNetwork.sendTo(simonSequence[simonDemoIndex].podId, CMD_ACTIVATE, 0);
      }
      simonState = SIMON_STATE_DEMO_LIGHT_OFF;
      simonStepTimer = now;
    }
  } else if (simonState == SIMON_STATE_DEMO_LIGHT_OFF) {
    uint32_t pauseBetweenSteps = min((uint32_t)160, simonStepDurationMs / 3);
    if (now - simonStepTimer >= pauseBetweenSteps) {
      simonDemoIndex++;
      if (simonDemoIndex < simonSequence.size()) {
        // Allumer l'étape suivante
        const auto& step = simonSequence[simonDemoIndex];
        if (step.podId == GameNetwork.getMyId()) {
          Hardware.setLight(step.color);
        } else {
          GameNetwork.sendTo(step.podId, CMD_ACTIVATE, step.color);
        }
        simonState = SIMON_STATE_DEMO_LIGHT_ON;
        simonStepTimer = now;
      } else {
        // Toute la séquence a été montrée ! Phase de saisie par le joueur
        simonState = SIMON_STATE_INPUT;
        simonInputIndex = 0;
        simonStepTimer = now;
        Hardware.setLight(0);
        addLog("Simon : À votre tour ! Reproduisez la séquence.", "GAME");
      }
    }
  } else if (simonState == SIMON_STATE_INPUT) {
    // Timeout de 15s d'inactivité
    if (now - simonStepTimer > 15000) {
      addLog("Simon : Temps écoulé (Inactivité)", "GAME");
      triggerSimonGameOver();
    }
  } else if (simonState == SIMON_STATE_SUCCESS) {
    if (now - simonStepTimer >= 1000) {
      // Démarrage de la manche suivante
      startSimonNextRound();
    }
  } else if (simonState == SIMON_STATE_GAMEOVER) {
    if (now - simonStepTimer >= 3000) {
      returnToMenu();
    }
  }
}

void GameEngine::handleSimonHit(uint64_t senderId) {
  if (simonState != SIMON_STATE_INPUT || !gameRunning || gamePaused) return;

  unsigned long now = millis();
  simonStepTimer = now; // Réinitialise le timer d'inactivité

  uint64_t expectedPod = simonSequence[simonInputIndex].podId;
  uint32_t stepColor = simonSequence[simonInputIndex].color;

  if (senderId == expectedPod) {
    // TOUCHE CORRECTE !
    sessionTotalHits++;
    addLog("Simon : Étape " + String(simonInputIndex + 1) + "/" + String(simonSequence.size()) + " validée sur " + GameNetwork.formatId(senderId), "HIT");

    // Flash de confirmation visuelle sur le pod touché si Master (les esclaves flashent localement sur le Hit)
    if (senderId == GameNetwork.getMyId()) {
      Hardware.flashWhite();
    }

    simonInputIndex++;

    if (simonInputIndex >= simonSequence.size()) {
      // TOUTE LA SÉQUENCE DE LA MANCHE EST COMPLÈTE !
      triggerSimonSuccess();
    }
  } else {
    // ERREUR DE TOUCHE -> GAME OVER !
    addLog("Simon : Mauvais pod touché (" + GameNetwork.formatId(senderId) + " au lieu de " + GameNetwork.formatId(expectedPod) + ")", "ERROR");
    triggerSimonGameOver();
  }
}

void GameEngine::triggerSimonSuccess() {
  simonState = SIMON_STATE_SUCCESS;
  simonStepTimer = millis();

  if (simonRound > simonBestRound) {
    simonBestRound = simonRound;
  }

  addLog("Simon : Manche " + String(simonRound) + " RÉUSSIE ! Bravo !", "GAME");

  // Animation de succès : Flash vert sur tous les pods
  Hardware.setLight(0x00FF00);
  GameNetwork.broadcast(CMD_MANUAL_SET, 0x00FF00);
  delay(120);
  Hardware.setLight(0);
  GameNetwork.broadcast(CMD_MANUAL_SET, 0);

  simonRound++;
}

void GameEngine::triggerSimonGameOver() {
  simonState = SIMON_STATE_GAMEOVER;
  simonStepTimer = millis();

  addLog("--- SIMON GAME OVER --- Score : " + String(simonRound) + " manche(s)", "GAME");

  // Animation Game Over : Flash rouge sur tous les pods
  Hardware.setLight(0xFF0000);
  GameNetwork.broadcast(CMD_MANUAL_SET, 0xFF0000);
  delay(200);
  Hardware.setLight(0);
  GameNetwork.broadcast(CMD_MANUAL_SET, 0);
  delay(150);
  Hardware.setLight(0xFF0000);
  GameNetwork.broadcast(CMD_MANUAL_SET, 0xFF0000);
  delay(200);
  Hardware.setLight(0);
  GameNetwork.broadcast(CMD_MANUAL_SET, 0);
}

String GameEngine::generateCsvReport() {
  String csv = "Numero_Touche;Pod_ID;Couleur_RGB;Temps_Reaction_ms;Temps_Ecoule_s\n";
  for (const auto& h : hitHistory) {
    char colHex[10];
    snprintf(colHex, sizeof(colHex), "#%06X", (unsigned int)h.color);
    csv += String(h.hitNum) + ";" + GameNetwork.formatId(h.podId) + ";" + String(colHex) + ";" + String(h.reactionTimeMs) + ";" + String(h.timestampMs / 1000.0f, 2) + "\n";
  }
  return csv;
}

const char* GameEngine::getModeDescription(int index) const {
  switch (index) {
    case 0: return "Vitesse Solo (1 Couleur, sans délai)";
    case 1: return "Duel Bicolore (2 Couleurs, sans délai)";
    case 2: return "Réflexe Aléatoire (1 Couleur, avec délai)";
    case 3: return "Agilité Cognitive (2 Couleurs, avec délai)";
    case 4: return "Jeu Simon (Mémoire séquentielle & accélération)";
    default: return "Inconnu";
  }
}

void GameEngine::logError(const String &err) {
  lastErrorMessage = err;
  lastErrorTimestamp = millis();
  addLog("ERREUR : " + err, "ERROR");
}

void GameEngine::printDashboard() {
  // Optionnel : affichage console compact
}

