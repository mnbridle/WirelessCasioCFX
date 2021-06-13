#include <map>
#include <vector>

class MatrixStorage
{
    public:
        MatrixStorage();
        std::vector<double> get(char variableName);
        bool set(char variableName, std::vector<double> value);
        bool clear(char variableName);
    private:
        std::map<char, std::vector<double>> storage;
};