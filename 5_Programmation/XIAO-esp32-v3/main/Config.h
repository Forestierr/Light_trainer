#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ---- CONFIGURATION MATERIELLE (Alignée Schéma V3 & Veroboard) ----
#define LED_PIN D7        // Broche 8 de U1 (LEDS WS2812B Data)
#define NUM_LEDS 7
#define SCL_PIN D4        // Broche 5 de U1 (I2C SCL - VL53L0X)
#define SDA_PIN D5        // Broche 6 de U1 (I2C SDA - VL53L0X)

#define SW_PIN D2         // Broche 3 de U1 (3V3_S - Détection ON/OFF & Wake-up)
#define CHARGE_PIN D10    // Broche 11 de U1 (Détection 5V USB VBUS)
#define BUZZER_PIN D3     // Broche 4 de U1 (Prévision Buzzer TP2)

// ---- CONFIGURATION TOF (VL53L0X) & GESTES UX ----
#define TOF_TRIGGER_DISTANCE 150 // Distance max en mm pour détecter la présence de la main (Tap / Hit / Hold)
#define TOF_PRECISION_MODE Adafruit_VL53L0X::VL53L0X_SENSE_HIGH_SPEED // Mode 50Hz pour réactivité sportive

// ---- PARAMETRES GESTUELS ----
#define GESTURE_TAP_MAX_MS 500            // Durée max pour un Tap court (changement de mode en Menu)
#define GESTURE_START_HOLD_MS 1500        // Durée de maintien pour démarrer la partie (Start en Menu)
#define GESTURE_MENU_HOLD_MS 3000         // Durée de maintien pour retourner au Menu depuis le jeu
#define GESTURE_GLITCH_TOLERANCE_MS 250   // Tolérance aux micro-décrochages optiques avant réinitialisation du Hold
#define HIT_FLASH_MS 60                   // Durée du flash blanc de confirmation lors d'un Hit

#define NVS_KEY_BRIGHTNESS "led_bright" // NVS key for brightness
#define DEFAULT_BRIGHTNESS 50 // Default brightness (0-255)

// ---- BATTERY MONITORING ----
#define ENABLE_BATTERY_MANAGEMENT true   // Activé pour le prototype Veroboard
#define BAT_ADC_PIN D1                   // Broche 2 de U1 (Mesure tension batterie)

// Pont diviseur 100k / 100k
#define BAT_R1 100.0  // kOhm
#define BAT_R2 100.0  // kOhm
#define VOLTAGE_DIVIDER_RATIO (BAT_R2 / (BAT_R1 + BAT_R2))

#define BAT_VOLT_FULL 4.2   // Volts (100% for LiPo)
#define BAT_VOLT_LOW 3.4    // Volts (approx 5% for LiPo, check datasheet for exact value)
#define BAT_VOLT_EMPTY 3.0  // Volts (0% for LiPo, critical cut-off)

#define ADC_MAX_VALUE 4095.0  // 12-bit ADC
#define ADC_REF_VOLTAGE 3.3   // ADC reference voltage for ESP32

// ---- BATTERY THRESHOLDS ----
#define CRITICAL_BATTERY_PERCENTAGE 2  // %
#define USB_POWERED_VOLTAGE_THRESHOLD 0.5 // Volts - if battery voltage is below this, assume USB power and no deep sleep

// ---- CONFIGURATION SYSTEME ----
#define MAX_NODES 20                // Remplacer std::vector par un tableau fixe pour la stabilité
#define SENSOR_TIMEOUT 200          // Evite de bloquer si le capteur plante
#define DISCOVERY_INT 5000         // Découverte toutes les 5s
#define INACTIVITY_TIMEOUT 1800000  // 30 minutes

// ---- DEBUG CONFIGURATION ----
// Define debug levels
#define DEBUG_NONE 0
#define DEBUG_ERROR 1
#define DEBUG_WARNING 2
#define DEBUG_INFO 3
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

// ---- MESH CONFIGURATION ----
#define DEFAULT_MESH_TTL 3          // Nombre maximum de sauts dans le réseau mesh

// ---- PROTOCOLE ----
enum MsgType { CMD_START,
               CMD_ACTIVATE,
               CMD_HIT,
               CMD_PING,
               CMD_ACK,
               CMD_RETURN_TO_MENU,
               CMD_MANUAL_SET,
               CMD_TEST_LEDS,
               CMD_POD_TELEMETRY,
               CMD_HEARTBEAT,
               CMD_CLAIM_MASTER };

// ---- GESTION DU MASTER & ÉLECTION DYNAMIQUE ----
#define MASTER_HEARTBEAT_INTERVAL_MS 1000  // Période d'émission du Heartbeat Master
#define MASTER_TIMEOUT_MS            3000  // Délai sans heartbeat avant élection d'urgence
#define BOOT_ELECTION_WINDOW_MS      2000  // Fenêtre d'attente au boot avant auto-élection

typedef struct __attribute__((packed)) struct_message {
  uint32_t msgId;     // Identifiant unique du message pour le mesh
  uint64_t senderID;  // ID du pod expéditeur original
  uint64_t targetID;  // 0 = Broadcast (tous), sinon ID du pod ciblé
  uint8_t msgType;    // Type de message (MsgType)
  uint8_t ttl;        // Time-To-Live (nombre de sauts restants)
  uint32_t color;     // Couleur associée
} struct_message;

// ---- ETATS DU POD ----
enum State { MENU,              // Mode selection
             CONTROLLER,        // This pod is the orchestrator
             CONTROLLER_TARGET, // This pod is the orchestrator AND the current target to light up
             ACTIVE_TARGET,     // Target to light up
             IDLE_LISTENER };   // Wait for command

// ---- MODES DE JEU ----
struct GameMode {
  uint32_t color1;
  uint32_t color2;
  uint32_t delayMin;  // secondes
  uint32_t delayMax;  // secondes
};

// ---- HELPERS DE DEBUG ----
inline const char* msgTypeToString(uint8_t type) {
  switch (type) {
    case CMD_START:          return "CMD_START";
    case CMD_ACTIVATE:       return "CMD_ACTIVATE";
    case CMD_HIT:            return "CMD_HIT";
    case CMD_PING:           return "CMD_PING";
    case CMD_ACK:            return "CMD_ACK";
    case CMD_RETURN_TO_MENU: return "CMD_RETURN_TO_MENU";
    case CMD_MANUAL_SET:     return "CMD_MANUAL_SET";
    case CMD_TEST_LEDS:      return "CMD_TEST_LEDS";
    case CMD_POD_TELEMETRY:  return "CMD_POD_TELEMETRY";
    case CMD_HEARTBEAT:      return "CMD_HEARTBEAT";
    case CMD_CLAIM_MASTER:   return "CMD_CLAIM_MASTER";
    default:                 return "CMD_UNKNOWN";
  }
}

inline const char* stateToString(State s) {
  switch (s) {
    case MENU:              return "MENU";
    case CONTROLLER:        return "CONTROLLER (Master)";
    case CONTROLLER_TARGET: return "CONTROLLER_TARGET (Master actif)";
    case ACTIVE_TARGET:     return "ACTIVE_TARGET (Esclave actif)";
    case IDLE_LISTENER:     return "IDLE_LISTENER (Esclave en attente)";
    default:                return "UNKNOWN_STATE";
  }
}

#endif