#include "messages.h"

MessageStorage::MessageStorage()
{
}

const std::map<char, unsigned short>& MessageStorage::ascii_to_scancode() 
{ 
    static const auto* map = new std::map<char, unsigned short>
    { 
        {'A', 76},
        {'B', 66},
        {'C', 56},
        {'D', 46},
        {'E', 36},
        {'F', 26},
        {'G', 75},
        {'H', 65},
        {'I', 55},
        {'J', 45},
        {'K', 35},
        {'L', 25},
        {'M', 74},
        {'N', 64},
        {'O', 54},
        {'P', 73},
        {'Q', 63},
        {'R', 53},
        {'S', 43},
        {'T', 33},
        {'U', 72},
        {'V', 62},
        {'W', 52},
        {'X', 42},
        {'Y', 32},
        {'Z', 71},
        {'a', 176},
        {'b', 166},
        {'c', 156},
        {'d', 146},
        {'e', 136},
        {'f', 126},
        {'g', 175},
        {'h', 165},
        {'i', 155},
        {'j', 145},
        {'k', 135},
        {'l', 125},
        {'m', 174},
        {'n', 164},
        {'o', 154},
        {'p', 173},
        {'q', 163},
        {'r', 153},
        {'s', 143},
        {'t', 133},
        {'u', 172},
        {'v', 162},
        {'w', 152},
        {'x', 142},
        {'y', 132},
        {'z', 171},
        {'0', 271},
        {'1', 272},
        {'2', 262},
        {'3', 252},
        {'4', 273},
        {'5', 263},
        {'6', 253},
        {'7', 274},
        {'8', 264},
        {'9', 254},
        {'!', 379},
        {'@', 369},
        {'#', 359},
        {'$', 349},
        {0xF6, 339}, // division symbol
        {'^', 329},
        {'&', 479},
        {'*', 469},
        {'(', 459},
        {')', 449},
        {'_', 439},
        {'+', 429},
        {'-', 579},
        {'=', 569},
        {'{', 559},
        {'}', 549},
        {'>', 539},
        {'P', 529},
        {'\'', 679},
        {';', 669},
        {'<', 659},
        {'=', 649},
        {'>', 639},
        {',', 629},
        {'/', 779},
        {'~', 769},
        {'<', 759},
        {'=', 749},
        {'>', 739},
        {'?', 729},
        {' ', 61},
        {'.', 261},
        {'\n', 31}
    }; 
    return *map; 
}

const std::map<unsigned short, char>& MessageStorage::scancode_to_ascii() 
{ 
    static const auto* map = new std::map<unsigned short, char>
    { 
        {76, 'A'},
        {66, 'B'},
        {56, 'C'},
        {46, 'D'},
        {36, 'E'},
        {26, 'F'},
        {75, 'G'},
        {65, 'H'},
        {55, 'I'},
        {45, 'J'},
        {35, 'K'},
        {25, 'L'},
        {74, 'M'},
        {64, 'N'},
        {54, 'O'},
        {73, 'P'},
        {63, 'Q'},
        {53, 'R'},
        {43, 'S'},
        {33, 'T'},
        {72, 'U'},
        {62, 'V'},
        {52, 'W'},
        {42, 'X'},
        {32, 'Y'},
        {71, 'Z'},
        {176, 'a'},
        {166, 'b'},
        {156, 'c'},
        {146, 'd'},
        {136, 'e'},
        {126, 'f'},
        {175, 'g'},
        {165, 'h'},
        {155, 'i'},
        {145, 'j'},
        {135, 'k'},
        {125, 'l'},
        {174, 'm'},
        {164, 'n'},
        {154, 'o'},
        {173, 'p'},
        {163, 'q'},
        {153, 'r'},
        {143, 's'},
        {133, 't'},
        {172, 'u'},
        {162, 'v'},
        {152, 'w'},
        {142, 'x'},
        {132, 'y'},
        {171, 'z'},
        {271, '0'},
        {272, '1'},
        {262, '2'},
        {252, '3'},
        {273, '4'},
        {263, '5'},
        {253, '6'},
        {274, '7'},
        {264, '8'},
        {254, '9'},
        {379, '!'},
        {369, '@'},
        {359, '#'},
        {349, '$'},
        {339, 0xF6}, // division symbol
        {329, '^'},
        {479, '&'},
        {469, '*'},
        {459, '('},
        {449, ')'},
        {439, '_'},
        {429, '+'},
        {579, '-'},
        {569, '='},
        {559, '{'},
        {549, '}'},
        {539, '>'},
        {529, 'P'},
        {679, '\''},
        {669, ';'},
        {659, '<'},
        {649, '='},
        {639, '>'},
        {629, ','},
        {779, '/'},
        {769, '~'},
        {759, '<'},
        {749, '='},
        {739, '>'},
        {729, '?'},
        {61, ' '},
        {261, '.'},
        {131, '\n'},
        {231, '\n'},
        {331, '\n'},
        {431, '\n'},
        {531, '\n'},
        {631, '\n'},
        {731, '\n'},
        {31, '\n'},
        {161, ' '}
    };
    return *map; 
}

