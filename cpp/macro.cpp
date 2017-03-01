#include <iostream>

#define swBegin switch(value) { case 0:
#define swCase return func(__LINE__); case __LINE__:;
#define swEnd } 

int func(int value) {
	swBegin;
	while(1) {
		std::cout << "first case " << value << std::endl;
		swCase;
		std::cout << "second case " << value << std::endl;
		swCase;
		std::cout << "third case " << value << std::endl;
		return 0;
	}
	swEnd;
}

int main(int argc, const char *argv[])
{
	func(0);
	return 0;
}
