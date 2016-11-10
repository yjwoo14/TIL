#include <iostream>
#include <vector>

int main(int argc, const char *argv[])
{
	std::vector<int> V(10);
	auto f = [](auto & vector, auto i) 
		// Need std::decay as type definition inside class cannot be uased from
		// reference type 
		-> typename std::decay<decltype(vector)>::type::value_type & {
		return vector[i];
	};
	V[2] = 1;
	std::cout << f(V, 2) << std::endl;

	f(V, 2) = 2;
	std::cout << V[2] << std::endl;
	return 0;
}
