#include <Arduino.h>
#include <wiring_private.h>
#include <string>
#include <queue>

#include <CFXSerial.hpp>
#include "helpers/VectorHelpers.h"

Uart Serial2( &sercom1, PIN_SERIAL2_RX, PIN_SERIAL2_TX, PAD_SERIAL2_RX, PAD_SERIAL2_TX ) ;

void SERCOM1_Handler()
{
  Serial2.IrqHandler();
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
  go_to_idle_state();
  ComplexValue default_value;
  default_value.isComplex = true;
  default_value.real_part = 0.42424242424242;
  default_value.imag_part = 0.21212121212121;
  
  variable_memory.set('A', default_value);
}

CFXSerial::~CFXSerial(void)
{
}

bool CFXSerial::receivePacket()
{
  // Receive entire packet
  unsigned long serialTimer = millis();
  unsigned long timeoutDuration = 100;
  unsigned long start = millis();

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
      delay(5);
    }

    if(i==1 && !(buffer[0]==':'))
    {
      // It's an acknowledgement or wakeup
      break;
    }

    if(i==50)
    {
      // No more data!
      break;
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
  Serial2.write(txByte);
}

void CFXSerial::sendBuffer(uint8_t* buffer, size_t size)
{
  for(size_t i=0; i<size; i++)
  {
    Serial2.write(buffer[i]);
  }
}

void CFXSerial::sendWakeUpAck()
{
  delay(10);
  sendByte(0x13);
}

void CFXSerial::sendDataAck()
{
  // Delay so that we don't ACK too quickly
  delay(10);
  sendByte(0x06);
}

// ---------------- State machine methods ----------------

