#include "thegame.hpp"
#include <iostream>
#include <sstream>
#include <functional>

template<typename T>
std::string print(const T& t)
{
    return std::to_string(t);
}

template<typename T>
std::string print(const std::vector<T>& v)
{
    std::stringstream ss;
    for(auto&& i : v)
    {
        ss << i << ',';
    }
    std::string s(ss.str());
    s.pop_back();
    return s;
}

template<typename T>
void assert_equal(const T& expected, const T& actual)
{
	if(expected != actual)
	{
		std::cerr << "\nERROR: [" << print(expected) << "] != [" << print(actual) << "]" << std::endl;
	}
}

void test_split_space()
{
	assert_equal({ "a", "b", "c", "d" }, split("a b\tc   d"));
}

int main()
{
    std::vector<std::function<void()>> tests{
        test_split_space
    };

    for(auto f : tests)
    {
        f();
        std::cout << ".";
    }
    return 0;
}