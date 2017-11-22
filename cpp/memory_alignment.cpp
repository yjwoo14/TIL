#include <iostream>

// Tested machine has 32bit alignment
struct NoPaddingCharOnlyStructure {
	char a1,a2,a3;
};
struct A {
	char a1;
	int b;
	char c1;
};

struct alignas(8) B {
	char a0;
	int b;
	char c1;
};

struct C {
	bool a;
	A b;
};

struct D {
	bool a;
	B b;
};

int main(int argc, const char *argv[])
{
	std::cout << "NoPaddingCharOnlyStructure: " << sizeof(NoPaddingCharOnlyStructure) << std::endl;
	std::cout << "A: " << sizeof(A) << std::endl;
	std::cout << "B: " << sizeof(B) << std::endl;
	std::cout << "C: " << sizeof(C) << std::endl;
	std::cout << "D: " << sizeof(D) << std::endl;
	return 0;
}
