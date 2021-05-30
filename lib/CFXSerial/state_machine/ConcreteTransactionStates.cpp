#include "ConcreteTransactionStates.h"

void Idle::received_data_wake_up(Transaction* Transaction)
{

}

void Idle::received_screenshot_wake_up(Transaction* Transaction)
{
    // Idle -> WaitForScreenshotRequest
 	Transaction->setState(WaitForScreenshotRequest::getInstance());
}

TransactionState& Idle::getInstance()
{
    static Idle singleton;
    return singleton;
}

void WaitForDataRequest::received_request_packet(Transaction* Transaction)
{
    // WaitForDataRequest -> SendVariableDescriptionPacket
    Transaction->setState(SendVariableDescriptionPacket::getInstance());
}

void WaitForDataRequest::received_variable_description_packet(Transaction* Transaction)
{
    // WaitForDataRequest->ReceiveValuePacket
    Transaction->setState(ReceiveValuePacket::getInstance());
}

TransactionState& WaitForDataRequest::getInstance()
{
    static WaitForDataRequest singleton;
    return singleton;
}

void SendVariableDescriptionPacket::received_ack(Transaction* Transaction)
{
    // SendVariableDescriptionPacket->SendValuePacket
    Transaction->setState(SendValuePacket::getInstance());
}

TransactionState& SendVariableDescriptionPacket::getInstance()
{
    static SendVariableDescriptionPacket singleton;
    return singleton;
}

void ReceiveValuePacket::received_value_packet(Transaction* Transaction)
{
    // ReceiveValuePacket->ReceiveEndPacket
    Transaction->setState(ReceiveEndPacket::getInstance());
}

TransactionState& ReceiveValuePacket::getInstance() 
{
    static ReceiveValuePacket singleton;
    return singleton;
}

void SendValuePacket::received_ack(Transaction* Transaction)
{
    // SendValuePacket -> SendEndPacket
    Transaction->setState(SendEndPacket::getInstance());
}

TransactionState& SendValuePacket::getInstance()
{
    static SendValuePacket singleton;
    return singleton;
}

TransactionState& SendEndPacket::getInstance()
{
    static SendEndPacket singleton;
    return singleton;
}

void SendEndPacket::sent_end_packet(Transaction* Transaction)
{
    // SendEndPacket -> Idle
    Transaction->setState(Idle::getInstance());
}

void WaitForScreenshotRequest::received_screenshot_request(Transaction* Transaction)
{
    // WaitForScreenshotRequest -> ReceiveScreenshotData
 	Transaction->setState(ReceiveScreenshotData::getInstance());
}

TransactionState& WaitForScreenshotRequest::getInstance()
{
    static WaitForScreenshotRequest singleton;
    return singleton;
}

void ReceiveScreenshotData::received_screenshot_data(Transaction* Transaction)
{
    // ReceiveScreenshotData -> ReceiveEndPacket
    Transaction->setState(ReceiveEndPacket::getInstance());
}

TransactionState& ReceiveScreenshotData::getInstance()
{
    static ReceiveScreenshotData singleton;
    return singleton;
}

void ReceiveEndPacket::received_end_packet(Transaction* Transaction)
{
    // ReceiveEndPacket -> Idle
    Transaction->setState(Idle::getInstance());
}

TransactionState& ReceiveEndPacket::getInstance()
{
    static ReceiveEndPacket singleton;
    return singleton;
}