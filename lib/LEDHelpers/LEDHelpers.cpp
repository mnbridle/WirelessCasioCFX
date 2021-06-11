#include "LEDHelpers.hpp"

Adafruit_DotStar strip = Adafruit_DotStar(
  NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);

void ledInit() {
  // Hardware setup
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