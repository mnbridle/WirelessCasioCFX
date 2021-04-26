// Imports for redefining serial ports
#include <Arduino.h>
#include "wiring_private.h"

// Imports for onboard LED
#include <Adafruit_DotStar.h>

// Imports for radio
#include <RadioHead.h>
#include <RH_RF24.h>

// DotStar defines
#define NUMPIXELS 1
#define DATAPIN 7
#define CLOCKPIN 8

// Define some colours
#define RED   0xFF0000
#define GREEN 0x00FF00
#define YELLOW 0xFFFF00
#define BLUE  0x0000FF

// Define IO pins
#define DRF4463_SDN 0
#define DRF4463_nIRQ 1
#define DRF4463_SDO 2
#define DRF4463_SCLK 3
#define DRF4463_SDI 4
#define DRF4463_nSEL 13
#define CASIO_TX 20
#define CASIO_RX 19

// Define serial interface
#define SERIAL_RX CASIO_TX
#define SERIAL_TX CASIO_RX

// Hardware setup
Adafruit_DotStar strip = Adafruit_DotStar(
  NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);
  
RH_RF24 drf4463(DRF4463_nSEL, DRF4463_nIRQ, DRF4463_SDN);

void setLEDState(unsigned long pixelColor) {
  strip.setPixelColor(0, pixelColor);
  strip.show();
}

void clearLED() {
  strip.setPixelColor(0, 0x000000);
  strip.show();
}

void getRadioStatus(){
  float batteryVoltage = drf4463.get_battery_voltage();
  float temperature = drf4463.get_temperature();
  int16_t lastRssi = drf4463.lastRssi();
  Serial.print("Last RSSI: ");
  Serial.print(lastRssi);
  Serial.println("dBm");
  Serial.print("Battery voltage: ");
  Serial.print(batteryVoltage);
  Serial.print("V, temperature: ");
  Serial.print(temperature);
  Serial.println("'C");
}

// TESTS

void runSerialPortEchoTest() {
  // Serial port echo test
  while(1) {
    // Serial2.write("U");
    if (Serial.available() > 0) {
      Serial2.write(Serial.read());
    }
    if (Serial2.available() > 0) {
      Serial.println("Character(s) received!");
      Serial.write(Serial2.read());
    }
  }
}

bool setupRadio() {
    return drf4463.init();
}

void setupCasioSerialPort() {
  Serial.println("Initialising serial port...");
  Serial2.begin(9600, SERIAL_8N2);
  
  // Receive
  pinPeripheral(SERIAL_RX, PIO_SERCOM_ALT);
  // Transmit
  pinPeripheral(SERIAL_TX, PIO_SERCOM_ALT);
}

void setup() {
  // Set up DotStar LED
  strip.begin();
  clearLED();
  setLEDState(RED);
  
  // Set up serial port
  setupCasioSerialPort();
  
  // Set up radio
  if(setupRadio()) {
    setLEDState(YELLOW);
    drf4463.setModeRx();
  } else {
    setLEDState(RED);
    Serial.println("Error initialising DRF4463 radio. Rebooting...");
  }

  delay(3000);
  setLEDState(GREEN);
}

void loop() {  
  clearLED();
  
  getRadioStatus();
  runSerialPortEchoTest();
}

