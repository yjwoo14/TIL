#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <iostream>
#include <unistd.h>
#include <sys/time.h>

void (* originalhandler)(int);

void handler(int signo) {
	std::cout << "happend " << SIGINT << " " << signo << std::endl;
	originalhandler(signo);
}

int main(int argc, const char *argv[])
{

	originalhandler = signal(SIGINT, handler);
	
	for (int i = 0 ; i < 4 ; ++i) {
		std::cout << "sleep 3 sec." << std::endl;
		int a = sleep(3);
		std::cout << a << std::endl;
	}
	return 0;
}
