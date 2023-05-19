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

bool Mailbox::sendMsgToOutbox(Message* message)
{
}