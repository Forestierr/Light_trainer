#include "Hardware.h"

HardwareManager Hardware;

HardwareManager::HardwareManager() :
  lox(), // Initialize lox here
  hasSensor(false),
  lastSensorRead(0),
  lastActivityTime(0),
  triggerStart(false),
  triggerHit(false),
  triggerMode(-1),
  blinkState(BLINK_IDLE),
  blinkColor(0),
  blinkCountRemaining(0),
  blinkTimer(0),
  blinkDelay(100),
  rainbowBlinkState(RAINBOW_BLINK_IDLE),
  rainbowBlinkCountRemaining(0),
  rainbowBlinkTimer(0),
  rainbowBlinkDelay(0),
  rainbowBlinkCurrentColorIndex(0)
#if ENABLE_BATTERY_MANAGEMENT
  ,
  batteryVoltage(0.0),
  lastBatteryCheckTime(0),
  // lastMeasuredVoltage(0.0), // Not needed as it's static in isCharging()
  // lastVoltageMeasurementTime(0), // Not needed as it's static in isCharging()
  voltageHistory{0}, // Initialize array
  voltageHistoryIndex(0)
#endif
{
  // No hardware-dependent initialization here
}

void HardwareManager::init() {
  DEBUG_PRINT(DEBUG_INFO, "Initializing HardwareManager...");
  // Init LEDs
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  setBrightness(DEFAULT_BRIGHTNESS); // Set default brightness here
  setLight(0);
  DEBUG_PRINT(DEBUG_INFO, "LEDs initialized on pin D%d.", LED_PIN);

#if ENABLE_BATTERY_MANAGEMENT
  // Init ADC for battery monitoring
  pinMode(BAT_ADC_PIN, INPUT);
  DEBUG_PRINT(DEBUG_INFO, "Battery initialized on pin D%d.", BAT_ADC_PIN);
#else
  DEBUG_PRINT(DEBUG_INFO, "Battery verification not initialized");
#endif

  // Init I2C & Sensor
  Wire.begin(SDA_PIN, SCL_PIN);
  if (!lox.begin()) {
    hasSensor = false;
    setLight(0xFFA500);  // Orange = Erreur Capteur
    DEBUG_PRINT(DEBUG_ERROR, "VL53L0X sensor initialization failed! Setting light to Orange.");
    sleep(5); // Attendre 5 sec
  } else {
    hasSensor = true;
    lox.startRangeContinuous();  // Mode rapide
    DEBUG_PRINT(DEBUG_INFO, "VL53L0X sensor initialized successfully.");
  }
}

void HardwareManager::setBrightness(uint8_t brightness) {
  FastLED.setBrightness(brightness);
  FastLED.show(); // Update LEDs immediately with new brightness
  DEBUG_PRINT(DEBUG_INFO, "LED brightness set to %d.", brightness);
}

void HardwareManager::setLight(uint32_t color) {
  DEBUG_PRINT(DEBUG_INFO, "Setting all LEDs to color %lx (original RGB).", (unsigned long)color); // Use %lx and cast
  // Extract R, G, B components
  uint8_t r = (color >> 16) & 0xFF;
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = color & 0xFF;

  // Recombine in GRB order expected by WS2812B
  CRGB grbColor = CRGB(r, g, b); // Corrected to RGB order

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = grbColor;
  }
  FastLED.show();
}

void HardwareManager::blink(uint32_t color, int count) {
  if (blinkState == BLINK_IDLE) { // Only start if no other blink is active
    DEBUG_PRINT(DEBUG_INFO, "Initiating non-blocking blink for color %lx, count %d.", (unsigned long)color, count);
    blinkColor = color;
    blinkCountRemaining = count * 2; // Each cycle has ON and OFF phase
    blinkState = BLINK_ON;
    blinkTimer = millis();
    setLight(blinkColor); // Turn on immediately
  } else {
    DEBUG_PRINT(DEBUG_WARNING, "Attempted to start blink while another blink is active. Ignored.");
  }
}

