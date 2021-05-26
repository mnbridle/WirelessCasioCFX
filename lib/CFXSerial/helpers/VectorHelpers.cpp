#include <vector>
#include <string>

template<typename T>
std::vector<T> slice(std::vector<T> &v, int m, int n)
{
    std::vector<T> vec;
    std::copy(v.begin() + m, v.begin() + n + 1, std::back_inserter(vec));
    return vec;
}

template<typename T>
std::string slice_str(std::vector<T> &v, int m, int n)
{
    std::vector<int> vec = slice(v, m, n);
    std::string vec_str(vec.begin(), vec.end());
    return vec_str;
}