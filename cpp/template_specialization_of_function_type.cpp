#include <iostream>

template <typename T>
struct S {
	bool operator()() {
		std::cout << "General type." << std::endl;
	}
};

template <typename R>
struct S<R()> {
	bool operator()() {
		std::cout << "Function type without argument." << std::endl;
	}
};

template <typename R, typename ...ARGS>
struct S<R(ARGS...)> {
	bool operator()() {
		std::cout << "Function type with argument." << std::endl;
	}
};

template <typename R, typename T>
struct S<R(T::*)()> {
	bool operator()() {
		std::cout << "Member function" << std::endl;
	}
};

template <typename R, typename T, typename A1, typename ...ARGS>
struct S<R(T::*)(A1, ARGS...)> {
	bool operator()() {
		std::cout << "Member function with argument " << typeid(A1).name() << std::endl;
	}
};


void foo() {}
void bar(int) {}

struct Functor {
	void foo() {}
	void operator()(const S<int> &){};
};

int main(int argc, const char *argv[])
{
	S<int> one;
	one();
	S<decltype(foo)> two;
	two();
	S<decltype(bar)> three;
	three();

	Functor f;
	S<decltype(&Functor::foo)> four;
	four();
	S<decltype(&Functor::operator())> five;
	five();

	return 0;
}
