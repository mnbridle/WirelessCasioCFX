#include "Transaction.h"
#include "ConcreteTransactionStates.h"

Transaction::Transaction()
{
	// All Transactions are initially turned off
	currentState = &TransactionOff::getInstance();
}

void Transaction::setState(TransactionState& newState)
{
	currentState->exit(this);  // do stuff before we change state
	currentState = &newState;  // actually change states now
	currentState->enter(this); // do stuff after we change state
}

void Transaction::toggle()
{
	// Delegate the task of determining the next state to the current state
	currentState->toggle(this);
}