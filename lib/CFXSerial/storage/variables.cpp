#include "variables.h"

VariableStorage::VariableStorage()
{
}

double VariableStorage::get(char variableName)
{
    double value;
    if (storage.find(variableName) != storage.end())
    {
        value = storage[variableName];
    }
    else
    {
        value = 1.234567890123;
    }
    return value;
}

void VariableStorage::set(char variableName, double value)
{
    storage[variableName] = value;
}