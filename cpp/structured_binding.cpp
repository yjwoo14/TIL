#include <map>
#include <iostream>

std::map<std::string, int> get_map() {
	return {
		{"Hi", 1},
		{"This", 2},
		{"Is", 3},
		{"An", 4},
		{"Example", 5}
	};
}

int main(int argc, const char *argv[])
{
	auto map = get_map();
	
	for (auto &[key, value]: map) {
		// key = "Test"; // key is const so not allowed
		value = 10;
	}

	for (auto &[key, value]: map) {
		std::cout << key << " " << value << std::endl;
	}


	return 0;
}
