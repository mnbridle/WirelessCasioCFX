#pragma once

#include <vector>
#include <string>
#include <queue>
#include <Arduino.h>
#include "codecs/codec.hpp"

// Serial2
#define PIN_SERIAL2_RX       (20ul) // PA31
#define PAD_SERIAL2_RX       (SERCOM_RX_PAD_3)
#define PIN_SERIAL2_TX       (19ul) // PA30
#define PAD_SERIAL2_TX       (UART_TX_PAD_2)

#define SCREENSHOT_WAKEUP      0x16
#define TRANSACTION_WAKEUP     0x15

#define ACK_WAKEUP             0x13
#define ACK_PACKET             0x06

#define REAL_VARIABLE_BYTES    'VariableR'
#define COMPLEX_VARIABLE_BYTES 'VariableC'
#define VARIABLE_BYTES         'VM'
#define LIST_BYTES             'LT'
#define MATRIX_BYTES           'MT'
#define IMAGE_BYTES            'PC'
#define SCREENSHOT_BYTES       'DW'

enum class Transitions {
	RECEIVED_DATA_WAKE_UP,
	RECEIVED_SCREENSHOT_WAKE_UP,
	RECEIVED_REQUEST_PACKET,
	RECEIVED_VARIABLE_DESCRIPTION_PACKET,
	RECEIVED_SCREENSHOT_REQUEST,
	RECEIVED_ACK,
	RECEIVED_VALUE_PACKET,
	RECEIVED_SCREENSHOT_DATA,
	RECEIVED_END_PACKET,
	SENT_END_PACKET
};

enum class States {
    IDLE,
    WAIT_FOR_DATA_REQUEST,
    WAIT_FOR_ACK,
    SEND_VARIABLE_DESCRIPTION_PACKET,
    RECEIVE_VALUE_PACKET,
    SEND_VALUE_PACKET,
    SEND_END_PACKET,
    WAIT_FOR_SCREENSHOT_REQUEST,
    RECEIVE_SCREENSHOT_DATA,
    RECEIVE_END_PACKET
};

class CFXSerial {
    public:
        CFXSerial();
        ~CFXSerial();

        void receiveByte(char);
        bool receivePacket(void);

        bool process_transaction(void);

        void sendWakeUpAck(void);
        void sendDataAck(void);

        bool execute_current_state();

        bool state_IDLE();
        bool state_WAIT_FOR_DATA_REQUEST();
        bool state_WAIT_FOR_ACK();
        bool state_SEND_VARIABLE_DESCRIPTION_PACKET();
        bool state_RECEIVE_VALUE_PACKET();
        bool state_SEND_VALUE_PACKET();
        bool state_SEND_END_PACKET();
        bool state_WAIT_FOR_SCREENSHOT_REQUEST();
        bool state_RECEIVE_SCREENSHOT_DATA();
        bool state_RECEIVE_END_PACKET();

        uint8_t buffer[1024];
        size_t size;
        PacketType packet_type;
        
    private:
        States current_state;
        void sendByte(uint8_t);
        void sendBuffer(uint8_t* buffer, size_t size);

        Request data_request;

};

void setUpSerialPort(void);
void SERCOM1_Handler(void);