#pragma once

#include <map>
#include <codecs/CasioCFX/CasioCFX.hpp>

struct VariableData
{
    ComplexValue data;
    bool isValid;
    bool isComplex;
};
class VariableStorage
{
    public:
        VariableStorage();
        bool init(char variableName, bool isComplex);
        void set(char variableName, ComplexValue value);
        void clear(char variableName);
        VariableData get(char variableName);

    private:
        std::map<char, VariableData> storage;
        bool is_initialised(char variableName);
};
