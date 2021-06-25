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
  debugMode = true;
}

CFXSerial::~CFXSerial(void)
{
}

void CFXSerial::init(void)
{
  go_to_idle_state("State machine initialised");
}

void CFXSerial::cfx_software_interface(unsigned long run_time)
{
  unsigned long start_time = millis();

  Serial.begin(9600, SERIAL_8N2);
  while(1)
  {
    if (Serial.available())
    {
      Serial2.write(Serial.read());
    }
    if (Serial2.available())
    {
      Serial.write(Serial2.read());
    }
    
    if (millis() - start_time > (run_time*1000)) 
    {
      break;
    }
  }
}

bool CFXSerial::receivePacket()
{
  // Receive entire packet
  unsigned long serialTimer = millis();
  unsigned long timeoutDuration = 100;

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

    if ((i==1 && !(buffer[0]==':')) || i==50)
    {
      break;
    }
  }

  if(i == 0)
  {
    packet_type = PacketType::UNSUPPORTED;
    size = i;
    return false;
  }

  // Determine packet type
  packet_type = PacketCodec().getPacketType(buffer, i);
  size = i;

  if(debugMode)
  {
    debug_buffer(buffer, size, false);
  }

  return true;
}

void CFXSerial::sendByte(uint8_t txByte) 
{
  uint8_t buffer[1];
  buffer[0] = txByte;

  sendBuffer(buffer, 1);
}

void CFXSerial::sendBuffer(uint8_t* buffer, size_t size)
{
  if(debugMode)
  {
    debug_buffer(buffer, size, true);
  }

  for(size_t i=0; i<size; i++)
  {
    Serial2.write(buffer[i]);
  }
}

