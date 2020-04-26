#include "thegame.cpp"
#include <iostream>

template<typename T>
void assert_equal(const T& expected, const T& actual)
{
	if(expected != actual)
	{
		std::cerr << "ERROR: [" << expected << "] != [" << actual << "]" << std::endl;
	}
}

void test_split_space()
{
	assert_equal({ "a", "b", "c" }, split("a b\tc"));
}