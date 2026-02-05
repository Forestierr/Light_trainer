#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ---- CONFIGURATION MATERIELLE ----
#define LED_PIN D0
#define NUM_LEDS 7
#define SDA_PIN D4    // Use for VL53L0X (TOF)
#define SCL_PIN D5    // Use for VL53L0X (TOF)
// #define BRIGHTNESS 50 // This will be replaced by NVS setting

#define NVS_KEY_BRIGHTNESS "led_bright" // NVS key for brightness
#define DEFAULT_BRIGHTNESS 50 // Default brightness (0-255)

// ---- BATTERY MONITORING ----
#define ENABLE_BATTERY_MANAGEMENT false  // Set to true to enable battery monitoring and LED indications
#define BAT_ADC_PIN D6                  // GPIO21 for Battery Voltage Reading (ADC1_CH0)

// Voltage divider resistors for battery monitoring (R1 connects to battery +, R2 connects to GND)
// V_out = V_batt * (R2 / (R1 + R2))
// For LiPo 3.0V-4.2V, ADC 0-3.3V. Using R1=4.7k, R2=10k gives a ratio of 10k/(4.7k+10k) = 0.68
#define BAT_R1 4.7   // kOhm
#define BAT_R2 10.0  // kOhm
#define VOLTAGE_DIVIDER_RATIO (BAT_R2 / (BAT_R1 + BAT_R2))

#define BAT_VOLT_FULL 4.2   // Volts (100% for LiPo)
#define BAT_VOLT_LOW 3.4    // Volts (approx 5% for LiPo, check datasheet for exact value)
#define BAT_VOLT_EMPTY 3.0  // Volts (0% for LiPo, critical cut-off)

#define ADC_MAX_VALUE 4095.0  // 12-bit ADC
#define ADC_REF_VOLTAGE 3.3   // ADC reference voltage for ESP32

// ---- BATTERY THRESHOLDS ----
#define CRITICAL_BATTERY_PERCENTAGE 2  // %

// ---- CONFIGURATION SYSTEME ----
#define MAX_NODES 20                // Remplacer std::vector par un tableau fixe pour la stabilité
#define SENSOR_TIMEOUT 200          // Evite de bloquer si le capteur plante
#define DISCOVERY_INT 10000         // Découverte toutes les 10s
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

// ---- PROTOCOLE ----
enum MsgType { CMD_START,
               CMD_ACTIVATE,
               CMD_HIT,
               CMD_PING,
               CMD_ACK,
               CMD_RETURN_TO_MENU };

typedef struct struct_message {
  uint8_t msgType;
  uint32_t color;
  uint64_t targetID;
  uint64_t senderID;
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

#endif