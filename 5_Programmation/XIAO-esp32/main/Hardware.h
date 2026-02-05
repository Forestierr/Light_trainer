#ifndef HARDWARE_H
#define HARDWARE_H

#include "Config.h"
#include <FastLED.h>
#include <Wire.h>
#include "Adafruit_VL53L0X.h"

class HardwareManager {
public:
  void init();
  void update();  // Pour les animations LED

  // LEDs
  void setLight(uint32_t color);
  void blink(uint32_t color, int count);
  void rainbowBlink(int count, int delayMs);
  void rainbowAnimation(); // Continuous rainbow animation

  // Capteur
  bool isSensorActive() {
    return hasSensor;
  }
  int getDistance();  // Retourne -1 si erreur ou pas de nouvelle donnée

  // Inputs détectés
  bool triggerStart = false;
  bool triggerHit = false;
  int triggerMode = -1;

  // Sleep
  void checkSleep();
  void resetActivityTimer() {
    lastActivityTime = millis();
  }

private:
  CRGB leds[NUM_LEDS];
  Adafruit_VL53L0X lox = Adafruit_VL53L0X();
  bool hasSensor = false;
  unsigned long lastSensorRead = 0;
  unsigned long lastActivityTime = 0;
};

extern HardwareManager Hardware;  // Instance globale

#endif