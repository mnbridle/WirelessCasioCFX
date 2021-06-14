#include <map>
#include <codecs/codec.hpp>

class VariableStorage
{
    public:
        VariableStorage();
        ComplexValue get(char variableName);
        void set(char variableName, ComplexValue value);
    private:
        std::map<char, ComplexValue> storage;
};