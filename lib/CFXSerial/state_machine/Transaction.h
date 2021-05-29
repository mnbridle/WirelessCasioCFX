#pragma once
#include "TransactionState.h"

// Forward declaration to resolve circular dependency/include
class TransactionState;

class Transaction
{
public:
	Transaction();
	inline TransactionState* getCurrentState() const { return currentState; }
	void toggle();
	// This is where the magic happens
	void setState(TransactionState& newState);

private:
	TransactionState* currentState;
};