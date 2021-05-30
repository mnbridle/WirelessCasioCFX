#include "Transaction.h"
#include "ConcreteTransactionStates.h"

Transaction::Transaction()
{
	// All Transactions are initially turned off
	currentState = &Idle::getInstance();
}

void Transaction::setState(TransactionState& newState)
{
	currentState->exit(this);  // do stuff before we change state
	currentState = &newState;  // actually change states now
	currentState->enter(this); // do stuff after we change state
}
