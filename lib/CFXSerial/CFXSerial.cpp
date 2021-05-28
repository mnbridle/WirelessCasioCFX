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

bool CFXSerial::receivePacket() {
  // Receive entire packet
  unsigned long serialTimer = millis();
  unsigned long timeoutDuration = 250;

  size_t i = 0;

  while (millis() < serialTimer + timeoutDuration) {
    if(!rxBuffer.empty())
    {
      buffer[i] = rxBuffer.front();
      rxBuffer.pop();
      serialTimer = millis();
      i++;
    }
    else
    {
      delay(10);
    }
  }

  if(i == 0) {
    packet_type = UNSUPPORTED;
    size = i;
    return false;
  }

  // Store packet data in class
  // Determine packet type
  packet_type = PacketCodec().getPacketType(buffer, i);
  size = i;
  
  return true;
}

void CFXSerial::sendByte(uint8_t txByte) {
  txBuffer.push(txByte);
  send_serial();
}

void CFXSerial::sendWakeUpAck() {
  sendByte(0x13);
}

void CFXSerial::sendDataAck() {
  sendByte(0x06);
}