void HardwareManager::rainbowBlink(int count, int delayMs) {
  if (rainbowBlinkState == RAINBOW_BLINK_IDLE) { // Only start if no other rainbow blink is active
    DEBUG_PRINT(DEBUG_INFO, "Initiating non-blocking rainbow blink for %d cycles with %dms delay.", count, delayMs);
    rainbowBlinkCountRemaining = count;
    rainbowBlinkDelay = delayMs;
    rainbowBlinkCurrentColorIndex = 0;
    rainbowBlinkState = RAINBOW_BLINK_CYCLE;
    rainbowBlinkTimer = millis();
    setLight(rainbowColors[rainbowBlinkCurrentColorIndex]); // Set first color immediately
  } else {
    DEBUG_PRINT(DEBUG_WARNING, "Attempted to start rainbow blink while another rainbow blink is active. Ignored.");
  }
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
#if ENABLE_BATTERY_MANAGEMENT
  // Check for critical battery level
  if (getBatteryPercentage() < CRITICAL_BATTERY_PERCENTAGE && !isCharging()) {
    DEBUG_PRINT(DEBUG_ERROR, "CRITICAL BATTERY LEVEL (%d%%)! Entering deep sleep to protect battery.", getBatteryPercentage());

    // Visual feedback for critical battery (Red blinking)
    blink(0xFF0000, 3); // Use non-blocking blink to initiate, but will be cut short by deep sleep

    DEBUG_PRINT(DEBUG_INFO, "Entering Deep Sleep...");
    esp_deep_sleep_start();
  }
#endif

  // Original inactivity sleep check
  if (millis() - lastActivityTime > INACTIVITY_TIMEOUT) {
    DEBUG_PRINT(DEBUG_INFO, "Inactivity detected (last activity %lu ms ago). Entering deep sleep.", millis() - lastActivityTime);

    // Petit feedback visuel avant de dormir (ex: Rouge lent)
    blink(0xFF0000, 2); // Use non-blocking blink to initiate, but will be cut short by deep sleep

    DEBUG_PRINT(DEBUG_INFO, "Entering Deep Sleep...");
    esp_deep_sleep_start();
  }
}

// New Battery Management functions
#if ENABLE_BATTERY_MANAGEMENT
float HardwareManager::getBatteryVoltage() {
  // Rate-limit battery checks to every 5 seconds
  if (millis() - lastBatteryCheckTime > 5000 || lastBatteryCheckTime == 0) {
    lastBatteryCheckTime = millis();
    // Read the analog value
    int adc_val = analogRead(BAT_ADC_PIN);

    // Convert ADC value to voltage at the ADC pin
    float adc_voltage = (adc_val * ADC_REF_VOLTAGE) / ADC_MAX_VALUE;

    // Apply the voltage divider formula to get the actual battery voltage
    batteryVoltage = adc_voltage / VOLTAGE_DIVIDER_RATIO;

    DEBUG_PRINT(DEBUG_VERBOSE, "Battery ADC: %d, ADC Voltage: %.2fV, Battery Voltage: %.2fV", adc_val, adc_voltage, batteryVoltage);
  }
  return batteryVoltage;
}

int HardwareManager::getBatteryPercentage() {
  float voltage = getBatteryVoltage();
  // Map voltage to percentage, assuming linear discharge for simplicity between empty and full
  // Using BAT_VOLT_EMPTY for 0% and BAT_VOLT_FULL for 100%
  int percentage = map(voltage * 100, BAT_VOLT_EMPTY * 100, BAT_VOLT_FULL * 100, 0, 100);
  percentage = constrain(percentage, 0, 100); // Ensure percentage is within 0-100
  DEBUG_PRINT(DEBUG_VERBOSE, "Battery Percentage: %d%% (Voltage: %.2fV)", percentage, voltage);
  return percentage;
}

bool HardwareManager::isCharging() {
  static unsigned long lastChargeCheckTime = 0;
  static float voltageAtLastCheck = 0.0; // Stores the average voltage from the previous check
  static bool currentlyCharging = false;
  
  // Update voltage history with the latest reading (more frequently than the full charge detection)
  voltageHistory[voltageHistoryIndex] = getBatteryVoltage();
  voltageHistoryIndex = (voltageHistoryIndex + 1) % 10; // Wrap around the buffer

  // Perform full charging detection logic every few seconds
  if (millis() - lastChargeCheckTime > 5000) { // Check every 5 seconds for charging trend
    lastChargeCheckTime = millis();
    
    // Calculate average of the voltage history for a more stable comparison
    float sumVoltage = 0;
    for (int i = 0; i < 10; i++) {
      sumVoltage += voltageHistory[i];
    }
    float averageVoltage = sumVoltage / 10.0;

    // Define a threshold for a significant voltage increase to indicate charging
    float voltageIncreaseThreshold = 0.01; // e.g., 10mV increase over the checking period

    // Logic to determine charging status
    if (averageVoltage > (voltageAtLastCheck + voltageIncreaseThreshold)) {
      // Voltage is consistently increasing, likely charging
      currentlyCharging = true;
    } else if (getBatteryVoltage() >= BAT_VOLT_FULL - 0.01) { 
      // Battery is full or nearly full, so it's not charging anymore (or trickle charging)
      currentlyCharging = false;
    } else if (averageVoltage < (voltageAtLastCheck - voltageIncreaseThreshold)) {
      // Voltage is consistently decreasing, not charging
      currentlyCharging = false;
    }
    // If none of the above, status remains as it was (e.g., stable voltage below full)

    voltageAtLastCheck = averageVoltage; // Update the reference voltage for the next comparison

    DEBUG_PRINT(DEBUG_INFO, "Charge check: Current %.2fV, Avg History %.2fV, Prev Check Avg %.2fV, Charging: %d", 
                getBatteryVoltage(), averageVoltage, voltageAtLastCheck, currentlyCharging);
  }
  
  return currentlyCharging;
}

