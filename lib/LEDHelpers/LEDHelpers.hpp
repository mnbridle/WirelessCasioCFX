// Imports for onboard LED
#include <Adafruit_DotStar.h>

// DotStar defines
#define NUMPIXELS 1
#define DATAPIN 7
#define CLOCKPIN 8

// Define some colours
#define RED   0xFF0000
#define GREEN 0x00FF00
#define YELLOW 0xFFFF00
#define BLUE  0x0000FF

void setLEDState(unsigned long);
void clearLED();
void ledInit();