#include "codec.hpp"

bool PacketCodec::checksumValid(uint8_t* buffer, size_t size) {
    bool checksumValid = false;
    return checksumValid;
}

uint8_t PacketCodec::calculateChecksum(uint8_t buffer, size_t size) {
    uint8_t checksum = 0x00;
    return checksum;
}

double PacketCodec::getDoubleFromBinary(uint8_t* buffer, size_t size) {
    double value = 0.00;
    return value;
}

bool PacketCodec::getBinaryFromDouble(double value, uint8_t* buffer, size_t size) {
    buffer[0] = 0;
    return true;
}

PacketType PacketCodec::getPacketType(uint8_t* buffer, size_t size) {
    switch(size) {
        case 1:
            return WAKE_UP;
        case 16:
            return VALUE;
        case 26:
            return VALUE;
        case 50:
            std::string signature;
            for (int i=0; i<4; i++) {
                signature.push_back((char)buffer[i]);
            }

            if(signature.compare(":REQ") == 0) {
                return REQUEST;
            } else if(signature.compare(":END") == 0) {
                return END;
            } else if(signature.compare(":VAL") == 0) {
                return VARIABLE_DESCRIPTION;
            } else {
                return UNSUPPORTED;
            }
    }
    return UNSUPPORTED;
}

uint8_t* WakeUpPacket::encode(WakeUpByte wakeUpType) 
{
    encodedPacket[0] = (char)wakeUpType;
    return encodedPacket;
}

WakeUp WakeUpPacket::decode(uint8_t* buffer, size_t size) 
{
    WakeUp decodedPacket;
    decodedPacket.wakeUpType = static_cast<WakeUpByte>(buffer[0]);
    return decodedPacket;
}

// uint8_t* RequestPacket::encode(char* variableType, char* variableName) 
// {
// }

Request RequestPacket::decode(uint8_t* buffer, size_t size) 
{
    Request decodedPacket;
    return decodedPacket;
}

// uint8_t* VariableDescriptionPacket::encode(char* variableType, bool variableInUse, char* variableName, bool isComplex)
// {
// }

// VariableDescription VariableDescriptionPacket::decode(uint8_t* buffer, size_t size)
// {
//     VariableDescription decodedPacket;
//     return decodedPacket;
// }

// uint8_t* ValuePacket::encode(uint8_t row, uint8_t col, double value) {
// }

// uint8_t* ValuePacket::encode(uint8_t row, uint8_t col, double real_part, double imag_part) {
// }

// ComplexValue ValuePacket::decode(uint8_t* buffer, size_t size) {
//     ComplexValue decodedPacket;
//     return decodedPacket;
// }

// uint8_t* EndPacket::encode() {
// }

// End EndPacket::decode(uint8_t* buffer, size_t size) {
//     End decodedPacket;
//     return decodedPacket;
// }