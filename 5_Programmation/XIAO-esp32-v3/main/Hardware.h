#ifndef HARDWARE_H
#define HARDWARE_H

#include "Config.h"
#include <FastLED.h>
#include <Wire.h>
#include <vector>
#include "Adafruit_VL53L0X.h"

// Échantillon de télémétrie batterie pour graphique style iOS
struct BatterySample {
  uint32_t timestampSec;
  uint8_t percentage;
  float voltage;
  bool isCharging;
};

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
  void setBrightness(uint8_t brightness);
  void blink(uint32_t color, int count);
  void rainbowBlink(int count, int delayMs);
  void rainbowAnimation(); // Continuous rainbow animation
  void setGauge(uint32_t activeColor, float progress, uint32_t bgColor = 0); // Jauge circulaire 0.0 - 1.0 sur les 7 LEDs
  void flashWhite(uint16_t durationMs = HIT_FLASH_MS); // Flash blanc instantané de confirmation

  // Capteur
  bool isSensorActive() const { return hasSensor; }
  int getDistance();
  int getLastValidDistance() const { return lastStoredDistance; }
  bool isHandCurrentlyPresent() const { return isHandPresent; }
  String getSensorDebugString();

  // Public getters for triggers
  bool getTriggerTap() const { return triggerTap; }
  bool getTriggerStartHold() const { return triggerStartHold; }
  bool getTriggerMenuHold() const { return triggerMenuHold; }
  bool getTriggerHit() const { return triggerHit; }
  float getHoldProgress() const { return holdProgress; }

  // Public setter for triggers
  void setTriggerHit(bool state) { triggerHit = state; }

  // Public methods to clear triggers
  void clearAllTriggers() {
    triggerTap = false;
    triggerStartHold = false;
    triggerMenuHold = false;
    triggerHit = false;
  }

  // Sleep & Power Switch
  void checkSleep();
  void checkPowerSwitch(); // Gère l'interrupteur ON/OFF matériel
  void goToDeepSleep();    // Isole les GPIOs et active la veille profonde
  void resetActivityTimer() {
    lastActivityTime = millis();
  }

#if ENABLE_BATTERY_MANAGEMENT
  // Battery Management & History
  float getBatteryVoltage();
  int getBatteryPercentage();
  bool isCharging();
  bool isLowBattery();
  void updateBatteryLED();
  String getBatteryDebugString();
  void recordBatterySample();
  const std::vector<BatterySample>& getBatteryHistory() const { return batteryHistory; }
#else
  String getBatteryDebugString() { return "Gestion batterie desactivee"; }
#endif

private:
  CRGB leds[NUM_LEDS];
  Adafruit_VL53L0X lox = Adafruit_VL53L0X();
  bool hasSensor = false;
  unsigned long lastSensorRead = 0;
  unsigned long lastActivityTime = 0;
  int lastStoredDistance = 8192;

  // Inputs détectés
  bool triggerTap = false;
  bool triggerStartHold = false;
  bool triggerMenuHold = false;
  bool triggerHit = false;
  float holdProgress = 0.0f;
  unsigned long lastHitTriggerTime = 0;
  bool hitArmed = true;

  // Machine à états gestuelle ToF
  bool isHandPresent = false;
  unsigned long handPresenceStartTime = 0;
  unsigned long handAbsenceStartTime = 0;
  bool startHoldTriggered = false;
  bool menuHoldTriggered = false;

  // Non-blocking hit flash
  bool isFlashingWhite = false;
  unsigned long flashWhiteEndTime = 0;

  // Non-blocking blink variables
  BlinkState blinkState = BLINK_IDLE;
  uint32_t blinkColor;
  int blinkCountRemaining = 0;
  unsigned long blinkTimer = 0;
  int blinkDelay = 100;

  // Non-blocking rainbow blink variables
  RainbowBlinkState rainbowBlinkState = RAINBOW_BLINK_IDLE;
  int rainbowBlinkCountRemaining = 0;
  unsigned long rainbowBlinkTimer = 0;
  int rainbowBlinkDelay = 0;
  int rainbowBlinkCurrentColorIndex = 0;
  uint32_t rainbowColors[7] = {0xFF0000, 0xFF7F00, 0xFFFF00, 0x00FF00, 0x0000FF, 0x4B0082, 0x9400D3};

  // Helper for non-blocking LED animations
  void processNonBlockingLEDAnimations();
  
#if ENABLE_BATTERY_MANAGEMENT
  // Battery related members
  float batteryVoltage = 0.0;
  unsigned long lastBatteryCheckTime = 0;
  unsigned long lastBatterySampleTime = 0;
  std::vector<BatterySample> batteryHistory;
  float voltageHistory[10];
  int voltageHistoryIndex = 0;
#endif
};

extern HardwareManager Hardware;  // Instance globale

#endif