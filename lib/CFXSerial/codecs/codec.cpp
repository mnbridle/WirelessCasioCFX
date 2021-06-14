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

bool PacketCodec::double2bcd(double value, bool hasImaginaryPart, uint8_t* buffer, size_t size, size_t offset) 
{
    bool partIsNegative, valueIsOneOrMore;
    float exponent;
    uint8_t nibble_buffer[16] = {0x00};
    uint8_t signInfoByte = 0;

    // Get information for SignInfoByte and exponent
    partIsNegative = (value < 0);
    valueIsOneOrMore = (value >= 1);
    value = abs(value);
    exponent = round(log10(value));
    
    // Convert value into fixed-point using exponent
    value /= pow(10, exponent);

    // First nibble is always zero
    nibble_buffer[0] = 0;

    for(uint8_t i=1; i<16; i++)
    {
        nibble_buffer[i] = (uint8_t)value;
        value -= (uint8_t)value;
        value *= 10;
    }

    // Get nibbles out and create the actual buffer
    for(uint8_t i=0; i<8; i++)
    {
        buffer[i + offset] = nibble_buffer[i*2] << 4 | nibble_buffer[i*2 + 1];
    }

    // signInfoByte is byte 8 of buffer
    signInfoByte = hasImaginaryPart << 7 | partIsNegative << 6 | partIsNegative << 4 | valueIsOneOrMore;
    buffer[8 + offset] = signInfoByte;

    if(exponent<0)
    {
        exponent += 100;
    } else {
        exponent = abs(exponent);
    }

    nibble_buffer[0] = (uint8_t)(exponent/10);
    nibble_buffer[1] = (uint8_t)(10*((exponent/10)-nibble_buffer[0]));

    // exponent_bcd is byte 9 of buffer
    buffer[9 + offset] = nibble_buffer[0] << 4 | nibble_buffer[1];

    return true;
}

double PacketCodec::bcd2double(uint8_t* buffer, size_t size, size_t offset){
    double value = 0;
    uint8_t digits = 0;
    int8_t exponent = 0;

    bool hasImaginaryPart, partIsNegative, valueIsOneOrMore;

    hasImaginaryPart = (buffer[8+offset] >> 7) & 0x01;
    partIsNegative = (buffer[8+offset] >> 6) & 0x01;
    valueIsOneOrMore = buffer[8+offset] & 0x01;

    for (int8_t i=7; i>=0; i--)
    {
        digits = ((buffer[i+offset] & 0xF0) >> 4) * 10 + (buffer[i+offset] & 0x0F);
        value += digits * pow(10, 2*(7-i));
    }

    // Put decimal point in the right place
    value *= pow(10, -14);

    if(partIsNegative)
    {
        value *= -1;
    }

    // Get exponent - lives in buffer[9]
    exponent = ((buffer[9+offset] & 0xF0) >> 4) * 10 + (buffer[9+offset] & 0x0F);

    if(!valueIsOneOrMore){
        exponent -= 100;
    } 

    value *= pow(10, exponent);
    return value;
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
        {
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
                Serial.println("Invalid packet of size 50!");
                //return PacketType::UNSUPPORTED;
            }
        }
        default:
        {
            Serial.println("Something funky happened.");
            for(size_t i=0; i<size; i++)
            {
              Serial.print(buffer[i], HEX);
              buffer[i] = 0x00;
              Serial.print(" ");
            }
            size = 0;
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
    std::string tempString;
        
    for (size_t i=5; i<7; i++) {
        tempString.push_back((char)buffer[i]);
    }

    decodedPacket.variableName = (char)buffer[11];

    if(tempString.compare("VM") == 0) 
    {
        decodedPacket.variableType = RequestDataType::VARIABLE;
    }
    else if(tempString.compare("PC") == 0) 
    {
        decodedPacket.variableType = RequestDataType::PICTURE;
    }
    else if(tempString.compare("LT") == 0) 
    {
        decodedPacket.variableType = RequestDataType::LIST;
    }
    else if(tempString.compare("MT") == 0) 
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
    std::string tempString;

    decodedPacket.isValid = checksumValid(buffer, size);
        
    for (size_t i=5; i<7; i++) {
        tempString.push_back((char)buffer[i]);
    }

    decodedPacket.variableName = (char)buffer[11];

    if(tempString.compare("VM") == 0) 
    {
        decodedPacket.variableType = RequestDataType::VARIABLE;
    }
    else if(tempString.compare("PC") == 0) 
    {
        decodedPacket.variableType = RequestDataType::PICTURE;
    }
    else if(tempString.compare("LT") == 0) 
    {
        decodedPacket.variableType = RequestDataType::LIST;
    }
    else if(tempString.compare("MT") == 0) 
    {
        decodedPacket.variableType = RequestDataType::MATRIX;
    }
    else 
    {
        Serial.print("Unknown type ");
        Serial.println(tempString.c_str());
        decodedPacket.variableType = RequestDataType::UNKNOWN;
    }

    if(buffer[27] == 'C')
    {
        decodedPacket.isComplex = true;
    } else {
        decodedPacket.isComplex = false;
    }

    decodedPacket.row = buffer[8];
    decodedPacket.col = buffer[10];

    decodedPacket.variableInUse = !(decodedPacket.row == 0);

    return decodedPacket;
}

uint8_t* ValuePacket::encode(ComplexValue packetToEncode) {
    uint8_t packet_length = 16;

    encodedPacket[0] = ':';
    encodedPacket[1] = 0x00;
    encodedPacket[2] = packetToEncode.row;
    encodedPacket[3] = 0x00;
    encodedPacket[4] = packetToEncode.col;

    if(packetToEncode.isComplex)
    {
        packet_length = 26;
    }

    double2bcd(packetToEncode.real_part, packetToEncode.isComplex, encodedPacket, packet_length, 5);

    if(packetToEncode.isComplex)
    {
        double2bcd(packetToEncode.imag_part, packetToEncode.isComplex, encodedPacket, packet_length, 15);
    }

    // Get checksum
    encodedPacket[packet_length - 1] = calculateChecksum(encodedPacket, packet_length);

    return encodedPacket;
}

ComplexValue ValuePacket::decode(uint8_t* buffer, size_t size) {
    ComplexValue decodedPacket;
    decodedPacket.isValid = checksumValid(buffer, size);
    decodedPacket.row = buffer[2];
    decodedPacket.col = buffer[4];
    decodedPacket.isComplex = (bool)((buffer[13]>>7) & 0x01);
    decodedPacket.real_part = bcd2double(buffer, size, 5);

    if(decodedPacket.isComplex)
    {
        decodedPacket.imag_part = bcd2double(buffer, size, 15);
    } else {
        decodedPacket.imag_part = 0;
    }

    return decodedPacket;
}

uint8_t* EndPacket::encode(End packetToEncode) {
    encodedPacket[0] = ':';
    encodedPacket[1] = 'E';
    encodedPacket[2] = 'N';
    encodedPacket[3] = 'D';
    for(size_t i=4; i<49; i++)
    {
        encodedPacket[i] = 0xFF;
    }
    encodedPacket[49] = 0x56;

    return encodedPacket;
}

End EndPacket::decode(uint8_t* buffer, size_t size) {
    End decodedPacket;
    // We don't actually care what the contents are, it will always be valid
    decodedPacket.isValid = true;

    return decodedPacket;
}