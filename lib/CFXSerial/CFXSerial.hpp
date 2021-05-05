#include <list>
#include <vector>

#include <Arduino.h>

// Serial2
#define PIN_SERIAL2_RX       (20ul) // PA31
#define PAD_SERIAL2_RX       (SERCOM_RX_PAD_3)
#define PIN_SERIAL2_TX       (19ul) // PA30
#define PAD_SERIAL2_TX       (UART_TX_PAD_2)

#define SCREENSHOT_WAKEUP    0x16
#define TRANSACTION_WAKEUP   0x15

#define ACK_WAKEUP           0x13
#define ACK_PACKET           0x06

enum TransactionState {
    WAIT_FOR_WAKEUP,
    WAIT_FOR_TRANSACTION_REQUEST_PACKET,
    PROCESS_TRANSACTION,
    TRANSACTION_PROCESSED
};

void setUpSerialPort(void);
void SERCOM1_Handler(void);
void sendByte(char);
void receiveData(char);