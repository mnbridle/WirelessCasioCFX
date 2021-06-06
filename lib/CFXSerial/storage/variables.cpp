#include "variables.h"

double VariableStorage::get(char variableName)
{
    return storage[variableName];
}

void VariableStorage::set(char variableName, double value)
{
    storage[variableName] = value;
}