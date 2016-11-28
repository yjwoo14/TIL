#include <iostream>
#include <tuple>

int main(int argc, const char *argv[])
{
	int a,b,c,d,e;
	std::tie(a,b,c,d,e) = std::make_tuple(1,2,3,4,5);
	return 0;
}
