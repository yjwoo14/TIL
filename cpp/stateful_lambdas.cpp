#include <iostream>

struct X{
	int x;
	int y;
};

int main(int argc, const char *argv[])
{
	auto f = [states=0, b=10]() mutable {
		states += 1;
		return states;
	};


	std::cout << f() << std::endl;
	std::cout << f() << std::endl;
	std::cout << f() << std::endl;
	std::cout << f() << std::endl;
	std::cout << f() << std::endl;
	std::cout << reinterpret_cast<X *>(&f)->y << std::endl;
	return 0;
}
