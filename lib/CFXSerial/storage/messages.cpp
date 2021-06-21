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
        {'÷', 339},
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
        {339, '÷'},
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
    size_t length = sent_message.matrix_data.size();

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

    for(size_t i=19; i<length; i++)
    {
        message.message.push_back(convert_scancode_to_ascii(sent_message.matrix_data.at(i)));
    }

    Serial.println("Received message!");
    Serial.print("From: ");
    Serial.println(message.sender.c_str());
    Serial.print("To: ");
    Serial.println(message.recipient.c_str());
    Serial.print("On ");
    Serial.print(message.date);
    Serial.print(" at ");
    Serial.println(message.time);
    Serial.print("Length: ");
    Serial.println(length);
    Serial.print("Contents: ");
    Serial.println(message.message.c_str());

    send_queue.push_back(message);

    return true;
}

MatrixData MessageStorage::process_received_message()
{
    MatrixData received_message;
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