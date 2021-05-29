#pragma once
#include "TransactionState.h"
#include "Transaction.h"

/* State Transition Diagram



                                                                  ____________________________________________________________________________________
                                                                 /                                                                                    \
                                                                 |                                                                                     \
                                                              _|IDLE|______________________________                                                     \
                                                            /                                      \                                                    |
                                   received_data_wake_up() /                                        \  received_screenshot_wake_up()                    |
                                                          /                                          \                                                  |
                                               |WaitForDataRequest|                       |WaitForScreenshotRequest|                                    |
                                                       / \                                            |                                                 |
                            received_request_packet() /   \ received_variable_description_packet()    | received_screenshot_request()                   |
                                                     /     \                                          |                                                 |
                                                    /       \                                         |                                                 |
                         |SendVariableDescriptionPacket| |ReceiveValuePacket|              |ReceiveScreenshotData|                                      |
                                        |                         |                                   |                                                 |
                         received_ack() | received_value_packet()  \                                  | received_screenshot_data()                      |
                                        |                           \                                 |                                                 |
                                 |SendValuePacket|                   \_______________________|ReceiveEndPacket|                                         /
                                        |                                                             |                                                /
                         received_ack() |                                                             | received_end_packet()                         /           
                                        |                                                              \____________________________________________ /
                                   |SendEndPacket|                                                                                                  /
                                        \__________________________________________________________________________________________________________/

*/

class TransactionState
{
    public:
        void enter(Transaction* Transaction) {}
	    void exit(Transaction* Transaction) {}
        static TransactionState& getInstance();
};

class Idle : public TransactionState
{
public:
	void received_data_wake_up(Transaction* Transaction);
    void received_screenshot_wake_up(Transaction* Transaction);

private:
	Idle() {}
	Idle(const Idle& other);
	Idle& operator=(const Idle& other);
};

class WaitForDataRequest : public TransactionState
{
public:
	void received_request_packet(Transaction* Transaction);
    void received_variable_description_packet(Transaction* Transaction);

private:
	WaitForDataRequest() {}
	WaitForDataRequest(const WaitForDataRequest& other);
	WaitForDataRequest& operator=(const WaitForDataRequest& other);
};

class SendVariableDescriptionPacket : public TransactionState
{
public:
	void received_ack(Transaction* Transaction);

private:
	SendVariableDescriptionPacket() {}
	SendVariableDescriptionPacket(const SendVariableDescriptionPacket& other);
	SendVariableDescriptionPacket& operator=(const SendVariableDescriptionPacket& other);
};

class ReceiveValuePacket : public TransactionState
{
public:
	void received_value_packet(Transaction* Transaction);

private:
	ReceiveValuePacket() {}
	ReceiveValuePacket(const ReceiveValuePacket& other);
	ReceiveValuePacket& operator=(const ReceiveValuePacket& other);
};

class SendValuePacket : public TransactionState
{
public:
	void received_ack(Transaction* Transaction);

private:
	SendValuePacket() {}
	SendValuePacket(const SendValuePacket& other);
	SendValuePacket& operator=(const SendValuePacket& other);
};

class SendEndPacket : public TransactionState
{
public:
	
private:
	SendEndPacket() {}
	SendEndPacket(const SendEndPacket& other);
	SendEndPacket& operator=(const SendEndPacket& other);
};

class WaitForScreenshotRequest : public TransactionState
{
public:
	void received_screenshot_request(Transaction* Transaction);

private:
	WaitForScreenshotRequest() {}
	WaitForScreenshotRequest(const WaitForScreenshotRequest& other);
	WaitForScreenshotRequest& operator=(const WaitForScreenshotRequest& other);
};

class ReceiveScreenshotData : public TransactionState
{
public:
	void received_screenshot_data(Transaction* Transaction);

private:
	ReceiveScreenshotData() {}
	ReceiveScreenshotData(const ReceiveScreenshotData& other);
	ReceiveScreenshotData& operator=(const ReceiveScreenshotData& other);
};

class ReceiveEndPacket : public TransactionState
{
public:
	void received_end_packet(Transaction* Transaction);

private:
	ReceiveEndPacket() {}
	ReceiveEndPacket(const ReceiveEndPacket& other);
	ReceiveEndPacket& operator=(const ReceiveEndPacket& other);
};