#include <iostream>

template <typename Functor>
void function(Functor f = [](const int &){}) {
	f(1);
}

int main(int argc, const char *argv[])
{
	function();
	return 0;
}
