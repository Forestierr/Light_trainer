#include <Adafruit_CircuitPlayground.h>

uint8_t offset = 0; // Décalage pour créer l'effet de rotation

void setup() {
  CircuitPlayground.begin();

  // Éteint toutes les LED au départ
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 0, 0, 0);
  }

  // Désactive le haut-parleur pour économiser l'énergie
  CircuitPlayground.speaker.enable(false);
}

void loop() {
  // Met à jour les couleurs pour l'arc-en-ciel
  for (int i = 0; i < 10; i++) {
    // Utilisation de colorWheel pour des couleurs de l'arc-en-ciel
    uint32_t color = CircuitPlayground.colorWheel((i * 25 + offset) % 255);
    CircuitPlayground.setPixelColor(i, color);
  }

  // Décale l'arc-en-ciel pour l'effet tournant
  offset = (offset + 5) % 255;

  // Petite pause pour ajuster la vitesse de rotation
  delay(100);
}
