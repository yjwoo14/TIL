#include <iostream>

struct A {
	size_t size;
	char data[];
};

int main(int argc, const char *argv[])
{
	// sizeof a structure containing zero length array doesn't count the size of the zero length array
	std::cout << sizeof(A) << std::endl;
	return 0;
}
