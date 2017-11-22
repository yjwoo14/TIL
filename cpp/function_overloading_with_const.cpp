#include <iostream>

using namespace std;

void funct(int &) {
	std::cout << "Int reference type function" << std::endl;
}

void funct(int const &) {
	std::cout << "Int const reference type function" << std::endl;
}

/*
// This function conflicts with the funct(int &)
void funct(int) {
	std::cout << "Int const reference type function" << std::endl;
}
*/

int main(int argc, const char *argv[])
{
	
	int a = 3;
	const int b = 3;
	funct(3);
	funct(a);
	funct(b);
	return 0;
}
