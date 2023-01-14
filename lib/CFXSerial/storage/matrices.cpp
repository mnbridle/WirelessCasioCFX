#include "matrices.h"

MatrixStorage::MatrixStorage()
{
}

bool MatrixStorage::clear(char variableName) 
{
    storage.erase(variableName);
    return true;
}

bool MatrixStorage::init(char variableName, uint8_t rows, uint8_t cols, bool isComplex)
{
    if (is_initialised(variableName))
    {
        clear(variableName);
    }

    MatrixData matrix_data;
    matrix_data.rows = rows;
    matrix_data.cols = cols;
    matrix_data.isComplex = isComplex;
    matrix_data.isValid = false;
    matrix_data.receivedFromCFX = false;

    storage[variableName] = matrix_data;

    Serial.print("Successfully did an init for ");
    Serial.println(variableName);

    return true;
}

bool MatrixStorage::wasReceivedFromCFX(char variableName)
{
    if (!is_initialised(variableName))
    {
        return false;
    }

    return storage[variableName].receivedFromCFX;
}

bool MatrixStorage::receivedFromCFX(char variableName, bool receivedFromCFX)
{
    if (!is_initialised(variableName))
    {
        return false;
    }

    storage[variableName].receivedFromCFX = receivedFromCFX;
    return true;
}

bool MatrixStorage::append(char variableName, ComplexValue value)
{
    if (!is_initialised(variableName))
    {
        return false;
    }

    storage[variableName].matrix_data.push_back(value);
    storage[variableName].isValid = size(variableName) == rows(variableName) * cols(variableName);

    return true;
}

bool MatrixStorage::append_matrix(char variableName, MatrixData matrix)
{
    clear(variableName);
    storage[variableName] = matrix;
    return true;
}

MatrixData MatrixStorage::get_all(char variableName)
{
    MatrixData value;
    if (is_initialised(variableName))
    {
        value = storage[variableName];
    }

    // Clear the memory after it's been read back
    Serial.print("Clearing mat ");
    Serial.println(variableName);
    clear(variableName);

    return value;
}

bool MatrixStorage::is_valid(char variableName)
{
    if (!is_initialised(variableName))
    {
        return false;
    }

    return storage[variableName].isValid;
}

bool MatrixStorage::is_complex(char variableName)
{
    if (!is_initialised(variableName))
    {
        return false;
    }
    
    return storage[variableName].isComplex;    
}

uint8_t MatrixStorage::rows(char variableName)
{
    if (!is_initialised(variableName))
    {
        return 0;
    }

    return storage[variableName].rows;    
}

uint8_t MatrixStorage::cols(char variableName)
{
    if (!is_initialised(variableName))
    {
        return 0;
    }

    return storage[variableName].cols;    
}

size_t MatrixStorage::size(char variableName)
{
    if (!is_initialised(variableName))
    {
        return 0;
    }

    return storage[variableName].matrix_data.size();
}

bool MatrixStorage::is_initialised(char variableName)
{
    return (storage.find(variableName) != storage.end());
}
