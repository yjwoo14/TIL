#include <iostream>
#include <memory>

void unique_ptr_trial() {
	std::unique_ptr<int> x;
	x.reset(new int());
	*x = 0;

	// HERE we cannot capture x by value since unique ptr does not have copy constructor
	auto funct = [&x](const int i) {
		*x += i;
	};

	funct(3);
	std::cout << *x << std::endl;
}

void ptr_trial() {
	int * x = new int();
	*x = 0;

	// HERE we can capture x by value 
	auto funct = [x](const int i) {
		*x += i;
	};

	funct(3);
	std::cout << *x << std::endl;
	delete x;
}

int main(int argc, const char *argv[])
{
	unique_ptr_trial();
	ptr_trial();
	return 0;
}
