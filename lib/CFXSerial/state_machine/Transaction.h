#pragma once
#include "TransactionState.h"

enum Transitions {
	RECEIVED_DATA_WAKE_UP,
	RECEIVED_SCREENSHOT_WAKE_UP,
	RECEIVED_REQUEST_PACKET,
	RECEIVED_VARIABLE_DESCRIPTION_PACKET,
	RECEIVED_SCREENSHOT_REQUEST,
	RECEIVED_ACK,
	RECEIVED_VALUE_PACKET,
	RECEIVED_SCREENSHOT_DATA,
	RECEIVED_END_PACKET,
	SENT_END_PACKET
};

// Forward declaration to resolve circular dependency/include
class TransactionState;

class Transaction
{
public:
	Transaction();
	inline TransactionState* getCurrentState() const { return currentState; }

	void change_state(Transitions transition);

	// This is where the magic happens
	void setState(TransactionState& newState);

private:
	TransactionState* currentState;
};