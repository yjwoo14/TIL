#include <iostream>

struct A {
	A() = default;
	A(const A&) = default;
	A(A &&) = default;
	A &operator=(const A &) = default;
	A &operator=(A &&) = default;
};

struct B {
	B() = default;
	B(const B&) = default;
	B(B &&) = default;
	B(B &) = default;

//	template <typename... T>
//	explicit B(T &&... t) {
//		static_assert(false,"No!");	
//	}

	B &operator=(B &) = default;
	B &operator=(const B &) = default;
	B &operator=(B &&) = default;
};

int main(int argc, const char *argv[])
{
	static_assert(std::is_trivially_copyable<A>::value, "It must be trivially copyable");
	static_assert(std::is_trivially_copyable<B>::value, "It must be trivially copyable");
	B b;
	auto c = b;
	return 0;
}
