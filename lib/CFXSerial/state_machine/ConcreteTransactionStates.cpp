#include "ConcreteTransactionStates.h"

bool Idle::transition(Transaction* Transaction, Transitions transition)
{
    // Check to ensure the requested transition is valid
    if(transition == RECEIVED_SCREENSHOT_WAKE_UP)
    {
        Transaction->setState(WaitForScreenshotRequest::getInstance());
    } else if(transition == RECEIVED_DATA_WAKE_UP)
    {
        Transaction->setState(WaitForDataRequest::getInstance());
    } else {
        return false;
    }

    return true;
}

TransactionState& Idle::getInstance()
{
    static Idle singleton;
    return singleton;
}

// ------------------- WaitForDataRequest -------------------

bool WaitForDataRequest::transition(Transaction* Transaction, Transitions transition)
{
    if(transition == RECEIVED_REQUEST_PACKET) {
        Transaction->setState(SendVariableDescriptionPacket::getInstance());
    } else if(transition == RECEIVED_VARIABLE_DESCRIPTION_PACKET) {
        Transaction->setState(ReceiveValuePacket::getInstance());
    } else {
        return false;
    }
    return true;
}
TransactionState& WaitForDataRequest::getInstance()
{
    static WaitForDataRequest singleton;
    return singleton;
}

// -------------------SendVariableRequestPacket -------------------

bool SendVariableDescriptionPacket::transition(Transaction* Transaction, Transitions transition)
{
    if(transition == RECEIVED_ACK)
    {
        Transaction->setState(SendValuePacket::getInstance());
    } else {
        return false;
    }

    return true;
}

TransactionState& SendVariableDescriptionPacket::getInstance()
{
    static SendVariableDescriptionPacket singleton;
    return singleton;
}

// ------------------- ReceiveValuePacket -------------------

bool ReceiveValuePacket::transition(Transaction* Transaction, Transitions transition)
{
    if(transition == RECEIVED_VALUE_PACKET)
    {
        Transaction->setState(ReceiveEndPacket::getInstance());
    } else {
        return false;
    }

    return true;
}

TransactionState& ReceiveValuePacket::getInstance() 
{
    static ReceiveValuePacket singleton;
    return singleton;
}

// ------------------- SendValuePacket -------------------

bool SendValuePacket::transition(Transaction* Transaction, Transitions transition)
{
    if(transition == RECEIVED_ACK)
    {
        Transaction->setState(SendEndPacket::getInstance());
    } else {
        return false;
    }

    return true;
}

TransactionState& SendValuePacket::getInstance()
{
    static SendValuePacket singleton;
    return singleton;
}

// ------------------- SendEndPacket -------------------

bool SendEndPacket::transition(Transaction* Transaction, Transitions transition)
{
    if(transition == SENT_END_PACKET)
    {
        Transaction->setState(Idle::getInstance());
    } else {
        return false;
    }

    return true;
}

TransactionState& SendEndPacket::getInstance()
{
    static SendEndPacket singleton;
    return singleton;
}

// ------------------- WaitForScreenshotRequest -------------------

bool WaitForScreenshotRequest::transition(Transaction* Transaction, Transitions transition)
{
    if(transition == RECEIVED_SCREENSHOT_REQUEST)
    {
        Transaction->setState(ReceiveScreenshotData::getInstance());
    } else {
        return false;
    }

    return true;
}

TransactionState& WaitForScreenshotRequest::getInstance()
{
    static WaitForScreenshotRequest singleton;
    return singleton;
}

// ------------------- ReceiveScreenshotData -------------------

bool ReceiveScreenshotData::transition(Transaction* Transaction, Transitions transition)
{
    if(transition == RECEIVED_SCREENSHOT_DATA)
    {
        Transaction->setState(ReceiveEndPacket::getInstance());
    } else {
        return false;
    }

    return true;
}

TransactionState& ReceiveScreenshotData::getInstance()
{
    static ReceiveScreenshotData singleton;
    return singleton;
}

// -------------------ReceiveEndPacket -------------------

bool ReceiveEndPacket::transition(Transaction* Transaction, Transitions transition)
{
    if(transition == RECEIVED_END_PACKET)
    {
        Transaction->setState(Idle::getInstance());
    } else {
        return false;
    }

    return true;
}

TransactionState& ReceiveEndPacket::getInstance()
{
    static ReceiveEndPacket singleton;
    return singleton;
}