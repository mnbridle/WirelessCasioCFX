#pragma once

#include <vector>

#include "Message.h"

using namespace std;

class Mailbox {
    private:
        vector<Message> inbox;
        vector<Message> outbox;
    public:
        Mailbox();
        bool getInboxMsg(Message* message);
        bool getInboxMsg(uint16_t message_id, Message* message);
        bool getInboxMsgList(uint16_t from_id, uint16_t to_id, vector<Message>* sublist);
        bool sendMsgToOutbox(Message* message);

        bool sendAll(void);
        bool receiveAll(void);

        // Get information about messages in inbox and outbox
        uint16_t getInboxMsgCount(void);
        uint16_t getOutboxMsgCount(void);
};