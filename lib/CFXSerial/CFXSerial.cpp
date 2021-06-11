#include <Arduino.h>
#include <wiring_private.h>
#include <string>
#include <queue>

#include <CFXSerial.hpp>
#include "helpers/VectorHelpers.h"

Uart Serial2( &sercom1, PIN_SERIAL2_RX, PIN_SERIAL2_TX, PAD_SERIAL2_RX, PAD_SERIAL2_TX ) ;

std::queue<int> txBuffer;

void SERCOM1_Handler()
{
  Serial2.IrqHandler();
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
  current_state = States::IDLE;
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
    if(Serial2.available())
    {
      buffer[i] = Serial2.read();
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
    packet_type = PacketType::UNSUPPORTED;
    size = i;
    return false;
  }

  // Store packet data in class
  // Determine packet type
  packet_type = PacketCodec().getPacketType(buffer, i);
  size = i;
  
  return true;
}

void CFXSerial::sendByte(uint8_t txByte) 
{
  txBuffer.push(txByte);
  send_serial();
}

void CFXSerial::sendBuffer(uint8_t* buffer, size_t size)
{
  Serial.println("Writing buffer");
  for(size_t i=0; i<size; i++)
  {
    txBuffer.push(buffer[i]);
    Serial.print(buffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println("");
  Serial.println("Sending...");
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

// ---------------- State machine methods ----------------

bool CFXSerial::execute_current_state() {
  bool isSuccessful = false;

    switch(current_state) {
        case States::IDLE :
        {
            Serial.println("IDLE");
            isSuccessful = state_IDLE();
            break;
        }
        case States::RECEIVE_SCREENSHOT_DATA :
            Serial.println("RECEIVE_SCREENSHOT_DATA");
            isSuccessful = state_RECEIVE_SCREENSHOT_DATA();
            break;
        case States::RECEIVE_END_PACKET :
            Serial.println("RECEIVE_END_PACKET");
            isSuccessful = state_RECEIVE_END_PACKET();
            break;
        case States::RECEIVE_VALUE_PACKET :
            Serial.println("RECEIVE_VALUE_PACKET");
            isSuccessful = state_RECEIVE_VALUE_PACKET();
            break;
        case States::SEND_END_PACKET :
            Serial.println("SEND_END_PACKET");
            isSuccessful = state_SEND_END_PACKET();
            break;
        case States::SEND_VALUE_PACKET :
            Serial.println("SEND_VALUE_PACKET");
            isSuccessful = state_SEND_VALUE_PACKET();
            break;
        case States::SEND_VARIABLE_DESCRIPTION_PACKET :
            Serial.println("SEND_VARIABLE_DESCRIPTION_PACKET");
            isSuccessful = state_SEND_VARIABLE_DESCRIPTION_PACKET();
            break;
        case States::WAIT_FOR_DATA_REQUEST :
            Serial.println("WAIT_FOR_DATA_REQUEST");
            isSuccessful = state_WAIT_FOR_DATA_REQUEST();
            break;
        case States::WAIT_FOR_SCREENSHOT_REQUEST :
            Serial.println("WAIT_FOR_SCREENSHOT_REQUEST");
            isSuccessful = state_WAIT_FOR_SCREENSHOT_REQUEST();
            break;
        case States::WAIT_FOR_ACK :
            Serial.println("WAIT_FOR_ACK");
            isSuccessful = state_WAIT_FOR_ACK();
            break;
        default:
            Serial.println("Unknown state!");
            Serial.println((uint8_t)current_state);
            break;
    }
    return isSuccessful;
}

bool CFXSerial::state_IDLE()
{
  bool packetReceived = false;
  packetReceived = receivePacket();
  if(!packetReceived)
  {
    return false;
  }

  // To leave IDLE state, we need a wakeup
  if(packet_type == PacketType::WAKE_UP)
  {
    // Which kind of wakeup?
    WakeUp decodedPacket = WakeUpPacket().decode(buffer, size);
    if(!decodedPacket.isValid)
    {
      Serial.println("WakeUpPacket was not valid");
      return false;
    }

    sendWakeUpAck();

    if(decodedPacket.wakeUpType == WakeUpByte::DATA)
    {
      current_state = States::WAIT_FOR_DATA_REQUEST;
    }
    else if(decodedPacket.wakeUpType == WakeUpByte::SCREENSHOT)
    {
      current_state = States::WAIT_FOR_SCREENSHOT_REQUEST;
    }
    else
    {
      Serial.println("Invalid wakeup!");
      return false;
    }
  }
  else
  {
    return false;
  }
  
  return true;
}

bool CFXSerial::state_RECEIVE_SCREENSHOT_DATA()
{
  Serial.println("Not implemented");
  return false;
}

bool CFXSerial::state_RECEIVE_END_PACKET()
{
  Serial.println("Not implemented");
  return false;
}

bool CFXSerial::state_RECEIVE_VALUE_PACKET()
{
  Serial.println("Not implemented");
  return false;
}

bool CFXSerial::state_SEND_END_PACKET()
{
  Serial.println("Not implemented");
  return false;
}

bool CFXSerial::state_SEND_VALUE_PACKET()
{
  bool packetReceived = false;
  Value packetToEncode;

  packetToEncode.row = 1;
  packetToEncode.col = 1;
  packetToEncode.value = 1.23456789012345;
  packetToEncode.isValid = true;

  sendBuffer(ValuePacket().encode(packetToEncode), 16);

  current_state = States::IDLE;

  return true;
}

bool CFXSerial::state_SEND_VARIABLE_DESCRIPTION_PACKET()
{
  bool packetReceived = false;
  VariableDescription packetToEncode;

  // Copy bits from data request to packetToEncode
  packetToEncode.variableType = data_request.variableType;
  packetToEncode.variableName = data_request.variableName;
  packetToEncode.variableInUse = true;
  packetToEncode.isComplex = false;

  sendBuffer(VariableDescriptionPacket().encode(packetToEncode), 50);

  // Just transition to SEND_VALUE_PACKET
  // Clear anything bogus that's been received into the buffer
  packetReceived = receivePacket();

  Serial.println("Acked OK, changing state to SEND_VALUE_PACKET");
  current_state = States::SEND_VALUE_PACKET;

  return true;
}

bool CFXSerial::state_WAIT_FOR_DATA_REQUEST()
{
  bool packetReceived = false;
  packetReceived = receivePacket();
  if(!packetReceived)
  {
    return false;
  }

  // To leave WAIT_FOR_DATA_REQUEST state, we need either a request packet or a variable description packet
  if(packet_type == PacketType::REQUEST)
  {
    // Which kind of wakeup?
    data_request = RequestPacket().decode(buffer, size);
    if(!data_request.isValid)
    {
      Serial.println("Packet was not valid");
      return false;
    }
    sendDataAck();
    current_state = States::WAIT_FOR_ACK;
  }
  else if(packet_type == PacketType::VARIABLE_DESCRIPTION)
  {
    VariableDescription decodedPacket = VariableDescriptionPacket().decode(buffer, size);
    if(!decodedPacket.isValid)
    {
      Serial.println("Packet was not valid");
      return false;
    }
  }
  else
  {
    return false;
  }
  return true;
}

bool CFXSerial::state_WAIT_FOR_SCREENSHOT_REQUEST()
{
  Serial.println("Not implemented");
  return false;
}

bool CFXSerial::state_WAIT_FOR_ACK()
{
  bool packetReceived = false;
  packetReceived = receivePacket();
  if(!packetReceived)
  {
    return false;
  }

  // To leave WAIT_FOR_ACK state, we need an Ack packet
  if(packet_type != PacketType::ACK)
  {
    return false;
  }

  Ack decodedPacket = AckPacket().decode(buffer, size);
  if(!decodedPacket.isValid)
  {
    return false;
  }

  current_state = States::SEND_VARIABLE_DESCRIPTION_PACKET;
  return true;
}