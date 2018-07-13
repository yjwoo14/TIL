#include <iostream>

enum Enum {
	HELLO = 0,
	WORLD
};

template <typename T>
struct S{
	static void f() {
		std::cout << "Something else" << std::endl;
	}
};

template <>
struct S<Enum>{
	static void f() {
		std::cout << "Enum type. " << std::endl;
	}
};

int main(int argc, const char *argv[])
{
	S<int> a;
	S<Enum> b;
	a.f();
	b.f();
	
	return 0;
}
