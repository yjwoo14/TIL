#include <iostream>
#include <memory>

struct Fruit {
	Fruit() {
		std::cout << "constructed" << std::endl;
	}
	Fruit(const Fruit &) {
		std::cout << "copy" << std::endl;
	}
	Fruit(Fruit &&) {
		std::cout << "move" << std::endl;
	}
};

struct Durian: Fruit {
	double x = 1;
};

Fruit call1() {
	Fruit x;
	// Don't do move here unless you know what you are doing. RVO will make
	// implicit move! (only for return x;)
	return x;
}

Fruit call2(Fruit x) {
	return x;
}

Fruit call3() {
	Durian x;
	return x;
}

Fruit call4(Fruit x) {
	return std::move(x);
}

Fruit call5() {
	Durian x;
	return std::move(x);
}

void eat(Fruit x) {
	std::cout << "eatting" << std::endl;
	return;
}

std::unique_ptr<Fruit> call6() {
	auto p = std::make_unique<Durian>();
	return p;
}

Fruit call7(Fruit &&x) {
	return x; // This supposed to be implicit move by c++20?
}

int main(int argc, const char *argv[])
{
	{
		// copy elision
		std::cout << "Call 1" << std::endl;
		Fruit x = call1();
	}
	{
		// no copy elision
		std::cout << "Call 2" << std::endl;
		Fruit x = call2(Fruit());
	}
	{
		// copy elision
		std::cout << "eat" << std::endl;
		eat(call1());
	}
	{
		// no copy elision copy
		std::cout << "Call 3" << std::endl;
		Fruit x = call3();
	}
	{
		// no copy elision but move
		std::cout << "Call 4" << std::endl;
		Fruit x = call4(Fruit());
	}
	{
		// no copy elision move
		std::cout << "Call 5" << std::endl;
		Fruit x = call5();
	}
	{
		// copy elision
		std::cout << "Call 6" << std::endl;
		auto x = call6();
	}
	{
		// no copy elision copy
		std::cout << "Call 7" << std::endl;
		auto x = call7(Fruit());
	}

	return 0;
}
