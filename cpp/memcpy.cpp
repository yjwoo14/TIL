#include <iostream>
#include <cstring>

struct Item {
	int x;
};

int main(int argc, const char *argv[])
{
	std::aligned_storage<sizeof(Item), sizeof(Item)>::type buff;

	Item f, t;
	f.x = 0;

	static_assert(std::is_trivially_copyable<Item>::value, "Item should be trivially copyable");
	std::memcpy(&t, &f, sizeof(Item));
	std::cout << t.x << std::endl; // this is ok

	std::memcpy(&buff, &f, sizeof(Item));
	std::memcpy(&t, &buff, sizeof(Item));
	std::cout << t.x << std::endl; // this is ok

	std::memcpy(&buff, &f, sizeof(Item));
	std::cout << reinterpret_cast<Item &>(buff).x << std::endl; // this is undefined behavior

	return 0;
}
