#include <Adafruit_CircuitPlayground.h>
// do NOT include the standard NeoPixel library

#define NEOPIX_PIN    A2
#define NUM_PIXELS    5

int i = 0;

void setup() {
  // initialize the Circuit Playground as usual
  // this will initialize the onboard NeoPixels as well
  CircuitPlayground.begin();
}

void loop() {
  // for the on board NeoPixels, use the CircuitPlayground functions
  CircuitPlayground.clearPixels();

  CircuitPlayground.setPixelColor(i, 255,   0,   0);
  
  i = ++i % 10;

  delay(500);
}
