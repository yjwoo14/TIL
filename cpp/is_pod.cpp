#include <iostream>

struct POD {
	int a;
};

struct ISPOD {
	int a = 1;
};

struct NOTPOD {
	int a;
	NOTPOD():a(1){}
};

int main(int argc, const char *argv[])
{
	
	std::cout << std::is_pod<POD>::value << " " << std::is_pod<ISPOD>::value << " " << std::is_pod<NOTPOD>::value << std::endl;
	std::cout << std::is_trivially_copyable<POD>::value << " " << std::is_trivially_copyable<ISPOD>::value << " " << std::is_trivially_copyable<NOTPOD>::value << std::endl;
	return 0;
}
