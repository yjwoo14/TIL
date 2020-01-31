#include <iostream>
#include <type_traits>

// see
// https://stackoverflow.com/questions/47914612/adding-a-default-constructor-to-a-base-class-changes-sizeof-a-derived-type
//
// GCC follows the Itanium ABI for C++, which prevents from using the
// tail-padding of a POD being used for storage of derived class data members.

struct A {
	uint32_t a;
	uint32_t b;
	size_t c;
	float d;
	A() = default;
};

struct B : public A {
	uint32_t e;
	uint32_t f;
	uint32_t g;
};

struct C {
	uint32_t a;
	uint32_t b;
	size_t c;
	float d;
	C(){} // not pod type
};

struct D : public C { // C is not POD type so can reusing tail padding of C
	uint32_t e;
	uint32_t f;
	uint32_t g;
};

int main() {
	static_assert(std::is_trivially_copyable<B>::value);
	static_assert(std::is_trivially_copyable<D>::value);
	std::cout << sizeof(A) << " " << sizeof(B) << " " << sizeof(C) << " " << sizeof(D) << std::endl;
}
