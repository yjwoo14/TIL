#include <iostream>
#include <limits>

using namespace std;

void trial1(const size_t n) {
	size_t a = n;
	int ia = (int)(a & 0xFFFFFFFF);
	a >>= 32;
	int ib = (int)(a & 0xFFFFFFFF);

	// HERE we have an error when the sign bit of ib is set.
	// the 32 significant bits will be set in that case
	size_t b = ib;
	b <<= 32;
	b |= ia;
	std::cout << "Trial 1: " << n << " == " << b << " ? " << (n == b) << std::endl;
}

void trial2(const size_t n) {
	size_t a = n;

	// this is implementation defined as the number cannot be represented in signed.
	// but seems works in gcc and clang
	int ia = static_cast<int>(a & 0xFFFFFFFF);
	a >>= 32;
	int ib = static_cast<int>(a & 0xFFFFFFFF);

	// http://stackoverflow.com/questions/1751346/interpret-signed-as-unsigned
	// In standard (4.7/2)
	// If the destination type is unsigned, the resulting value is the least
	// unsigned integer congruent to the source integer (modulo 2n where n is
	// the number of bits used to represent the unsigned type). [Note: In a
	// two’s complement representation, this conversion is conceptual and there
	// is no change in the bit pattern (if there is no truncation). ]
	// 
	// Also reinterpret_cast is implementation defined..(5.2.10/3)
	// "The mapping performed by reinterpret_cast is implementation-defined.
	// [Note: it might, or might not, produce a representation different from
	// the original value. ]
	size_t b = static_cast<uint32_t>(ib);
	b <<= 32;
	b |= static_cast<uint32_t>(ia);
	std::cout << "Trial 2: " << n << " == " << b << " ? " << (n == b) << std::endl;
}

int main(int argc, const char *argv[])
{
	
	for (size_t i = std::numeric_limits<size_t>::max() ; i > 0 ; i >>= 1) {
		trial1(i);
		trial2(i);
	}
	return 0;
}
