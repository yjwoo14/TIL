#include <iostream>
#include <functional>

struct BinaryOperator {
	template <typename T>
	bool operator()(T const & a, T const & b) {
		return std::less<T>()(a, b);
	}
};

int main(int argc, const char *argv[])
{
	int a = 1, b = 2;

	BinaryOperator op1;
	std::binary_negate<BinaryOperator> op2;
	std::cout << op1(a, b) << " " << op2(a, b) << std::endl;
	return 0;
}
