#include "Hardware.h"

HardwareManager Hardware;

HardwareManager::HardwareManager() :
  lox(),
  hasSensor(false),
  lastSensorRead(0),
  lastActivityTime(0),
  triggerTap(false),
  triggerStartHold(false),
  triggerMenuHold(false),
  triggerHit(false),
  holdProgress(0.0f),
  isHandPresent(false),
  handPresenceStartTime(0),
  handAbsenceStartTime(0),
  startHoldTriggered(false),
  menuHoldTriggered(false),
  isFlashingWhite(false),
  flashWhiteEndTime(0),
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
  voltageHistory{0},
  voltageHistoryIndex(0)
#endif
{
}

void HardwareManager::init() {
  DEBUG_PRINT(DEBUG_INFO, "Initializing HardwareManager...");

  // Configuration des broches d'entrée matérielles
  pinMode(SW_PIN, INPUT_PULLDOWN);       // Interrupteur ON/OFF (D2)
  pinMode(CHARGE_PIN, INPUT);            // Détection USB 5V (D10) - haute impédance sans pull-down interne

  // Init LEDs sur D7
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  setBrightness(DEFAULT_BRIGHTNESS);
  setLight(0);
  DEBUG_PRINT(DEBUG_INFO, "LEDs initialized on pin D%d.", LED_PIN);

#if ENABLE_BATTERY_MANAGEMENT
  pinMode(BAT_ADC_PIN, INPUT);
  analogSetAttenuation(ADC_11db); // Atténuation 11dB pour lecture pleine échelle ADC ESP32-C3
  DEBUG_PRINT(DEBUG_INFO, "Battery initialized on pin D%d.", BAT_ADC_PIN);
  recordBatterySample(); // Échantillon initial à l'allumage
#else
  DEBUG_PRINT(DEBUG_INFO, "Battery verification not initialized");
#endif

  // Init I2C & Sensor en mode HIGH_SPEED pour réactivité sportive (SDA=D5, SCL=D4)
  Wire.begin(SDA_PIN, SCL_PIN);
  if (!lox.begin(0x29, false, &Wire, TOF_PRECISION_MODE)) {
    hasSensor = false;
    setLight(0xFFA500);  // Orange = Erreur Capteur
    DEBUG_PRINT(DEBUG_ERROR, "VL53L0X sensor initialization failed! Setting light to Orange.");
  } else {
    hasSensor = true;
    lox.startRangeContinuous();  // Mode rapide continu
    DEBUG_PRINT(DEBUG_INFO, "VL53L0X sensor initialized successfully in HIGH_SPEED mode.");
  }
}

void HardwareManager::setBrightness(uint8_t brightness) {
  FastLED.setBrightness(brightness);
  FastLED.show();
  DEBUG_PRINT(DEBUG_INFO, "LED brightness set to %d.", brightness);
}

void HardwareManager::setLight(uint32_t color) {
  uint8_t r = (color >> 16) & 0xFF;
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = color & 0xFF;

  CRGB grbColor = CRGB(r, g, b);

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = grbColor;
  }
  FastLED.show();
}

void HardwareManager::setGauge(uint32_t activeColor, float progress, uint32_t bgColor) {
  progress = constrain(progress, 0.0f, 1.0f);
  int numActiveLeds = round(progress * NUM_LEDS);

  uint8_t ar = (activeColor >> 16) & 0xFF;
  uint8_t ag = (activeColor >> 8) & 0xFF;
  uint8_t ab = activeColor & 0xFF;
  CRGB activeCRGB = CRGB(ar, ag, ab);

  uint8_t br = (bgColor >> 16) & 0xFF;
  uint8_t bg = (bgColor >> 8) & 0xFF;
  uint8_t bb = bgColor & 0xFF;
  CRGB bgCRGB = CRGB(br, bg, bb);

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = (i < numActiveLeds) ? activeCRGB : bgCRGB;
  }
  FastLED.show();
}

void HardwareManager::flashWhite(uint16_t durationMs) {
  isFlashingWhite = true;
  flashWhiteEndTime = millis() + durationMs;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(255, 255, 255);
  }
  FastLED.show();
}

void HardwareManager::blink(uint32_t color, int count) {
  if (blinkState == BLINK_IDLE) {
    DEBUG_PRINT(DEBUG_INFO, "Initiating non-blocking blink for color %lx, count %d.", (unsigned long)color, count);
    blinkColor = color;
    blinkCountRemaining = count * 2;
    blinkState = BLINK_ON;
    blinkTimer = millis();
    setLight(blinkColor);
  }
}

