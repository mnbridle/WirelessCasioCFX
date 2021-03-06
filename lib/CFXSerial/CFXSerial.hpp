#pragma once

#include <vector>
#include <string>
#include <queue>
#include <Arduino.h>
#include "codecs/CasioCFX/CasioCFX.hpp"
#include "storage/variables.h"
#include "storage/matrices.h"
#include "storage/messages.h"

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

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

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

        void init();
        
        bool state_IDLE();
        bool state_WAIT_FOR_DATA_REQUEST();
        bool state_SEND_VARIABLE_DESCRIPTION_PACKET();
        bool state_RECEIVE_VALUE_PACKET();
        bool state_SEND_VALUE_PACKET();
        bool state_SEND_END_PACKET();
        bool state_WAIT_FOR_SCREENSHOT_REQUEST();
        bool state_RECEIVE_SCREENSHOT_DATA();
        bool state_RECEIVE_END_PACKET();

        int freeMemory();

        void cfx_software_interface(unsigned long run_time);

        uint8_t buffer[1024];
        size_t size;
        PacketType packet_type;
        VariableStorage variable_memory;
        MatrixStorage matrix_memory;
        MessageStorage message_storage;

        bool debugMode;
        
    private:
        States current_state;
        void sendByte(uint8_t);
        void sendBuffer(uint8_t* buffer, size_t size);
        void debug_buffer(uint8_t* buffer, size_t size, bool is_sending);
        bool wait_for_ack();
        bool go_to_idle_state(const std::string& reason);
        bool go_to_idle_state();

        Request data_request;
        VariableDescription variable_description;

        RequestDataType request_type;
        char variableName;
        long data_items_to_send;

        bool send_variable();
        bool send_matrix();
};

void setUpSerialPort(void);
void SERCOM1_Handler(void);