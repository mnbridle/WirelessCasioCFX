#include <map>

class VariableStorage
{
    public:
        double get(char variableName);
        void set(char variableName, double value);
    private:
        std::map<char, double> storage;
};