void CFXSerial::debug_buffer(uint8_t* buffer, size_t size, bool is_sending)
{
  Serial.print("len=");
  Serial.print(size);
  Serial.print(", ");

  if(is_sending)
  {
    Serial.println("transmitting");
  }
  else
  {
    Serial.println("receiving");
  }

  for(size_t i=0; i<size; i++)
  {
    Serial.print(buffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println("");
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

  if(debugMode && (uint8_t)current_state != 0)
  {
    Serial.print("Current state: ");
    Serial.println((uint8_t)current_state);
  }

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
  go_to_idle_state("receive_screenshot_data() not implemented");
  return false;
}

bool CFXSerial::state_RECEIVE_END_PACKET()
{
  bool packetReceived = false;
  packetReceived = receivePacket();
  if(!packetReceived)
  {
    go_to_idle_state("Packet not received");
    return false;
  }

  if(packet_type == PacketType::END)
  {
    go_to_idle_state("End packet RXed");
    return true;
  }

  go_to_idle_state("");
  return false;
}

bool CFXSerial::state_RECEIVE_VALUE_PACKET()
{
  bool packetReceived = false;
  ComplexValue value_packet;
  packetReceived = receivePacket();

  if(!packetReceived)
  {
    go_to_idle_state("Packet not received");
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
        variable_memory.set(variable_description.variableName, value_packet);
        current_state = States::RECEIVE_END_PACKET;
        return true;
      case RequestDataType::MATRIX :
        matrix_memory.append(variable_description.variableName, value_packet);
        if(debugMode)
        {
          Serial.print("Data in buffer: ");
          Serial.println(matrix_memory.size(variable_description.variableName));
        }
        return true;
      default:
        go_to_idle_state("Unsupported data type");
        return false;
    }
  }

  if(packet_type == PacketType::END)
  {
    go_to_idle_state("Received END, going to idle");
    return true;
  } 

  go_to_idle_state("Unknown error, going to idle");
  return false;
}

bool CFXSerial::state_SEND_END_PACKET()
{
  End packetToEncode;
  sendBuffer(EndPacket().encode(packetToEncode), 50);
  go_to_idle_state("END packet sent");
  return true;
}

bool CFXSerial::state_SEND_VALUE_PACKET()
{
  bool succeeded = false;
  
  if(request_type == RequestDataType::VARIABLE)
  {
    Serial.println("Sending variable");
    succeeded = send_variable();
  }
  else if(request_type == RequestDataType::MATRIX)
  {
    Serial.println("Sending matrix");
    succeeded = send_matrix();
  }
  else
  {
    go_to_idle_state("Unknown request type!");
    return false;
  }

  if(!succeeded)
  {
    go_to_idle_state("Failed to send value packet");
    return false;
  }

  current_state = States::SEND_END_PACKET;

  return true;
}

bool CFXSerial::send_variable()
{
    VariableData packetToEncode = variable_memory.get(data_request.variableName);
    ComplexValue data_to_send = packetToEncode.data;
    data_to_send.row = 1;
    data_to_send.col = 1;
    packetToEncode.isValid = true;

    if(packetToEncode.isComplex)
    {
      sendBuffer(ValuePacket().encode(data_to_send, true), 26);
    }
    else
    {
      sendBuffer(ValuePacket().encode(data_to_send, false), 16);
    }

    return wait_for_ack();
}

bool CFXSerial::send_matrix()
{
  MatrixData matrix_data = matrix_memory.get_all(data_request.variableName);
  int item_count = 0;

  if(debugMode)
  {
    Serial.print("Matrix data is valid: ");
    Serial.println(matrix_data.isValid);
    Serial.print("Matrix size: ");
    Serial.println(matrix_data.matrix_data.size());
  }
  
  for (ComplexValue value : matrix_data.matrix_data) {    
    item_count += 1;

    if(debugMode)
    {
      Serial.print("Item: ");
      Serial.println(item_count);
    }

    if(matrix_data.isComplex)
    {
      sendBuffer(ValuePacket().encode(value, true), 26);
    }
    else
    {
      sendBuffer(ValuePacket().encode(value, false), 16);
    }

    if(!wait_for_ack())
    {
      return false;
    }
  }

  return true;
}

bool CFXSerial::state_SEND_VARIABLE_DESCRIPTION_PACKET()
{
  VariableDescription packetToEncode;

  // Copy bits from data request to packetToEncode
  packetToEncode.variableType = data_request.variableType;
  packetToEncode.variableName = data_request.variableName;

  request_type = packetToEncode.variableType;

  if(packetToEncode.variableType == RequestDataType::VARIABLE)
  {
    // Get variable from storage
    VariableData requested_variable = variable_memory.get(data_request.variableName);
    if (!requested_variable.isValid)
    {
      variable_memory.init(data_request.variableName, false);
      requested_variable = variable_memory.get(data_request.variableName);
    }

    packetToEncode.variableInUse = true;
    packetToEncode.isComplex = requested_variable.isComplex;
    packetToEncode.row = 1;
    packetToEncode.col = 1;
  } 
  else if(packetToEncode.variableType == RequestDataType::MATRIX) 
  {
    if (!matrix_memory.is_valid(data_request.variableName))
    {
      // Initialise an "empty" matrix to complete the transaction
      ComplexValue value;
      value.row = 1;
      value.col = 1;
      value.real_part = 0;
      value.imag_part = 0;

      if(debugMode)
      {
        Serial.println("Creating empty matrix to send, to complete the transaction");
      }

      matrix_memory.init(data_request.variableName, 1, 1, false);
      matrix_memory.receivedFromCFX(data_request.variableName, false);
      matrix_memory.append(data_request.variableName, value);
    }
    packetToEncode.variableInUse = true;
    packetToEncode.isComplex = matrix_memory.is_complex(data_request.variableName);
    packetToEncode.row = matrix_memory.rows(data_request.variableName);
    packetToEncode.col = matrix_memory.cols(data_request.variableName);

    Serial.println("Data item is valid: ");
    Serial.println(matrix_memory.is_valid(data_request.variableName));
  }
  else {
    go_to_idle_state("Request currently unsupported!");
    return false;
  }

  if(debugMode)
  {
    Serial.println("Sending variable description packet");
    Serial.print("Rows: ");
    Serial.print(packetToEncode.row);
    Serial.print(", cols: ");
    Serial.println(packetToEncode.col);
  }
  sendBuffer(VariableDescriptionPacket().encode(packetToEncode), 50);

  wait_for_ack();
  current_state = States::SEND_VALUE_PACKET;

  return true;
}

bool CFXSerial::state_WAIT_FOR_DATA_REQUEST()
{
  bool packetReceived = false;
  packetReceived = receivePacket();
  if(!packetReceived)
  {
    go_to_idle_state("Data request not received in time");
    return false;
  }

  // To leave WAIT_FOR_DATA_REQUEST state, we need either a request packet or a variable description packet
  if(packet_type == PacketType::REQUEST)
  {
    // Which kind of wakeup?
    data_request = RequestPacket().decode(buffer, size);
    if(!data_request.isValid)
    {
      go_to_idle_state("Data request is invalid");
      return false;
    }

    sendDataAck();

    if(!wait_for_ack())
    {
      go_to_idle_state("Data ack not acked");
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
      go_to_idle_state("Variable description packet not valid");
      return false;
    }

    // What are we about to receive?
    request_type = decodedPacket.variableType;
    variableName = decodedPacket.variableName;
    data_items_to_send = decodedPacket.row * decodedPacket.col;

    if(decodedPacket.variableType == RequestDataType::MATRIX)
    {
      matrix_memory.init(decodedPacket.variableName, decodedPacket.row, decodedPacket.col, decodedPacket.isComplex);
      matrix_memory.receivedFromCFX(decodedPacket.variableName, true);

      if(debugMode)
      {
        Serial.print("Matrix ");
        Serial.print(variableName);
        Serial.print(" has ");
        Serial.print(decodedPacket.row);
        Serial.print(" rows, ");
        Serial.print(decodedPacket.col);
        Serial.print(" cols and is ");
        if(decodedPacket.isComplex)
        {
          Serial.println("complex");
        } else {
          Serial.println("not complex");
        }
      }
    }
    else if(decodedPacket.variableType == RequestDataType::VARIABLE)
    {
      variable_memory.init(decodedPacket.variableName, decodedPacket.isComplex);
      if(debugMode)
      {
        Serial.print("Variable ");
        Serial.print(variableName);
        if(decodedPacket.isComplex)
        {
          Serial.println(" is complex");
        } else {
          Serial.println(" is not complex");
        }
      }
    }

    sendDataAck();
    // Store variable description packet
    variable_description = decodedPacket;
    current_state = States::RECEIVE_VALUE_PACKET;
    return true;
  }

  Serial.print("Packet type invalid - ");
  Serial.println((uint8_t)packet_type);

  go_to_idle_state("Received packet type invalid");
  return false;
}

bool CFXSerial::state_WAIT_FOR_SCREENSHOT_REQUEST()
{
  go_to_idle_state("WAIT_FOR_SCREENSHOT_REQUEST() not implemented");
  return false;
}

bool CFXSerial::wait_for_ack()
{
  // not really a state!
  bool packetReceived = false;
  packetReceived = receivePacket();
  if(!packetReceived)
  {
    go_to_idle_state("ACK not received");
    return false;
  }

  // Return false if not an ack
  if(packet_type != PacketType::ACK)
  {
    go_to_idle_state("Wrong ACK received");
    return false;
  }

  Ack decodedPacket = AckPacket().decode(buffer, size);
  if(!decodedPacket.isValid)
  {
    go_to_idle_state("ACK not valid");
    return false;
  }

  return true;
}

bool CFXSerial::go_to_idle_state(const std::string& reason)
{
  current_state = States::IDLE;
  if(debugMode)
  {
    Serial.print("Going to idle, reason: ");
    Serial.println(reason.c_str());
  }
  return true;
}

int CFXSerial::freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}