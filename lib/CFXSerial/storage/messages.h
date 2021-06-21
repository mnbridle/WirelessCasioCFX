#pragma once

#include <vector>
#include <list>
#include <map>
#include "matrices.h"

struct Message
{
    unsigned int time;
    unsigned int date;
    std::string sender;
    std::string recipient;
    std::string message;
};

/*

process_received_message - pull a Message out of the receive_queue and turn it into MatrixData, to
                           on-send to the calculator.
process_sent_message - takes a message from MatrixStorage and converts it into a textual message. This
                       textual message is sent to the send_queue.

*/

class MessageStorage
{
    public:
        MessageStorage();
        MatrixData process_received_message();
        MatrixData process_received_message(unsigned short index);
        bool process_sent_message(MatrixData sent_message);

    private:
        std::list<Message> send_queue;
        std::list<Message> receive_queue;
        char convert_scancode_to_ascii(ComplexValue scancode);
        ComplexValue convert_ascii_to_scancode(char ascii, uint8_t row, uint8_t col);
        const std::map<char, unsigned short>& ascii_to_scancode(void);
        const std::map<unsigned short, char>& scancode_to_ascii(void);
};