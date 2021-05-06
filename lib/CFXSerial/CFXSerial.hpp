#include <list>
#include <vector>
#include <string>

#include <Arduino.h>

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

enum TransactionState {
    WAIT_FOR_WAKEUP,
    WAIT_FOR_TRANSACTION_REQUEST_PACKET,
    PROCESS_TRANSACTION,
    TRANSACTION_PROCESSED
};

enum RequestType {
    DataRequest,
    ValueData,
    EndPacket,
    Screenshot
};

struct DataInfo {
    std::string dataType;
    char identifier;
    int rows;
    int cols;
    char variableName;
    char realOrComplex;
};

class CFXSerial {
    private:
        char wakeupType;
        TransactionState transactionState = WAIT_FOR_WAKEUP;

        RequestType requestType;
        DataInfo dataInfo;

        std::vector<int> buffer;

        void waitForWakeup(char);
        void waitForTransactionRequestPacket(char);
        void processTransaction(char);
        void showBufferContents(void);

    public:
        void receiveByte(char);
        void sendByte(Uart &serialPort, char);
};

void setUpSerialPort(void);
void SERCOM1_Handler(void);