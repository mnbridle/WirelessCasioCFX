#include "codec.hpp"

bool PacketCodec::checksumValid(uint8_t* buffer, size_t size) {
    uint8_t checksum = PacketCodec::calculateChecksum(buffer, size);
    return checksum == buffer[size-1];
}

uint8_t PacketCodec::calculateChecksum(uint8_t* buffer, size_t size) {
    uint8_t checksum = 0x00;
    uint16_t sum_bytes = 0x0000;

    for (size_t i=0; i<size-1; i++) {
        sum_bytes += buffer[i];
    }

    sum_bytes -= 0x3A;
    checksum = (0x01 + ~sum_bytes) & 0xFF;

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
            if(buffer[0] == 0x06)
            {
                return PacketType::ACK;
            }
            else 
            {
                return PacketType::WAKE_UP;
            }
        case 16:
            return PacketType::VALUE;
        case 26:
            return PacketType::VALUE;
        case 50:
            std::string signature;
            for (size_t i=0; i<4; i++) {
                signature.push_back((char)buffer[i]);
            }

            if(signature.compare(":REQ") == 0) {
                return PacketType::REQUEST;
            } else if(signature.compare(":END") == 0) {
                return PacketType::END;
            } else if(signature.compare(":VAL") == 0) {
                return PacketType::VARIABLE_DESCRIPTION;
            } else {
                return PacketType::UNSUPPORTED;
            }
    }
    return PacketType::UNSUPPORTED;
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
    decodedPacket.isValid = true;
    return decodedPacket;
}

uint8_t* AckPacket::encode()
{
    encodedPacket[0] = 0x06;
    return encodedPacket;
}

Ack AckPacket::decode(uint8_t* buffer, size_t size)
{
    Ack decodedPacket;
    decodedPacket.isValid = (buffer[0] == 0x06);
    return decodedPacket;
}

// uint8_t* RequestPacket::encode(char* variableType, char* variableName) 
// {
// }

Request RequestPacket::decode(uint8_t* buffer, size_t size) 
{
    Request decodedPacket;

    std::string request_data_type;
    
    for (size_t i=5; i<7; i++) {
        request_data_type.push_back((char)buffer[i]);
    }

    decodedPacket.variableName = (char)buffer[11];

    if(request_data_type.compare("VM") == 0) 
    {
        decodedPacket.variableType = RequestDataType::VARIABLE;
    }
    else if(request_data_type.compare("PC") == 0) 
    {
        decodedPacket.variableType = RequestDataType::PICTURE;
    }
    else if(request_data_type.compare("LT") == 0) 
    {
        decodedPacket.variableType = RequestDataType::LIST;
    }
    else if(request_data_type.compare("MT") == 0) 
    {
        decodedPacket.variableType = RequestDataType::MATRIX;
    }
    else 
    {
        decodedPacket.variableType = RequestDataType::UNKNOWN;
    }

    decodedPacket.isValid = checksumValid(buffer, size);

    return decodedPacket;
}

uint8_t* VariableDescriptionPacket::encode(VariableDescription packetToEncode)
{
    String tempString;
    uint8_t offset = 0;

    tempString = ":VAL";
    for (size_t i=0; i<4; i++)
    {
        encodedPacket[i + offset] = tempString[i];
    }

    encodedPacket[4] = 0x00;

    switch(packetToEncode.variableType)
    {
        case RequestDataType::LIST :
            tempString = "LT";
            break;
        case RequestDataType::MATRIX :
            tempString = "MT";
            break;
        case RequestDataType::PICTURE :
            tempString = "PC";
            break;
        case RequestDataType::VARIABLE :
            tempString = "VM";
            break;
        default:
            // Unsupported, just say VM
            tempString = "VM";
    }

    offset = 5;
    for (size_t i=0; i<2; i++)
    {
        encodedPacket[i + offset] = tempString[i];
    }

    encodedPacket[7] = 0x00;
    encodedPacket[8] = packetToEncode.variableInUse;
    encodedPacket[9] = 0x00;
    encodedPacket[10] = packetToEncode.variableInUse;
    encodedPacket[11] = packetToEncode.variableName;

    offset = 12;
    for (size_t i=0; i<8; i++)
    {
        encodedPacket[i+offset] = 0xFF;
    }

    tempString = "Variable";
    offset = 19;
    for (size_t i=0; i<8; i++)
    {
        encodedPacket[i+offset] = tempString[i];
    }

    if(packetToEncode.isComplex)
    {
        encodedPacket[27] = 'C';
    }
    else
    {
        encodedPacket[27] = 'R';
    }

    encodedPacket[28] = 0x0A;

    offset = 29;
    for(size_t i=0; i<20; i++)
    {
        encodedPacket[i+offset] = 0xFF;
    }
    encodedPacket[49] = calculateChecksum(encodedPacket, 50);

    return encodedPacket;
}

VariableDescription VariableDescriptionPacket::decode(uint8_t* buffer, size_t size)
{
    VariableDescription decodedPacket;
    decodedPacket.isValid = checksumValid(buffer, size);
    return decodedPacket;
}

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