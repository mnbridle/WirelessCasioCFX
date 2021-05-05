#include <Arduino.h>
#include <wiring_private.h>
#include <CFXSerial.hpp>

Uart Serial2( &sercom1, PIN_SERIAL2_RX, PIN_SERIAL2_TX, PAD_SERIAL2_RX, PAD_SERIAL2_TX ) ;

void setUpSerialPort()
{
    Serial2.begin(9600, SERIAL_8N2);
    pinPeripheral(PIN_SERIAL2_RX, PIO_SERCOM_ALT);
    pinPeripheral(PIN_SERIAL2_TX, PIO_SERCOM_ALT);
}

void SERCOM1_Handler()
{
  Serial2.IrqHandler();
  Serial.println("Character in buffer");
}

void runSerialPortEchoTest() {
  // Serial port echo test
  while(1) {
    if (Serial.available() > 0) {
      Serial2.write(Serial.read());
    }
    if (Serial2.available() > 0) {
      Serial.write(Serial2.read());
    }
  }
}

void hello_world() {
  Serial2.println("Hello world!");
}