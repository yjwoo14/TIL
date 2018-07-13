#include <iostream>


struct A {
	void f() { std::cout << "A" << std::endl; }
	void g() { std::cout << "A" << std::endl; }
};

struct B {
	void f() { std::cout << "B" << std::endl; }
	void g() { std::cout << "B" << std::endl; }
};

template <typename T1, typename T2>
struct C : T1, T2 {
	using T1::f;
	using T2::g;
};

int main(int argc, const char *argv[])
{

	C<A, B> c;
	c.f();
	c.g();
	
	return 0;
}
