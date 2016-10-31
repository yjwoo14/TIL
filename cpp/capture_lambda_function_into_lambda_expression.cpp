#include <iostream>

int main(int argc, const char *argv[])
{
	auto f1 = [](const auto x) {
		std::cout << x << std::endl;
	};

	auto f2 = [f1](const auto x) {
		f1(x);
	};

	f2(30);
	return 0;
}
