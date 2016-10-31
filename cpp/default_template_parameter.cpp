#include <iostream>

template <typename T>
struct A {
	T a;
};

template <typename T = int>
using B = A<T>;

int main(int argc, const char *argv[])
{
	B<> b;
	return 0;
}
