#include "variables.h"

VariableStorage::VariableStorage()
{
}

ComplexValue VariableStorage::get(char variableName)
{
    ComplexValue value;
    if (storage.find(variableName) != storage.end())
    {
        value = storage[variableName];
    }
    else
    {
        value.isValid = false;
    }
    return value;
}

void VariableStorage::set(char variableName, ComplexValue value)
{
    storage[variableName] = value;
}