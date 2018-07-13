#include <iostream>

template <typename T>
class A {
public:
	A(T t = T()) : t(std::move(t)) {}

	void f(int a) {
		t(*this, a);
	}

	int g() {
		return a;
	}

private:
	friend T;
	int a;
	T t;
};

struct S {
	template <typename T>
	void operator()(T & t, int x) {
		t.a = x;
	}
};

int main(int argc, const char *argv[])
{
	A<S> a;
	a.f(3);
	std::cout << a.g() << std::endl;
	return 0;
}
