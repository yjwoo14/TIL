#include <iostream>

int main(int argc, const char *argv[])
{
	// In general, shifting 32 bit interger value by more than 31 bit is
	// `undefined behavior` in C/C++.
	// The (possible) reason of it is that, in some architecture shifting 32
	// bit integer instruction only uses 5 bits in the second operand.

	// See the following
	uint32_t o1 = 0xffffffff;
	uint32_t o2 = 32;
	uint32_t o3 = (o1 << o2); 
	// IA64 ->    ^^^^^^^^^^ = o1 << (32 % 32 = 0)
	// o3 becomes just o1 without optimization.

	// The g++-5 compiler is clever enough to optimize this statement so that
	// o3 becomes 0
	std::cout << o3 << std::endl;
	return 0;
}
