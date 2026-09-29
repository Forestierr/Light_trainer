#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include "Config.h"
#include "GameNetwork.h"
#include "Hardware.h"
#include <vector>

// Structure pour enregistrer l'historique d'une frappe (Hit)
struct HitRecord {
  uint32_t hitNum;
  uint64_t podId;
  uint32_t color;
  uint32_t reactionTimeMs;
  uint32_t timestampMs;
};

// Structure pour le journal de logs système
struct LogEntry {
  uint32_t timestampMs;
  String level;
  String message;
};

// Structure pour gérer les réapparitions futures
struct ScheduledSpawn {
  unsigned long activationTime;  // Heure à laquelle allumer (millis)
  uint32_t color;                // Couleur à allumer
};

// Enum for Simon Game State Machine
enum SimonState {
  SIMON_STATE_IDLE,
  SIMON_STATE_DEMO_LIGHT_ON,
  SIMON_STATE_DEMO_LIGHT_OFF,
  SIMON_STATE_INPUT,
  SIMON_STATE_SUCCESS,
  SIMON_STATE_GAMEOVER
};

// Structure d'une étape dans la séquence Simon
struct SimonStep {
  uint64_t podId;
  uint32_t color;
};

// Structure pour les résultats d'un test Ping
struct PingPodResult {
  uint64_t podId;
  int8_t rssi;
  uint16_t rttMs;
  uint8_t batteryPct;
};

// Enum for non-blocking startGame sequence
enum StartGameSequenceState {
  START_GAME_IDLE,
  START_GAME_WAIT_COLOR1_BROADCAST,
  START_GAME_WAIT_COLOR2_BROADCAST
};

class GameEngine {
public:
  GameEngine();
  void init();
  void loop();

  // Gestion des modes de jeu (5 Modes : 0 à 4)
  const GameMode& getGameMode(int modeIndex) const {
    if (modeIndex >= 0 && modeIndex < 5) {
      return modes[modeIndex];
    }
    return modes[0];
  }

  GameMode (&getAllGameModes())[5] {
    return modes;
  }

  // Contrôles de jeu
  void startGame();
  void pauseGame();
  void resumeGame();
  void stopGame();
  void returnToMenu();
  void setMode(int mode);

  // Contrôles du Mode Manuel (Coach)
  void setManualPod(uint64_t podId, uint32_t color);
  void setAllPodsColor(uint32_t color);
  void turnOffAllPods();
  bool isManualActive() const { return isManualMode; }

  // Statistiques de session & métriques globales
  State getCurrentState() const { return currentState; }
  int getCurrentModeIndex() const { return currentMode; }
  const char* getModeDescription(int index) const;
  bool isRunning() const { return gameRunning; }
  bool isPaused() const { return gamePaused; }
  uint32_t getElapsedTimeMs() const;
  uint32_t getTotalHits() const { return sessionTotalHits; }
  uint32_t getLastReactionTimeMs() const { return lastReactionTime; }
  uint32_t getBestReactionTimeMs() const { return bestReactionTime; }
  float getAvgReactionTimeMs() const;
  float getCadenceBpm() const;

  // Métriques spécifiques pour les modes bicolores (Duel 1 vs 1 / Agilité)
  uint32_t getC1Hits() const { return c1Hits; }
  uint32_t getC1BestRt() const { return c1BestRt; }
  float getC1AvgRt() const { return c1Hits > 0 ? (float)c1SumRt / c1Hits : 0.0f; }

  uint32_t getC2Hits() const { return c2Hits; }
  uint32_t getC2BestRt() const { return c2BestRt; }
  float getC2AvgRt() const { return c2Hits > 0 ? (float)c2SumRt / c2Hits : 0.0f; }

  // Métriques spécifiques pour le Jeu Simon (Mode 4)
  SimonState getSimonState() const { return simonState; }
  const char* getSimonStateString() const;
  uint32_t getSimonRound() const { return simonRound; }
  uint32_t getSimonBestRound() const { return simonBestRound; }
  size_t getSimonInputStep() const { return simonInputIndex; }
  size_t getSimonTotalSteps() const { return simonSequence.size(); }
  uint32_t getSimonStepDurationMs() const { return simonStepDurationMs; }
  uint32_t getPodColorForSimon(uint64_t podId, size_t colorIndexOffset = 0) const;

