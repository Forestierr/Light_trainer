#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ---- CONFIGURATION MATERIELLE ----
#define LED_PIN D0
#define NUM_LEDS 7
#define SDA_PIN D4
#define SCL_PIN D5
#define BRIGHTNESS 50

// ---- CONFIGURATION SYSTEME ----
#define MAX_NODES 20                // Remplacer std::vector par un tableau fixe pour la stabilité
#define SENSOR_TIMEOUT 200          // Evite de bloquer si le capteur plante
#define DISCOVERY_INT 10000         // Découverte toutes les 10s
#define INACTIVITY_TIMEOUT 1800000  // 30 minutes

// ---- DEBUG CONFIGURATION ----
// Define debug levels
#define DEBUG_NONE 0
#define DEBUG_ERROR 1
#define DEBUG_INFO 2
#define DEBUG_WARNING 3
#define DEBUG_VERBOSE 4

// Set the desired debug level (e.g., DEBUG_INFO to show errors and info, DEBUG_VERBOSE for all)
#define CURRENT_DEBUG_LEVEL DEBUG_INFO

// Debug macro for serial printing
#define DEBUG_PRINT(level, format, ...) \
  do { \
    if (level <= CURRENT_DEBUG_LEVEL) { \
      Serial.printf("[%s] " format "\n", \
                    (level == DEBUG_ERROR ? "ERROR" : (level == DEBUG_INFO ? "INFO" : (level == DEBUG_WARNING ? "WARNING" : (level == DEBUG_VERBOSE ? "VERBOSE" : "UNKNOWN")))), \
                    ##__VA_ARGS__); \
    } \
  } while (0)

// ---- PROTOCOLE ----
enum MsgType { CMD_START,
               CMD_ACTIVATE,
               CMD_HIT,
               CMD_PING,
               CMD_ACK };

typedef struct struct_message {
  uint8_t msgType;
  uint32_t color;
  uint64_t targetID;
  uint64_t senderID;
} struct_message;

// ---- ETATS DU POD ----
enum State { MENU,
             CONTROLLER,
             CONTROLLER_TARGET,
             ACTIVE_TARGET,
             IDLE_LISTENER };

// ---- MODES DE JEU ----
struct GameMode {
  uint32_t color1;
  uint32_t color2;
  uint32_t delayMin;  // secondes
  uint32_t delayMax;  // secondes
};

#endif