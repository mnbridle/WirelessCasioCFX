#pragma once
#include "TransactionState.h"
#include "Transitions.h"

// Forward declaration to resolve circular dependency/include
class TransactionState;

class Transaction
{
public:
	Transaction();
	inline TransactionState* getCurrentState() const { return currentState; }

	void trigger();

	// This is where the magic happens
	void setState(TransactionState& newState);

private:
	TransactionState* currentState;
};