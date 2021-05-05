#include <Arduino.h>

// Serial2
#define PIN_SERIAL2_RX       (20ul) // PA31
#define PAD_SERIAL2_RX       (SERCOM_RX_PAD_3)
#define PIN_SERIAL2_TX       (19ul) // PA30
#define PAD_SERIAL2_TX       (UART_TX_PAD_2)


void setUpSerialPort(void);
void SERCOM1_Handler(void);
void hello_world(void);
void runSerialPortEchoTest(void);