#include <map>
#include <vector>
#include <codecs/codec.hpp>

struct MatrixData
{
    std::vector<ComplexValue> matrix_data;
    bool isValid;
    bool isComplex;
    uint8_t rows;
    uint8_t cols;
};

class MatrixStorage
{
    public:
        MatrixStorage();
        MatrixData get_all(char variableName);
        bool init(char variableName, uint8_t rows, uint8_t cols, bool isComplex);
        bool append(char variableName, ComplexValue value);
        bool clear(char variableName);
    private:
        bool is_initialised(char variableName);
        std::map<char, MatrixData> storage;
};