void HardwareManager::rainbowBlink(int count, int delayMs) {
  if (rainbowBlinkState == RAINBOW_BLINK_IDLE) {
    DEBUG_PRINT(DEBUG_INFO, "Initiating non-blocking rainbow blink for %d cycles with %dms delay.", count, delayMs);
    rainbowBlinkCountRemaining = count;
    rainbowBlinkDelay = delayMs;
    rainbowBlinkCurrentColorIndex = 0;
    rainbowBlinkState = RAINBOW_BLINK_CYCLE;
    rainbowBlinkTimer = millis();
    setLight(rainbowColors[rainbowBlinkCurrentColorIndex]);
  }
}

void HardwareManager::rainbowAnimation() {
  static unsigned long lastUpdate = 0;
  static uint8_t hue = 0;

  if (millis() - lastUpdate > 20) {
    lastUpdate = millis();
    fill_rainbow(leds, NUM_LEDS, hue, 7);
    FastLED.show();
    hue++;
    if (hue >= 255) hue = 0;
  }
}

int HardwareManager::getDistance() {
  if (!hasSensor) {
    return 8192;
  }

  // Échantillonnage rapide à 50Hz (20ms)
  if (millis() - lastSensorRead < 15) {
    return lastStoredDistance;
  }

  lastSensorRead = millis();
  if (lox.isRangeComplete()) {
    int dist = lox.readRange();
    if (dist < 8192) {
      lastStoredDistance = dist;
    } else {
      lastStoredDistance = 8192;
    }
    return lastStoredDistance;
  }
  return lastStoredDistance;
}

String HardwareManager::getSensorDebugString() {
  if (!hasSensor) {
    return "ERREUR (Capteur non détecté)";
  }
  if (lastStoredDistance >= 8000) {
    return "Hors portée (rien devant)";
  }
  String str = String(lastStoredDistance) + " mm";
  if (isHandPresent) {
    str += " [MAIN DETECTEE - Zone active <= " + String(TOF_TRIGGER_DISTANCE) + "mm]";
  } else {
    str += " [Zone libre]";
  }
  return str;
}

void HardwareManager::goToDeepSleep() {
  DEBUG_PRINT(DEBUG_INFO, "Isolation des GPIOs et passage en Deep Sleep...");
  
  // 1. Eteindre les LEDs et isoler la ligne de donnees pour eviter les fuites parasites
  setLight(0);
  FastLED.show();
  pinMode(LED_PIN, INPUT_PULLDOWN);

  // 2. Cloturer le bus I2C et isoler SDA/SCL pour eviter les fuites dans les pull-ups du capteur ToF
  Wire.end();
  pinMode(SCL_PIN, INPUT_PULLDOWN);
  pinMode(SDA_PIN, INPUT_PULLDOWN);

  // 3. Configurer le reveil sur front montant (D2 passe a HIGH quand on rallume SW1)
  esp_deep_sleep_enable_gpio_wakeup(1ULL << SW_PIN, ESP_GPIO_WAKEUP_GPIO_HIGH);

  // 4. Extinction ultra-basse consommation (~10uA)
  esp_deep_sleep_start();
}

void HardwareManager::checkPowerSwitch() {
  // Si l'interrupteur mécanique est sur OFF (3V3_S = 0V, SW_PIN D2 = LOW)
  // ET que l'USB n'est pas branché (permet le flash et debug sur PC)
  if (digitalRead(SW_PIN) == LOW && !isCharging()) {
    DEBUG_PRINT(DEBUG_INFO, "Interrupteur OFF detecte et pas d'USB.");
    goToDeepSleep();
  }
}

