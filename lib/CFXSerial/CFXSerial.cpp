#include <Arduino.h>
#include <wiring_private.h>
#include <CFXSerial.hpp>

#include <string>

Uart Serial2( &sercom1, PIN_SERIAL2_RX, PIN_SERIAL2_TX, PAD_SERIAL2_RX, PAD_SERIAL2_TX ) ;

TransactionState transactionState;
std::vector<int> cfxBuffer;
std::vector<int> transactionInfo;

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
  receiveData(rxChar);
}

void receiveData(char rxChar)
{    
  Serial.print("0x");
  Serial.print(rxChar < 16 ? "0" : "");
  Serial.print(rxChar, HEX);
  Serial.print(" - ");
  Serial.println(rxChar);

  if (transactionState == WAIT_FOR_WAKEUP && (rxChar == TRANSACTION_WAKEUP || rxChar == SCREENSHOT_WAKEUP)) {
    Serial.println("Wakeup received");
    sendByte(ACK_WAKEUP);
    Serial.println("Acknowledge wakeup");
    transactionState = WAIT_FOR_TRANSACTION_REQUEST_PACKET;
    Serial.println("Transaction started");
  }

  if (transactionState == WAIT_FOR_TRANSACTION_REQUEST_PACKET) {
    cfxBuffer.push_back(rxChar);
    if(cfxBuffer.size() == 50) {
      transactionInfo = cfxBuffer;
      cfxBuffer.clear();
      transactionState = PROCESS_TRANSACTION;
      Serial.print("TransactionInfo is ");
      Serial.print(transactionInfo.size());
      Serial.println(" bytes long.");
    }
  }

  if (transactionState == PROCESS_TRANSACTION) {
    // Decide what the transaction type is
    std::string tag = slice_str(transactionInfo, 0, 3);
    std::string datatype = slice_str(transactionInfo, 6, 7);

    Serial.print("Tag: ");
    Serial.println(tag.c_str());
    Serial.print("Data type: ");
    Serial.println(datatype.c_str());

    if(datatype == "VM") {
      Serial.println("Data type is a variable!");
    } else {
      Serial.println("Not sure what I've received");
    }
  }
}

void sendByte(char txByte) {
  Serial2.print(txByte);
}

void setUpSerialPort()
{
    Serial2.begin(9600, SERIAL_8N2);
    pinPeripheral(PIN_SERIAL2_RX, PIO_SERCOM_ALT);
    pinPeripheral(PIN_SERIAL2_TX, PIO_SERCOM_ALT);
}
