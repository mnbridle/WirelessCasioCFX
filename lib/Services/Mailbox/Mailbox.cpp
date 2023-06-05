#include "Mailbox.h"

Mailbox::Mailbox()
{
}

bool Mailbox::sendAll()
{
}

bool Mailbox::receiveAll()
{
}

bool Mailbox::getInboxMsg(Message* message)
{

}
bool Mailbox::getInboxMsg(uint16_t message_id, Message* message)
{

}
bool Mailbox::getInboxMsgList(uint16_t from_id, uint16_t to_id, vector<Message>* sublist)
{
}

Message Mailbox::convertBufToMsg(uint16_t* buf, size_t len)
{
    Message message;

    for(size_t i=1; i<9; i++)
    {
        message.recipient.push_back(convert_scancode_to_ascii(buf[i]));
    }

    for(size_t i=9; i<17; i++)
    {
        message.sender.push_back(convert_scancode_to_ascii(buf[i]));
    }

    // Get date and time from ARM
    message.date = (unsigned int)buf[17];
    message.time = (unsigned int)buf[18];
    message.length = len - 19;

    for(size_t i=19; i<len; i++)
    {
        message.message.push_back(convert_scancode_to_ascii(buf[i]));
    }

    return message;
}

bool Mailbox::sendMsgToOutbox(Message message)
{
    outbox.push_back(message);
    return true;
}

bool Mailbox::sendMsgToOutbox(uint16_t* buf, size_t len)
{
    Message message;
    message = convertBufToMsg(buf, len);

    outbox.push_back(message);

    return true;
}
