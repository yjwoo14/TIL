#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

template <typename T, typename Functor>
void forEach1(const T & t, Functor f) {
	for (const auto &x : t)
		f(x);
}

template <typename T, typename Functor>
void forEach2(const T & t, Functor f) {
	forEach1(t, f);
}

struct X {
	int a;
	template <typename T>
		int &x(int, T) { return a; }
	template <typename T>
		const int &x(int, T) const { return a; }
	void f() {
		auto k = [this](int){ x(3, 5) = 3; reinterpret_cast<const X*>(this)->x(5,5); };
		std::vector<int> t;
		forEach2(t, k);
	}
};

int main(int argc, const char *argv[])
{
	const X x = {1};
	const_cast<X &>(x).f();
	std::cout << x.x(3, 6) << std::endl;
	return 0;
}
