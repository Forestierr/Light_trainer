#ifndef HARDWARE_H
#define HARDWARE_H

#include "Config.h"
#include <FastLED.h>
#include <Wire.h>
#include "Adafruit_VL53L0X.h"

// States for non-blocking blink
enum BlinkState {
  BLINK_IDLE,
  BLINK_ON,
  BLINK_OFF
};

// States for non-blocking rainbow blink
enum RainbowBlinkState {
  RAINBOW_BLINK_IDLE,
  RAINBOW_BLINK_CYCLE
};

class HardwareManager {
public:
  HardwareManager(); // Constructor declaration
  void init();
  void update();  // Pour les animations LED

  // LEDs
  void setLight(uint32_t color);
  void setBrightness(uint8_t brightness); // New setter
  void blink(uint32_t color, int count);
  void rainbowBlink(int count, int delayMs);
  void rainbowAnimation(); // Continuous rainbow animation

  // Capteur
  bool isSensorActive() {
    return hasSensor;
  }
  int getDistance();  // Retourne -1 si erreur ou pas de nouvelle donnée

  // Public getters for triggers
  bool getTriggerStart() const { return triggerStart; }
  bool getTriggerHit() const { return triggerHit; }
  int getTriggerMode() const { return triggerMode; }

  // Public setter for triggers
  void setTriggerHit(bool state) { triggerHit = state; }

  // Public methods to clear triggers
  void clearTriggerStart() { triggerStart = false; }
  void clearTriggerHit() { triggerHit = false; }
  void clearTriggerMode() { triggerMode = -1; }
  void clearAllTriggers() {
    triggerStart = false;
    triggerHit = false;
    triggerMode = -1;
  }

  // Sleep
  void checkSleep();
  void resetActivityTimer() {
    lastActivityTime = millis();
  }

#if ENABLE_BATTERY_MANAGEMENT
  // Battery Management
  float getBatteryVoltage();
  int getBatteryPercentage();
  bool isCharging();
  bool isLowBattery();
  void updateBatteryLED();
#endif

private:
  CRGB leds[NUM_LEDS];
  Adafruit_VL53L0X lox = Adafruit_VL53L0X();
  bool hasSensor = false;
  unsigned long lastSensorRead = 0;
  unsigned long lastActivityTime = 0;

  // Inputs détectés (moved to private)
  bool triggerStart = false;
  bool triggerHit = false;
  int triggerMode = -1;

  // Non-blocking blink variables
  BlinkState blinkState = BLINK_IDLE;
  uint32_t blinkColor;
  int blinkCountRemaining = 0;
  unsigned long blinkTimer = 0;
  int blinkDelay = 100; // Default delay for blink segments

  // Non-blocking rainbow blink variables
  RainbowBlinkState rainbowBlinkState = RAINBOW_BLINK_IDLE;
  int rainbowBlinkCountRemaining = 0;
  unsigned long rainbowBlinkTimer = 0;
  int rainbowBlinkDelay = 0;
  int rainbowBlinkCurrentColorIndex = 0;
  uint32_t rainbowColors[7] = {0xFF0000, 0xFF7F00, 0xFFFF00, 0x00FF00, 0x0000FF, 0x4B0082, 0x9400D3}; // ROYGBIV

  // Helper for non-blocking LED animations
  void processNonBlockingLEDAnimations();
  
#if ENABLE_BATTERY_MANAGEMENT
  // Battery related members
  float batteryVoltage = 0.0;
  unsigned long lastBatteryCheckTime = 0;
  
  float lastMeasuredVoltage = 0.0;
  unsigned long lastVoltageMeasurementTime = 0;
  float voltageHistory[10]; // Stores last 10 voltage readings
  int voltageHistoryIndex = 0;
#endif
};

extern HardwareManager Hardware;  // Instance globale

#endif