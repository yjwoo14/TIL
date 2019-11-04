#include <iostream>
#include <vector>

int main() {
	std::vector<std::string> vec{"abc", "def", "ghi"};

	size_t out = 0;
	for (size_t i = 0 ; i < vec.size() ; ++i) {
		if (vec[i] == "def") continue;
		vec[out++] = std::move(vec[i]); // error! std::move to itself.
	}
	vec.resize(out);

	for (const auto & str: vec)
		std::cout << str << std::endl;
}
