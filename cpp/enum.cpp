#include <iostream>
enum class Channel : char { Red = 1, Green, Blue };

int main(int argc, const char *argv[])
{
	Channel c = Channel::_from_string("Green");
	std::cout << c._to_string() << std::endl;
	return 0;
}

