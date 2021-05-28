// Imports for radio
#include <CFXSerial.hpp>
#include <RadioHelpers.hpp>
#include <LEDHelpers.hpp>
#include <process_rx.h>

CFXSerial cfxSerial;

void setup() {
  // Set up DotStar LED
  ledInit();
  clearLED();
  setLEDState(RED);
  
  // Set up serial port
  Serial.println("Initialising serial port...");
  setUpSerialPort();
  
  // Set up radio
  if(setupRadio()) {
    setLEDState(YELLOW);
    setModeRx();
  } else {
    setLEDState(RED);
    Serial.println("Error initialising DRF4463 radio. Rebooting...");
  }

  delay(3000);
  setLEDState(GREEN);
}

void loop() {  
  clearLED();
  setLEDState(GREEN);

  main_processor(cfxSerial);

  // getRadioStatus();
  // delay(1000);
  // clearLED();
  // delay(1000);
}

