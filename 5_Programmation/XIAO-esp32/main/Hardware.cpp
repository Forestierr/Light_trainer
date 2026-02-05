#include "Hardware.h"

HardwareManager Hardware;

void HardwareManager::init() {
  DEBUG_PRINT(DEBUG_INFO, "Initializing HardwareManager...");
  // Init LEDs
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  setLight(0);
  DEBUG_PRINT(DEBUG_INFO, "LEDs initialized on pin D%d.", LED_PIN);

  // Init I2C & Sensor
  Wire.begin(SDA_PIN, SCL_PIN);
  if (!lox.begin()) {
    hasSensor = false;
    setLight(0xFFA500);  // Orange = Erreur Capteur
    DEBUG_PRINT(DEBUG_ERROR, "VL53L0X sensor initialization failed! Setting light to Orange.");
  } else {
    hasSensor = true;
    lox.startRangeContinuous();  // Mode rapide
    setLight(0x00FF00);          // Vert = OK
    delay(500);
    setLight(0);
    DEBUG_PRINT(DEBUG_INFO, "VL53L0X sensor initialized successfully. Setting light to Green.");
  }
}

void HardwareManager::setLight(uint32_t color) {
  DEBUG_PRINT(DEBUG_INFO, "Setting all LEDs to color %lx (original RGB).", (unsigned long)color); // Use %lx and cast
  // Extract R, G, B components
  uint8_t r = (color >> 16) & 0xFF;
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = color & 0xFF;

  // Recombine in GRB order expected by WS2812B
  CRGB grbColor = CRGB(g, r, b); // Use CRGB constructor for explicit GRB if needed

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = grbColor;
  }
  FastLED.show();
  DEBUG_PRINT(DEBUG_INFO, "LEDs updated (converted to GRB).");
}

void HardwareManager::blink(uint32_t color, int count) {
  for (int i = 0; i < count; i++) {
    setLight(color);
    delay(100);
    setLight(0);
    delay(100);
  }
}

void HardwareManager::rainbowBlink(int count, int delayMs) {
  DEBUG_PRINT(DEBUG_INFO, "Rainbow blinking LEDs for %d cycles with %dms delay.", count, delayMs);
  uint32_t colors[] = {0xFF0000, 0xFF7F00, 0xFFFF00, 0x00FF00, 0x0000FF, 0x4B0082, 0x9400D3}; // ROYGBIV
  int numColors = sizeof(colors) / sizeof(colors[0]);

  for (int c = 0; c < count; c++) {
    for (int i = 0; i < numColors; i++) {
      setLight(colors[i]);
      delay(delayMs);
    }
  }
  setLight(0); // Turn off LEDs after animation
}

// Non-blocking rainbow animation
void HardwareManager::rainbowAnimation() {
  static unsigned long lastUpdate = 0;
  static uint8_t hue = 0;

  if (millis() - lastUpdate > 20) { // Update every 20ms
    lastUpdate = millis();
    fill_rainbow(leds, NUM_LEDS, hue, 7); // '7' is the delta hue for color spread
    FastLED.show();
    hue++; // Increment hue for the next frame
    if (hue >= 255) hue = 0;
  }
}

int HardwareManager::getDistance() {
  if (!hasSensor) {
    return 8192;  // Hors portée
  }

  // Lecture non-bloquante simplifiée (on lit toutes les 50ms max)
  if (millis() - lastSensorRead < 50) return -1;

  lastSensorRead = millis();
  if (lox.isRangeComplete()) {
    int dist = lox.readRange();
    return dist;
  }
  return -1;
}

void HardwareManager::checkSleep() {
  if (millis() - lastActivityTime > INACTIVITY_TIMEOUT) {
    DEBUG_PRINT(DEBUG_INFO, "Inactivity detected (last activity %lu ms ago). Entering deep sleep.", millis() - lastActivityTime);

    // Petit feedback visuel avant de dormir (ex: Rouge lent)
    setLight(0xFF0000);
    delay(1000);
    setLight(0);

    // Configuration du réveil (Optionnel : si tu as un bouton sur un GPIO)
    // esp_sleep_enable_ext0_wakeup(GPIO_NUM_X, 0);

    DEBUG_PRINT(DEBUG_INFO, "Entering Deep Sleep...");
    esp_deep_sleep_start();
  }
}

void HardwareManager::update() {
  // Logique de détection des gestes (transférée du main original)
  int dist = getDistance();

  // Reset flags
  triggerStart = false;
  triggerHit = false;
  triggerMode = -1;

  if (dist != -1 && dist < 8000) {
    resetActivityTimer();  // reset le timer du deep sleep
    // Logique "Menu" (Distance moyenne)
    // Permet de selectionner les modes 0-3 en fonction de la distance (70mm à 300mm)
    if (dist > 70 && dist < 400) { // Extending the upper range to be more forgiving for max mode selection
      triggerMode = map(dist, 70, 300, 0, 3);
      triggerMode = constrain(triggerMode, 0, 3);
      DEBUG_PRINT(DEBUG_INFO, "Mode change trigger detected. Distance: %dmm, Mode: %d.", dist, triggerMode);
    }
    // Logique "Start" / "Hit" (Distance courte)
    if (dist < 70) {
      triggerStart = true;  // Contextuel (sera utilisé si en Menu)
      triggerHit = true;    // Contextuel (sera utilisé si en Jeu)
      DEBUG_PRINT(DEBUG_INFO, "Start/Hit trigger detected. Distance: %dmm.", dist);
    }
  } else if (dist == 8192) {
    DEBUG_PRINT(DEBUG_VERBOSE, "Sensor out of range or not active. Distance: %d", dist);
  } else if (dist == -1) {
    DEBUG_PRINT(DEBUG_VERBOSE, "Sensor reading not ready. Distance: %d", dist);
  }
}