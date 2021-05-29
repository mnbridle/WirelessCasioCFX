#include "ConcreteTransactionStates.h"

// void Idle::toggle(Transaction* Transaction)
// {
// 	// Off -> Low
// 	Transaction->setState(LowIntensity::getInstance());
// }

// TransactionState& Idle::getInstance()
// {
// 	static Idle singleton;
// 	return singleton;
// }

// void LowIntensity::toggle(Transaction* Transaction)
// {
// 	// Low -> Medium
// 	Transaction->setState(MediumIntensity::getInstance());
// }

// TransactionState& LowIntensity::getInstance()
// {
// 	static LowIntensity singleton;
// 	return singleton;
// }

// void MediumIntensity::toggle(Transaction* Transaction)
// {
// 	// Medium -> High
// 	Transaction->setState(HighIntensity::getInstance());
// }

// TransactionState& MediumIntensity::getInstance()
// {
// 	static MediumIntensity singleton;
// 	return singleton;
// }

// void HighIntensity::toggle(Transaction* Transaction)
// {
// 	// High -> Low
// 	Transaction->setState(Idle::getInstance());
// }

// TransactionState& HighIntensity::getInstance()
// {
// 	static HighIntensity singleton;
// 	return singleton;
// }