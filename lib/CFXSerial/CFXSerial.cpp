#include <Arduino.h>
#include <wiring_private.h>
#include <CFXSerial.hpp>

#include <string>

Uart Serial2( &sercom1, PIN_SERIAL2_RX, PIN_SERIAL2_TX, PAD_SERIAL2_RX, PAD_SERIAL2_TX ) ;
CFXSerial cfxSerial;

template<typename T>
std::vector<T> slice(std::vector<T> &v, int m, int n)
{
    std::vector<T> vec;
    std::copy(v.begin() + m, v.begin() + n + 1, std::back_inserter(vec));
    return vec;
}

template<typename T>
std::string slice_str(std::vector<T> &v, int m, int n)
{
    std::vector<int> vec = slice(v, m, n);
    std::string vec_str(vec.begin(), vec.end());
    return vec_str;
}

void SERCOM1_Handler()
{
  Serial2.IrqHandler();
  char rxChar = Serial2.read();
  cfxSerial.receiveByte(rxChar);
}

void CFXSerial::receiveByte(char rxChar) {
  switch(transactionState) {
    case WAIT_FOR_WAKEUP:
      waitForWakeup(rxChar);
      break;
    case WAIT_FOR_TRANSACTION_REQUEST_PACKET:
      waitForTransactionRequestPacket(rxChar);
      break;
    case PROCESS_TRANSACTION:
      processTransaction(rxChar);
      break;
    case TRANSACTION_PROCESSED:
      Serial.println("Done! Going back to wait state.");
      transactionState = WAIT_FOR_WAKEUP;
      break;

    default:
      break;
  }
}

void CFXSerial::waitForWakeup(char rxChar) {
  if(rxChar == TRANSACTION_WAKEUP || rxChar == SCREENSHOT_WAKEUP) {
    wakeupType = rxChar;
    transactionState = WAIT_FOR_TRANSACTION_REQUEST_PACKET;
    Serial.println("Transition to WAIT_FOR_TRANSACTION_REQUEST_PACKET");
    sendByte(Serial2, ACK_WAKEUP);
  } else {
    Serial.println("Received unknown character while waiting for wakeup");
  }
}

void CFXSerial::waitForTransactionRequestPacket(char rxChar) {
  buffer.push_back(rxChar);
  if(wakeupType == TRANSACTION_WAKEUP && buffer.size() == 50) {
    // Transaction has completed, process it!
    showBufferContents();

    std::string tag = slice_str(buffer, 0, 3);
    dataInfo.dataType = slice_str(buffer, 5, 6);

    dataInfo.rows = (uint8_t)buffer[8];
    dataInfo.cols = (uint8_t)buffer[10];

    dataInfo.variableName = (char)buffer[11];
    dataInfo.realOrComplex = (char)buffer[27];

    if(tag == ":REQ") {
      requestType = DataRequest;
    } else if(tag == ":VAL") {
      requestType = ValueData;
    } else if(tag == ":END") {
      requestType = EndPacket;
    } else if(tag == ":DD@") {
      requestType = Screenshot;
    } else {
      Serial.print("Unsupported request type: ");
      Serial.println(tag.c_str());
    }

    Serial.println(tag.c_str());
    Serial.println(requestType);
    Serial.println(dataInfo.rows);
    Serial.println(dataInfo.cols);
    Serial.println(dataInfo.variableName);
    Serial.println(dataInfo.realOrComplex);
    
    buffer.clear();
    transactionState = PROCESS_TRANSACTION;
    Serial.println("Transitioned to state PROCESS_TRANSACTION");
    sendByte(Serial2, ACK_PACKET);
  }
}

void CFXSerial::processTransaction(char rxChar) {
  buffer.push_back(rxChar);
  Serial.println(buffer.size());
  // What are we trying to do here?
  if (dataInfo.dataType == "VM" && dataInfo.realOrComplex == 'R' && buffer.size() == 16) {
    Serial.println("Value packet was received!");
    showBufferContents();
    Serial.println("Transition to state TRANSACTION_PROCESSED");
    buffer.clear();
    transactionState = TRANSACTION_PROCESSED;
    sendByte(Serial2, ACK_PACKET);
  }
}

void CFXSerial::sendByte(Uart &cfxSerialPort, char txByte) {
  delay(200);
  cfxSerialPort.print(txByte);
}

void CFXSerial::showBufferContents() {
  for (unsigned int i=0; i<buffer.size(); i++) {
    Serial.print("0x");
    Serial.print(buffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println("");
}

void setUpSerialPort()
{
    Serial2.begin(9600, SERIAL_8N2);
    pinPeripheral(PIN_SERIAL2_RX, PIO_SERCOM_ALT);
    pinPeripheral(PIN_SERIAL2_TX, PIO_SERCOM_ALT);
}
