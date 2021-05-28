#pragma once

#include <Arduino.h>
#include <string>

void decode(uint8_t buffer[], size_t message_length);

enum PacketType {
    WAKE_UP,
    REQUEST,
    VARIABLE_DESCRIPTION,
    VALUE,
    END,
    UNSUPPORTED
};

enum WakeUpByte : uint8_t {
    DATA = 0x15,
    SCREENSHOT = 0x16
};

struct WakeUp {
    WakeUpByte wakeUpType;
    bool isValid;
};

struct Request {
    char variableType[2];
    char variableName;
    bool isValid;
};

struct VariableDescription {
    char variableType[2];
    bool variableInUse;
    char variableName;
    bool isComplex;
    bool isValid;
};

struct Value {
    uint8_t row;
    uint8_t col;
    double value;
    bool isValid;
};

struct ComplexValue {
    uint8_t row;
    uint8_t col;
    bool isComplex;
    double real_part;
    double imag_part;
    bool isValid;
};

struct End {
    bool isValid;
};

class PacketCodec {
    public:
        bool checksumValid(uint8_t* buffer, size_t size);
        uint8_t calculateChecksum(uint8_t buffer, size_t size);
        double getDoubleFromBinary(uint8_t* buffer, size_t size);
        uint8_t* getBinaryFromDouble(double value);

        PacketType getPacketType(uint8_t* buffer, size_t size);
};

class WakeUpPacket : PacketCodec {
    public:
        uint8_t* encode(WakeUpByte wakeUpType);
        WakeUp decode(uint8_t* buffer, size_t size);
    private:
        WakeUp decodedPacket;
        size_t const size = 1;
        uint8_t encodedPacket[1];
};

// class RequestPacket : PacketCodec {
//     public:
//         uint8_t* encode(char* variableType, char* variableName);
//         Request decode(uint8_t* buffer, size_t size);
//     private:
//         Request decodedPacket;
//         size_t const size = 50;
//         uint8_t encodedPacket[50];
// };

// class VariableDescriptionPacket : PacketCodec {
//     public:
//         uint8_t* encode(char* variableType, bool variableInUse, char* variableName, bool isComplex);
//         VariableDescription decode(uint8_t* buffer, size_t size);
//     private:
//         VariableDescription decodedPacket;
//         size_t const size = 50;
//         uint8_t encodedPacket[50];
// };

// class ValuePacket : PacketCodec {
//     public:
//         uint8_t* encode(uint8_t row, uint8_t col, double value);
//         uint8_t* encode(uint8_t row, uint8_t col, double real_part, double imag_part);
//         ComplexValue decode(uint8_t* buffer, size_t size);
//     private:
//         ComplexValue decodedPacket;
//         size_t const size = 26;
//         // Use only lower 16 bytes if not complex
//         uint8_t encodedPacket[26];
// };

// class EndPacket : PacketCodec {
//     public:
//         uint8_t* encode(void);
//         End decode(uint8_t* buffer, size_t size);
// };
