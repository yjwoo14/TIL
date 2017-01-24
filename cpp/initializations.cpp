#include <iostream>

struct foo {
	foo() = default;
	int a;
};

struct bar {
	bar();
	int b;
};

bar::bar() = default; // user-provided constructor in this case

int main(int argc, const char *argv[])
{
	foo a{}; // value initialization (a.a is zero-initalized)
	bar b{}; // value initialization (b.b is not zero-intialized)
	std::cout << a.a << std::endl;
	std::cout << b.b << std::endl;
	return 0;
}
