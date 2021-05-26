#include <Arduino.h>
#include <wiring_private.h>
#include <CFXSerial.hpp>

#include "helpers/VectorHelpers.h"

#include <string>
#include <queue>

Uart Serial2( &sercom1, PIN_SERIAL2_RX, PIN_SERIAL2_TX, PAD_SERIAL2_RX, PAD_SERIAL2_TX ) ;

std::queue<int> rxBuffer;
std::queue<int> txBuffer;

void SERCOM1_Handler()
{
  Serial2.IrqHandler();
  rxBuffer.push(Serial2.read());
}

void send_serial() {
  while(!txBuffer.empty()) {
    Serial2.write(txBuffer.front());
    txBuffer.pop();
  }
}

void setUpSerialPort()
{
    Serial2.begin(9600, SERIAL_8N2);
    pinPeripheral(PIN_SERIAL2_RX, PIO_SERCOM_ALT);
    pinPeripheral(PIN_SERIAL2_TX, PIO_SERCOM_ALT);
}

/* -------- CFXSerial member functions -------- */

void CFXSerial::init() {
}

int CFXSerial::receivePacket() {
  // Receive entire packet
  unsigned long serialTimer = millis();
  unsigned long timeoutDuration = 250;
  std::vector<int> buffer;

  while (millis() < serialTimer + timeoutDuration) {
    if(!rxBuffer.empty())
    {
      buffer.push_back(rxBuffer.front());
      rxBuffer.pop();
      serialTimer = millis();
    }
    else
    {
      delay(10);
    }
  }
  if(buffer.size() == 0) {
    Serial.println("No packet received!");
    return -1;
  } else {
    Serial.println("Timed out! We have a packet");
    Serial.print("Buf len: ");
    Serial.println((int)buffer.size());
  }

  // Check to see if the packet is valid and return a struct?
  return 0;
}

void CFXSerial::sendByte(char txByte) {
  txBuffer.push(txByte);
  send_serial();
}
