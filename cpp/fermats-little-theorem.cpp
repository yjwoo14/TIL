#include <iostream>

template <long long M, long long B, long long P>
struct mod_pow_t {
	static constexpr auto lhs = B % M;
	static constexpr auto value = (lhs * mod_pow_t<M, B, P-1>::value) % M;
};

template <long long M, long long B>
struct mod_pow_t<M, B, 0> {
	static constexpr auto value = 1;
};

int main(int argc, const char *argv[])
{
	constexpr auto M = 460;
	std::cout << mod_pow_t<M, 2, M-1>::value << std::endl;
	return 0;
}
