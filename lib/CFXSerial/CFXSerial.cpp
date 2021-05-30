#include <Arduino.h>
#include <wiring_private.h>
#include <string>
#include <queue>

#include <CFXSerial.hpp>
#include "helpers/VectorHelpers.h"
#include "state_machine/Transitions.h"

Uart Serial2( &sercom1, PIN_SERIAL2_RX, PIN_SERIAL2_TX, PAD_SERIAL2_RX, PAD_SERIAL2_TX ) ;

std::queue<int> rxBuffer;
std::queue<int> txBuffer;

void SERCOM1_Handler()
{
  Serial2.IrqHandler();
  rxBuffer.push(Serial2.read());
}

void send_serial() 
{
  while(!txBuffer.empty()) 
  {
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

CFXSerial::CFXSerial(void)
{
}

CFXSerial::~CFXSerial(void)
{
}

bool CFXSerial::receivePacket()
{
  // Receive entire packet
  unsigned long serialTimer = millis();
  unsigned long timeoutDuration = 250;

  size_t i = 0;

  while (millis() < serialTimer + timeoutDuration)
  {
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

  if(i == 0)
  {
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

bool CFXSerial::process_transaction()
{
  bool isSuccessful = false;
  Serial.println("Processing transaction");

  if(!receivePacket())
  {
    return false;
  }

  // Decide what to do with packet
  if(packet_type == UNSUPPORTED)
  {
    return false;
  }

  if(packet_type == WAKE_UP)
  {
    // Decode wakeup and decide what to do
    WakeUp decodedPacket = WakeUpPacket().decode(buffer, size);
    
    if(decodedPacket.wakeUpType == DATA)
    {
      Serial.println("Trigger RECEIVED_DATA_WAKE_UP");
      //isSuccessful = transaction.transition(RECEIVED_DATA_WAKE_UP);
      transaction.trigger();
      Serial.println("Did the thing");
    } 
    else if(decodedPacket.wakeUpType == SCREENSHOT)
    {
      Serial.println("Trigger RECEIVED_SCREENSHOT_WAKE_UP");
      //isSuccessful = transaction.transition(RECEIVED_SCREENSHOT_WAKE_UP);
      transaction.trigger();
      Serial.println("Did the thing");
    } else {
      Serial.println("Unsuccessful!");
    }
  }

  Serial.print("Transition successful: ");
  Serial.println(isSuccessful);

  return true;

  // if(packet_type == REQUEST)
  // {
  //   Request decodedPacket = RequestPacket().decode(buffer, size);
  //   transaction.transition(RECEIVED_REQUEST_PACKET);
  // }
}

void CFXSerial::sendByte(uint8_t txByte) 
{
  txBuffer.push(txByte);
  send_serial();
}

void CFXSerial::sendWakeUpAck()
{
  sendByte(0x13);
}

void CFXSerial::sendDataAck()
{
  sendByte(0x06);
}