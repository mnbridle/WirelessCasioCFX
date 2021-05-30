#pragma once
#include "Transaction.h"
#include "Transitions.h"

// Forward declaration to resolve circular dependency/include
class Transaction;

class TransactionState
{
public:
	virtual void enter(Transaction* Transaction) = 0;
	virtual void trigger(Transaction* Transaction) = 0;
	virtual void exit(Transaction* Transaction) = 0;
	virtual ~TransactionState() {}
};