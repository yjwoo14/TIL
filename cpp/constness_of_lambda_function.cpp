#include <iostream>
#include <vector>

int main(int argc, const char *argv[])
{

	std::vector<int> items(10, 0);
	const auto f = [&items](int value) {
		for (auto & item : items)
			item = value;
	};
	f(1);
	f(2);
	return 0;
}
