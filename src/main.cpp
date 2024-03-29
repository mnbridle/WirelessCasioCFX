#include "main.h"

// CFXSerial cfxSerial;

void setup() {
  // Set up DotStar LED
  ledInit();
  clearLED();
  setLEDState(RED);
  
  // Set up serial port
  Serial.begin(9600, SERIAL_8N2);
  // setUpSerialPort();

  // cfxSerial.init();
  
  // // Serial.println("Setting up radio");
  // // Set up radio
  // if(setupRadio()) {
  //   setLEDState(YELLOW);
  //   setModeRx();
  // } else {
  //   setLEDState(RED);
  //   Serial.println("Error initialising DRF4463 radio. Rebooting...");
  // }
  // Serial.println("Setup complete!");
  setLEDState(GREEN);
}

void loop() {  
  clearLED();
  setLEDState(GREEN);

  CasioMessageProcessor app = CasioMessageProcessor();
  app.run();

  // main_processor(cfxSerial);
}

