#include "matrices.h"

MatrixStorage::MatrixStorage()
{
}

bool MatrixStorage::clear(char variableName) 
{
    storage[variableName].matrix_data.clear();
    storage[variableName].rows = 0;
    storage[variableName].cols = 0;
    storage[variableName].isValid = false;

    return true;
}

bool MatrixStorage::init(char variableName, uint8_t rows, uint8_t cols, bool isComplex)
{
    if (is_initialised(variableName))
    {
        return false;
    }

    MatrixData matrix_data;
    matrix_data.rows = rows;
    matrix_data.cols = cols;
    matrix_data.isComplex = isComplex;
    storage[variableName] = matrix_data;

    return true;
}

bool MatrixStorage::append(char variableName, ComplexValue value)
{
    if (!is_initialised(variableName))
    {
        return false;
    }
    else
    {
        storage[variableName].matrix_data.push_back(value);
        return true;
    }
}


MatrixData MatrixStorage::get_all(char variableName)
{
    MatrixData value;
    if (!is_initialised(variableName))
    {
        value.isValid = false;
    }
    else {
        value = storage[variableName];
        value.isValid = true;
    }

    return value;
}

bool MatrixStorage::is_initialised(char variableName)
{
    return (storage.find(variableName) != storage.end());
}