bool MessageStorage::process_sent_message(MatrixData sent_message)
{
    Message message;
    message.length = sent_message.matrix_data.size();

    for(size_t i=1; i<9; i++)
    {
        message.recipient.push_back(convert_scancode_to_ascii(sent_message.matrix_data.at(i)));
    }

    for(size_t i=9; i<17; i++)
    {
        message.sender.push_back(convert_scancode_to_ascii(sent_message.matrix_data.at(i)));
    }

    message.date = (unsigned int)sent_message.matrix_data.at(17).real_part;
    message.time = (unsigned int)sent_message.matrix_data.at(18).real_part;

    for(size_t i=19; i<message.length; i++)
    {
        message.message.push_back(convert_scancode_to_ascii(sent_message.matrix_data.at(i)));
    }

    outbox.push_back(message);
    // Send to inbox for debugging purposes
    inbox.push_back(message);

    return true;
}

MatrixData MessageStorage::process_received_message()
{
    MatrixData received_message;
    Message message = inbox.front();
    inbox.pop_front();

    ComplexValue date;
    ComplexValue time;
    ComplexValue scancode;
    
    uint8_t offset = 0;

    // Now we have the message, turn it into scancodes
    received_message.cols = (uint8_t)message.length;
    received_message.rows = 1;

    ComplexValue message_type;
    message_type.real_part = 2;
    message_type.row = 1;
    message_type.col = 1;

    received_message.matrix_data.push_back(message_type);

    offset = 2;
    for (uint8_t i=0; i<8; i++) 
    {
        if (i<message.sender.length())
        {
            scancode = convert_ascii_to_scancode(message.sender.at(i), 1, i+offset);
        }
        else
        {
            scancode.real_part = 0;
            scancode.row = 1;
            scancode.col = i+offset;
        }

        received_message.matrix_data.push_back(scancode);
    }

    offset = 10;
    for (uint8_t i=0; i<8 && i<message.recipient.length(); i++) 
    {
        if (i<message.recipient.length())
        {
            scancode = convert_ascii_to_scancode(message.recipient.at(i), 1, i+offset);
        }
        else
        {
            scancode.real_part = 0;
            scancode.row = 1;
            scancode.col = i+offset;
        }

        received_message.matrix_data.push_back(scancode);
    }

    date.real_part = message.date;
    date.col = 18;
    date.row = 1;
    time.real_part = message.time;
    time.col = 19;
    time.row = 1;

    received_message.matrix_data.push_back(date);
    received_message.matrix_data.push_back(time);

    offset = 20;
    for (uint8_t i=0; i<message.message.length(); i++)
    {
        received_message.matrix_data.push_back(convert_ascii_to_scancode(message.message.at(i), 1, i+offset));
    }

    received_message.isValid = true;
    received_message.isComplex = false;
    received_message.receivedFromCFX = false;

    return received_message;
}

MatrixData MessageStorage::process_received_message(unsigned short index)
{
    MatrixData received_message;
    return received_message;
}

char MessageStorage::convert_scancode_to_ascii(ComplexValue value)
{
    unsigned short scancode = (unsigned short)value.real_part;
    std::map<unsigned short, char> map = scancode_to_ascii();    
    char ascii = map[scancode];
    return ascii;
}

ComplexValue MessageStorage::convert_ascii_to_scancode(char ascii, uint8_t row, uint8_t col)
{
    ComplexValue scancode;
    std::map<char, unsigned short> map = ascii_to_scancode();
    scancode.real_part = (double)map[ascii];
    scancode.row = row;
    scancode.col = col;
    return scancode;
}

DatagramType MessageStorage::message_type(MatrixData message)
{
    // Message type lives in the first element of the matrix
    if (message.matrix_data.size() < 1)
    {
        return DatagramType::UNKNOWN;
    }

    return static_cast<DatagramType>(message.matrix_data.at(0).real_part);
}

bool MessageStorage::send_message_to_outbox(Message message)
{
    Serial.println("Sending message to outbox");
    outbox.push_back(message);
    return true;
}

bool MessageStorage::send_message_to_inbox(Message message)
{
    Serial.println("Sending message to inbox");
    inbox.push_back(message);
    return true;
}

bool MessageStorage::outbox_empty()
{
    return outbox.empty();
}

Message MessageStorage::get_outbox_message()
{
    Message message = outbox.front();
    outbox.pop_front();
    return message;
}