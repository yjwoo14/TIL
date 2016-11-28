#include <iostream>

using namespace std;

void f(int = 1) {
	std::cout << "ok" << std::endl;
}

int main(int argc, const char *argv[])
{
	f();
	f(0);
	return 0;
}