void HardwareManager::checkSleep() {
#if ENABLE_BATTERY_MANAGEMENT
  // Coupure securite si batterie critique (<3.0V) et pas en charge
  if (getBatteryPercentage() < CRITICAL_BATTERY_PERCENTAGE && !isCharging() && getBatteryVoltage() > USB_POWERED_VOLTAGE_THRESHOLD) {
    DEBUG_PRINT(DEBUG_ERROR, "NIVEAU BATTERIE CRITIQUE (%d%%)! Mise en veille profonde.", getBatteryPercentage());
    blink(0xFF0000, 3);
    goToDeepSleep();
  }
#endif

  // Mise en veille pour inactivite
#if ENABLE_BATTERY_MANAGEMENT
  if (millis() - lastActivityTime > INACTIVITY_TIMEOUT && getBatteryVoltage() > USB_POWERED_VOLTAGE_THRESHOLD) {
#else
  if (millis() - lastActivityTime > INACTIVITY_TIMEOUT) {
#endif
    DEBUG_PRINT(DEBUG_INFO, "Inactivite detectee (%lu ms). Mise en veille profonde.", millis() - lastActivityTime);
    blink(0xFF0000, 2);
    goToDeepSleep();
  }
}

// ---- GESTION DE BATTERIE ----
#if ENABLE_BATTERY_MANAGEMENT
float HardwareManager::getBatteryVoltage() {
  if (millis() - lastBatteryCheckTime > 2000 || lastBatteryCheckTime == 0) {
    lastBatteryCheckTime = millis();
    
    // Lecture calibree en millivolts via les courbes d'usine eFuse de l'ESP32-C3
    uint32_t mv = analogReadMilliVolts(BAT_ADC_PIN);
    float adc_voltage = mv / 1000.0f;

    // Calcul de la tension reelle batterie (Vbatt = V_adc / 0.5)
    batteryVoltage = adc_voltage / VOLTAGE_DIVIDER_RATIO;

    DEBUG_PRINT(DEBUG_VERBOSE, "Batterie ADC: %dmV, Tension calculee: %.2fV", mv, batteryVoltage);
  }
  return batteryVoltage;
}

int HardwareManager::getBatteryPercentage() {
  float voltage = getBatteryVoltage();
  int percentage = map(voltage * 100, BAT_VOLT_EMPTY * 100, BAT_VOLT_FULL * 100, 0, 100);
  percentage = constrain(percentage, 0, 100);
  DEBUG_PRINT(DEBUG_VERBOSE, "Batterie: %d%% (%.2fV)", percentage, voltage);
  return percentage;
}

bool HardwareManager::isCharging() {
  // Detection directe et instantanee via la broche CHARGE_PIN (D10 reliee a VBUS 5V)
  return digitalRead(CHARGE_PIN) == HIGH;
}

bool HardwareManager::isLowBattery() {
  bool lowBatt = (getBatteryVoltage() < BAT_VOLT_LOW) && !isCharging();
  if (lowBatt) {
    DEBUG_PRINT(DEBUG_WARNING, "Batterie Faible detectee! (%.2fV)", batteryVoltage);
  }
  return lowBatt;
}

void HardwareManager::updateBatteryLED() {
  if (blinkState != BLINK_IDLE || rainbowBlinkState != RAINBOW_BLINK_IDLE || isFlashingWhite) {
    return;
  }

  // Ne pas écraser les LEDs si une cible est active ou si le jeu est en cours
  // La mise à jour de la LED de charge est réservée aux états inactifs / menu
  if (isCharging()) {
    // Si l'utilisateur est en charge sur USB, on n'écrase pas systématiquement les LEDs
    // pour permettre de voir les couleurs du jeu et les tests
  } else if (isLowBattery()) {
    // Si batterie faible (< 3.4V) hors USB, signalement visuel périodique
    static unsigned long lastLowBattBlink = 0;
    if (millis() - lastLowBattBlink > 10000) {
      lastLowBattBlink = millis();
      blink(0xFF0000, 2);
    }
  }
}

String HardwareManager::getBatteryDebugString() {
  bool usb = isCharging(); // digitalRead(CHARGE_PIN) == HIGH
  float v = getBatteryVoltage();
  int pct = getBatteryPercentage();

  // Si tension inférieure à 1.5V, aucune batterie LiPo n'est connectée
  bool battConnected = (v >= 1.5f);

  String str = "";
  if (!battConnected) {
    if (usb) {
      str = "Non connectee [Alimente UNIQUEMENT par USB]";
    } else {
      str = "Non detectee (0.00V)";
    }
  } else {
    // Batterie présente
    str = String(v, 2) + "V (" + String(pct) + "%)";
    if (usb) {
      if (pct >= 98 || v >= 4.18f) {
        str += " [Connectee - Charge terminee (Alimente par USB)]";
      } else {
        str += " [Connectee - EN CHARGE via USB]";
      }
    } else {
      if (v < BAT_VOLT_LOW) {
        str += " [Connectee - Sur Batterie - /!\\ BATTERIE FAIBLE]";
      } else {
        str += " [Connectee - Sur Batterie]";
      }
    }
  }
  return str;
}

void HardwareManager::recordBatterySample() {
  BatterySample sample;
  sample.timestampSec = millis() / 1000;
  sample.percentage = getBatteryPercentage();
  sample.voltage = getBatteryVoltage();
  sample.isCharging = isCharging();

  batteryHistory.push_back(sample);
  if (batteryHistory.size() > 40) {
    batteryHistory.erase(batteryHistory.begin());
  }
}
#endif // ENABLE_BATTERY_MANAGEMENT

void HardwareManager::update() {
  processNonBlockingLEDAnimations();
  
  int dist = getDistance();
  unsigned long now = millis();

  // Reset des déclencheurs one-shot à chaque itération
  clearAllTriggers();

  // Échantillonnage périodique de la batterie (toutes les 45s ou si l'état USB change)
#if ENABLE_BATTERY_MANAGEMENT
  static bool lastUsbState = false;
  bool currentUsb = isCharging();
  if (millis() - lastBatterySampleTime >= 45000 || lastBatterySampleTime == 0 || (currentUsb != lastUsbState)) {
    lastBatterySampleTime = millis();
    lastUsbState = currentUsb;
    recordBatterySample();
  }
#endif

  // Détection de présence dans la zone de déclenchement (seuil élargi à 150mm)
  bool handDetectedNow = (dist > 0 && dist <= TOF_TRIGGER_DISTANCE);

  if (handDetectedNow) {
    resetActivityTimer();
    
    // Détection de Hit instantané avec délai réfractaire et hystérésis
    if (hitArmed && (now - lastHitTriggerTime > 150)) {
      triggerHit = true;
      lastHitTriggerTime = now;
      hitArmed = false;
    }

    if (!isHandPresent) {
      // Nouvelle détection ou reprise après micro-coupure
      if (handAbsenceStartTime > 0 && (now - handAbsenceStartTime <= GESTURE_GLITCH_TOLERANCE_MS)) {
        // Micro-coupure optique tolérée : on ne réinitialise pas le temps de début !
      } else {
        handPresenceStartTime = now;
        startHoldTriggered = false;
        menuHoldTriggered = false;
      }
      isHandPresent = true;
      handAbsenceStartTime = 0;
    }

    unsigned long heldDuration = now - handPresenceStartTime;

    // Calcul de la progression du maintien (0.0 -> 1.0)
    holdProgress = constrain((float)heldDuration / (float)GESTURE_START_HOLD_MS, 0.0f, 1.0f);

    // Déclenchement du Start Hold (1.5s en Menu)
    if (heldDuration >= GESTURE_START_HOLD_MS && !startHoldTriggered) {
      triggerStartHold = true;
      startHoldTriggered = true;
      DEBUG_PRINT(DEBUG_INFO, "Gesture: START HOLD triggered (1.5s).");
    }

    // Déclenchement du Menu Hold (3.0s en Jeu)
    if (heldDuration >= GESTURE_MENU_HOLD_MS && !menuHoldTriggered) {
      triggerMenuHold = true;
      menuHoldTriggered = true;
      DEBUG_PRINT(DEBUG_INFO, "Gesture: MENU HOLD triggered (3.0s).");
    }

  } else {
    // La main n'est plus détectée dans la zone de déclenchement
    if (dist > TOF_TRIGGER_DISTANCE + 20 || dist >= 8000) {
      hitArmed = true; // Réarmement du hit dès le retrait franc de la main
    }

    if (isHandPresent) {
      if (handAbsenceStartTime == 0) {
        handAbsenceStartTime = now;
      }

      // Si l'absence dépasse la tolérance de micro-coupure, on valide le retrait
      if (now - handAbsenceStartTime > GESTURE_GLITCH_TOLERANCE_MS) {
        unsigned long totalPresenceTime = handAbsenceStartTime - handPresenceStartTime;

        // Détection d'un Tap court (entre 40ms et 500ms, et si aucun Hold n'a été déclenché)
        if (totalPresenceTime >= 40 && totalPresenceTime <= GESTURE_TAP_MAX_MS && !startHoldTriggered) {
          triggerTap = true;
          DEBUG_PRINT(DEBUG_INFO, "Gesture: TAP detected (%lu ms).", totalPresenceTime);
        }

        // Réinitialisation de l'état
        isHandPresent = false;
        handPresenceStartTime = 0;
        handAbsenceStartTime = 0;
        startHoldTriggered = false;
        menuHoldTriggered = false;
        holdProgress = 0.0f;
      }
    }
  }
}

// Helper function to manage non-blocking LED animations
void HardwareManager::processNonBlockingLEDAnimations() {
  unsigned long now = millis();

  // --- Process non-blocking hit flash ---
  if (isFlashingWhite) {
    if (now >= flashWhiteEndTime) {
      isFlashingWhite = false;
      setLight(0); // Éteindre après le flash
    }
    return; // Priorité au flash
  }

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