bool CFXSerial::execute_current_state() {
  bool isSuccessful = false;

  switch(current_state) {
    case States::IDLE :
        isSuccessful = state_IDLE();
        break;
    case States::RECEIVE_SCREENSHOT_DATA :
        isSuccessful = state_RECEIVE_SCREENSHOT_DATA();
        break;
    case States::RECEIVE_END_PACKET :
        isSuccessful = state_RECEIVE_END_PACKET();
        break;
    case States::RECEIVE_VALUE_PACKET :
        isSuccessful = state_RECEIVE_VALUE_PACKET();
        break;
    case States::SEND_END_PACKET :
        isSuccessful = state_SEND_END_PACKET();
        break;
    case States::SEND_VALUE_PACKET :
        isSuccessful = state_SEND_VALUE_PACKET();
        break;
    case States::SEND_VARIABLE_DESCRIPTION_PACKET :
        isSuccessful = state_SEND_VARIABLE_DESCRIPTION_PACKET();
        break;
    case States::WAIT_FOR_DATA_REQUEST :
        isSuccessful = state_WAIT_FOR_DATA_REQUEST();
        break;
    case States::WAIT_FOR_SCREENSHOT_REQUEST :
        isSuccessful = state_WAIT_FOR_SCREENSHOT_REQUEST();
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
  go_to_idle_state();
  return false;
}

bool CFXSerial::state_RECEIVE_END_PACKET()
{
  bool packetReceived = false;
  packetReceived = receivePacket();
  if(!packetReceived)
  {
    go_to_idle_state();
    return false;
  }

  if(packet_type == PacketType::END)
  {
    go_to_idle_state();
    return true;
  }

  go_to_idle_state();
  return false;
}

bool CFXSerial::state_RECEIVE_VALUE_PACKET()
{
  bool packetReceived = false;
  ComplexValue value_packet;
  packetReceived = receivePacket();

  if(!packetReceived)
  {
    go_to_idle_state();
    return false;
  }

  if(packet_type == PacketType::VALUE)
  {
    // decode value packet
    value_packet = ValuePacket().decode(buffer, size);
    sendDataAck();

    // Do something with the received data

    switch(variable_description.variableType)
    {
      case RequestDataType::VARIABLE :
        Serial.println("This was a variable");
        variable_memory.set(variable_description.variableName, value_packet);
        break;
      default:
        Serial.println("Currently unsupported");
        Serial.print("Data type: ");
        Serial.println((uint8_t)variable_description.variableType);
    }

    // Is there more data to come? If so, be prepared to receive another value packet.
    current_state = States::RECEIVE_END_PACKET;
    return true;
  } 

  Serial.println("Invalid type, going to idle");
  go_to_idle_state();
  return false;

}

bool CFXSerial::state_SEND_END_PACKET()
{
  End packetToEncode;
  sendBuffer(EndPacket().encode(packetToEncode), 50);
  go_to_idle_state();
  return true;
}

bool CFXSerial::state_SEND_VALUE_PACKET()
{
  bool packetReceived = false;
  ComplexValue packetToEncode;
  
  packetToEncode = variable_memory.get(data_request.variableName);
  packetToEncode.row = 1;
  packetToEncode.col = 1;
  packetToEncode.isValid = true;

  if(packetToEncode.isComplex)
  {
    sendBuffer(ValuePacket().encode(packetToEncode), 26);
  }
  else
  {
    sendBuffer(ValuePacket().encode(packetToEncode), 16);
  }

  if(!wait_for_ack())
  {
    go_to_idle_state();
    return false;
  }

  current_state = States::SEND_END_PACKET;

  return true;
}

bool CFXSerial::state_SEND_VARIABLE_DESCRIPTION_PACKET()
{
  bool packetReceived = false;
  VariableDescription packetToEncode;

  // Copy bits from data request to packetToEncode
  packetToEncode.variableType = data_request.variableType;
  packetToEncode.variableName = data_request.variableName;

  if(packetToEncode.variableType == RequestDataType::VARIABLE)
  {
    // Get variable from storage
    ComplexValue requested_variable = variable_memory.get(data_request.variableName);
    packetToEncode.variableInUse = true;
    packetToEncode.isComplex = requested_variable.isComplex;
  } else {
    Serial.println("Request currently unsupported!");
    go_to_idle_state();
    return false;
  }

  sendBuffer(VariableDescriptionPacket().encode(packetToEncode), 50);

  // Just transition to SEND_VALUE_PACKET
  // Clear anything bogus that's been received into the buffer
  packetReceived = receivePacket();
  current_state = States::SEND_VALUE_PACKET;

  return true;
}

bool CFXSerial::state_WAIT_FOR_DATA_REQUEST()
{
  bool packetReceived = false;
  packetReceived = receivePacket();
  if(!packetReceived)
  {
    go_to_idle_state();
    return false;
  }

  // To leave WAIT_FOR_DATA_REQUEST state, we need either a request packet or a variable description packet
  if(packet_type == PacketType::REQUEST)
  {
    Serial.println("Received data request");
    // Which kind of wakeup?
    data_request = RequestPacket().decode(buffer, size);
    if(!data_request.isValid)
    {
      go_to_idle_state();
      return false;
    }

    sendDataAck();

    if(!wait_for_ack())
    {
      go_to_idle_state();
      return false;
    }

    current_state = States::SEND_VARIABLE_DESCRIPTION_PACKET;
    return true;
  }

  if(packet_type == PacketType::VARIABLE_DESCRIPTION)
  {
    VariableDescription decodedPacket = VariableDescriptionPacket().decode(buffer, size);
    if(!decodedPacket.isValid)
    {
      go_to_idle_state();
      return false;
    }

    sendDataAck();
    // Store variable description packet
    variable_description = decodedPacket;
    current_state = States::RECEIVE_VALUE_PACKET;
    return true;
  }

  Serial.print("Packet type invalid - ");
  Serial.println((uint8_t)packet_type);

  go_to_idle_state();
  return false;
}

bool CFXSerial::state_WAIT_FOR_SCREENSHOT_REQUEST()
{
  Serial.println("Not implemented");
  go_to_idle_state();
  return false;
}

bool CFXSerial::wait_for_ack()
{
  // not really a state!
  bool packetReceived = false;
  packetReceived = receivePacket();
  if(!packetReceived)
  {
    go_to_idle_state();
    return false;
  }

  // Return false if not an ack
  if(packet_type != PacketType::ACK)
  {
    go_to_idle_state();
    return false;
  }

  Ack decodedPacket = AckPacket().decode(buffer, size);
  if(!decodedPacket.isValid)
  {
    go_to_idle_state();
    return false;
  }

  return true;
}

bool CFXSerial::go_to_idle_state()
{
  current_state = States::IDLE;
  return true;
}