bool HardwareManager::isLowBattery() {
  // Low battery if voltage is below BAT_VOLT_LOW AND not charging
  bool lowBatt = (getBatteryVoltage() < BAT_VOLT_LOW) && !isCharging();
  if (lowBatt) {
    DEBUG_PRINT(DEBUG_WARNING, "Low Battery detected! Voltage: %.2fV", batteryVoltage);
  }
  return lowBatt;
}

void HardwareManager::updateBatteryLED() {
  // Only update if no explicit non-blocking LED animation (blink or rainbowBlink) is active
  if (blinkState != BLINK_IDLE || rainbowBlinkState != RAINBOW_BLINK_IDLE) {
    // Another explicit animation is running, let it control the LEDs
    return;
  }

  if (isCharging()) {
    // Orange for charging
    setLight(0xFFA500);
    DEBUG_PRINT(DEBUG_INFO, "Setting LEDs to Orange (Charging)");
  } else if (isLowBattery()) {
    // Red for low battery
    setLight(0xFF0000);
    DEBUG_PRINT(DEBUG_WARNING, "Setting LEDs to Red (Low Battery)");
  } else {
    // Green for sufficient charge, but only if not otherwise active (e.g. game in progress)
    // For now, let's just make it green if full and not charging.
    // In actual game logic, this will be overridden by game colors.
    if (getBatteryPercentage() > 98) { // Only green if full
      setLight(0x00FF00);
      DEBUG_PRINT(DEBUG_INFO, "Setting LEDs to Green (Full Battery)");
    } else {
      // If not charging, not low, and not full, let the game logic decide the LED color
      // Or set to off/default if not in a game state
      setLight(0); // For now, turn off if not critical and not full
      DEBUG_PRINT(DEBUG_VERBOSE, "Battery normal, LEDs off (Game logic should control)");
    }
  }
}
#endif // ENABLE_BATTERY_MANAGEMENT

void HardwareManager::update() {
  processNonBlockingLEDAnimations(); // Process non-blocking LED animations
  // Logique de détection des gestes (transférée du main original)
  int dist = getDistance();

  // Reset flags
  clearAllTriggers();

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

// Helper function to manage non-blocking LED animations
void HardwareManager::processNonBlockingLEDAnimations() {
  unsigned long now = millis();

  // --- Process non-blocking blink ---
  if (blinkState != BLINK_IDLE) {
    if (now - blinkTimer >= blinkDelay) {
      blinkTimer = now;
      if (blinkState == BLINK_ON) {
        setLight(0); // Turn off
        blinkState = BLINK_OFF;
      } else { // BLINK_OFF
        setLight(blinkColor); // Turn on
        blinkState = BLINK_ON;
        blinkCountRemaining--;
      }

      if (blinkCountRemaining <= 0) {
        blinkState = BLINK_IDLE; // Animation complete
        setLight(0); // Ensure off
      }
    }
  }

  // --- Process non-blocking rainbow blink ---
  if (rainbowBlinkState != RAINBOW_BLINK_IDLE) {
    if (now - rainbowBlinkTimer >= rainbowBlinkDelay) {
      rainbowBlinkTimer = now;
      rainbowBlinkCurrentColorIndex++;
      if (rainbowBlinkCurrentColorIndex >= (sizeof(rainbowColors) / sizeof(rainbowColors[0]))) {
        rainbowBlinkCurrentColorIndex = 0;
        rainbowBlinkCountRemaining--;
      }

      if (rainbowBlinkCountRemaining > 0) {
        setLight(rainbowColors[rainbowBlinkCurrentColorIndex]);
      } else {
        rainbowBlinkState = RAINBOW_BLINK_IDLE; // Animation complete
        setLight(0); // Ensure off
      }
    }
  }
}