  // Historique des frappes & export CSV
  const std::vector<HitRecord>& getHitHistory() const { return hitHistory; }
  void clearHitHistory();
  String generateCsvReport();

  // Journal de logs
  void addLog(const String &msg, const String &level = "INFO");
  const std::vector<LogEntry>& getLogs() const { return systemLogs; }
  void clearLogs();

  // Debug & Dashboard Série
  void printDashboard();
  void logError(const String &err);

  // Réseau & Test Ping
  void processNetwork() { handleNetwork(); }
  void clearPingResults() { lastPingResults.clear(); }
  void addPingResult(uint64_t id, int8_t rssi, uint16_t rttMs, uint8_t batPct);
  const std::vector<PingPodResult>& getPingResults() const { return lastPingResults; }
  void setPingInProgress(bool inProgress, unsigned long startTime) {
    pingInProgress = inProgress;
    pingStartTime = startTime;
  }
  bool isPingInProgress() const { return pingInProgress; }

private:
  // Variables Test Ping
  bool pingInProgress = false;
  unsigned long pingStartTime = 0;
  std::vector<PingPodResult> lastPingResults;
  GameMode modes[5] = {
    { 0x00FF00, 0x000000, 0, 0 },   // Mode 0: Vert (1 couleur, sans délai)
    { 0xFF0000, 0x00FF00, 0, 0 },   // Mode 1: Rouge/Vert (2 couleurs, sans délai)
    { 0x0000FF, 0x000000, 1, 5 },   // Mode 2: Bleu (1 couleur, délai)
    { 0xFFFF00, 0xFF00FF, 1, 5 },   // Mode 3: Jaune/Magenta (2 couleurs, délai)
    { 0x30D158, 0x000000, 800, 6 }  // Mode 4: Jeu Simon (Vitesse base: 800ms, Accélération: 6%/tour)
  };

  State currentState = MENU;
  int currentMode = 0;

  // Variables de session globales
  bool gameRunning = false;
  bool gamePaused = false;
  bool isManualMode = false;
  uint32_t sessionStartTime = 0;
  uint32_t pauseStartTime = 0;
  uint32_t totalPausedDuration = 0;
  uint32_t sessionTotalHits = 0;
  uint32_t lastReactionTime = 0;
  uint32_t bestReactionTime = 0;
  uint32_t sumReactionTimes = 0;

  // Variables de métriques bicolores (Duel 1 vs 1)
  uint32_t c1Hits = 0;
  uint32_t c1BestRt = 0;
  uint32_t c1SumRt = 0;
  uint32_t c2Hits = 0;
  uint32_t c2BestRt = 0;
  uint32_t c2SumRt = 0;

  // Variables pour le Jeu Simon (Mode 4)
  SimonState simonState = SIMON_STATE_IDLE;
  std::vector<SimonStep> simonSequence;
  size_t simonDemoIndex = 0;
  size_t simonInputIndex = 0;
  uint32_t simonRound = 0;
  uint32_t simonBestRound = 0;
  unsigned long simonStepTimer = 0;
  uint32_t simonStepDurationMs = 800;

  // Historique des touches et logs
  std::vector<HitRecord> hitHistory;
  std::vector<LogEntry> systemLogs;

  // Non-blocking startGame sequence variables
  StartGameSequenceState startGameSequenceState = START_GAME_IDLE;
  unsigned long startGameTimer = 0;

  struct ActiveTarget {
    uint64_t id;
    uint32_t color;
    unsigned long activationTime;
  };
  std::vector<ActiveTarget> activeTargets;
  std::vector<ScheduledSpawn> pendingSpawns;

  // Helpers internes
  void handleInputs();
  void handleNetwork();
  void processStartGameSequence();
  void scheduleNextTurn(uint32_t color);
  void activateRandomPod(uint32_t color);
  void onHitReceived(uint64_t senderId);

  // Helpers Simon
  void startSimonGame();
  void startSimonNextRound();
  void processSimonLoop();
  void handleSimonHit(uint64_t senderId);
  void triggerSimonSuccess();
  void triggerSimonGameOver();

  // Debug
  String lastErrorMessage = "Aucune";
  unsigned long lastErrorTimestamp = 0;
};

extern GameEngine Game;

#endif