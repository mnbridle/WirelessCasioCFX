#include "LEDHelpers.hpp"

// Hardware setup
Adafruit_DotStar strip = Adafruit_DotStar(
  NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);

void ledInit() {
    strip.begin();
}
  
void setLEDState(unsigned long pixelColor) {
  strip.setPixelColor(0, pixelColor);
  strip.show();
}

void clearLED() {
  strip.setPixelColor(0, 0x000000);
  strip.show();
}