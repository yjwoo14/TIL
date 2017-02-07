#include <iostream>
#include <limits>

int main(int argc, const char *argv[])
{
	{
		int a;
		int b = a; // referencing uninitialized variable (a) is undefined behavior
		std::cout << a << " " << b << std::endl;
	}

	{
		const int x = 2;
		// §7.1.5.1/4 says Except that any class member declared mutable (7.1.1) can be modified,
		// any attempt to modify a const object during its lifetime (3.8) results in undefined behavior
		const_cast<int &>(x) = 1; 
		std::cout << x << std::endl;

		// Further playing around.
		const int &y = x;
		const_cast<int &>(y) = 1;

		// Seems gcc/clang implementation replace "x" usage by 2 in compile time ;)
		std::cout << x << " " << y << std::endl;
		std::cout << &x << " " << &y << std::endl;

		const int *k = &x, *l = &y;
		std::cout << x << " " << *k << " " << *l << std::endl;
	}

	{
		int x = std::numeric_limits<int>::max();
		++x; // signed integer overflow is undefined behavior
	}


	return 0;
}
