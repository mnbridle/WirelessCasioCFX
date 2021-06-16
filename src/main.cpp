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
  Serial.begin(9600, SERIAL_8N2);
  setUpSerialPort();
  
  // Serial.println("Setting up radio");
  // Set up radio
  if(setupRadio()) {
    setLEDState(YELLOW);
    setModeRx();
  } else {
    setLEDState(RED);
    Serial.println("Error initialising DRF4463 radio. Rebooting...");
  }
  // Serial.println("Setup complete!");
  setLEDState(GREEN);
}

void loop() {  
  clearLED();
  setLEDState(GREEN);

  main_processor(cfxSerial);
}

