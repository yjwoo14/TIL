#include <cmath>
#include <iostream>

int main(int argc, const char *argv[])
{
	std::cout << NAN << " " << std::isnan(NAN) << std::endl;
	std::cout << -NAN << " " << std::isnan(-NAN) << std::endl;
	std::cout << std::signbit(NAN) << " " << std::signbit(-NAN) << std::endl;
	return 0;
}
