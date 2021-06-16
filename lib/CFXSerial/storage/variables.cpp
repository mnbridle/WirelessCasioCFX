#include "variables.h"

VariableStorage::VariableStorage()
{
}

bool VariableStorage::init(char variableName, bool isComplex)
{
    if (is_initialised(variableName))
    {
        return false;
    }

    VariableData value;
    value.isComplex = isComplex;
    value.isValid = true;
    value.data.real_part = 0;
    value.data.imag_part = 0;
    storage[variableName] = value;
    return true;
}

VariableData VariableStorage::get(char variableName)
{
    VariableData value;
    if (is_initialised(variableName))
    {
        value = storage[variableName];
        value.isValid = true;
    }
    else
    {
        value.isValid = false;
    }
    return value;
}

void VariableStorage::set(char variableName, ComplexValue value)
{
    storage[variableName].data = value;
}

bool VariableStorage::is_initialised(char variableName)
{
    return (storage.find(variableName) != storage.end());
}