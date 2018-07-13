#include <iostream>
#include <limits>
#include <cassert>
#include <cstring>
#include <random>
#include <bitset>

// IEEE754 floating point
size_t constexpr float_bias = 127;
size_t constexpr float_exponent = 8;
size_t constexpr float_mantissa = 23;

size_t constexpr double_bias = 1023;
size_t constexpr double_exponent = 11;
size_t constexpr double_mantissa = 52;

size_t constexpr representable_values(size_t exponent, size_t mantissa) {
	return ((1ull << exponent) - 1) * (1ull << mantissa);
}

size_t constexpr total_representable_values(size_t exponent, size_t mantissa) {
	return 2 * representable_values(exponent, mantissa) - 1; // -0 and +0
}

size_t constexpr representation(uint32_t index, size_t exponent, size_t mantissa) {
	assert(index < representable_values(exponent, mantissa)); // "Out of range"
	size_t fraction = index % (1ull << mantissa);
	size_t exp = index / (1ull << mantissa);
	return (exp << mantissa) + fraction;
}

// Cannot be constexpr due to memcpy (possibly constexpr bit_cast solves this in the future)
float float_value_one_side(uint32_t index) {
	uint32_t rep = representation(index, float_exponent, float_mantissa);
	float ret;
	std::memcpy(&ret, &rep, sizeof(float));
	return ret;
}

float float_value(uint32_t index) {
	assert(index < total_representable_values(float_exponent, float_mantissa));
	if (index < representable_values(float_exponent, float_mantissa) - 1) {
		return -float_value_one_side(representable_values(float_exponent, float_mantissa) - index - 1);
	}
	return float_value_one_side(index - representable_values(float_exponent, float_mantissa) + 1);
}

/*Out convert(In position) {
	Out hi = std::numeric_limits<Out>::max();
	Out lo = std::numeric_limits<Out>::lowest();
}*/

int main(int argc, const char *argv[])
{
	static_assert(representation(0, float_exponent, float_mantissa) == 0, "Float 0 does not match");
	static_assert(representation(0, double_exponent, double_mantissa) == 0, "Double 0 does not match");

	assert(float_value(0) == std::numeric_limits<float>::lowest());
	assert(float_value(representable_values(float_exponent, float_mantissa) - 1) == 0);
	assert(float_value(total_representable_values(float_exponent, float_mantissa) - 1) == std::numeric_limits<float>::max());

	// Test
	std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> dist(1, 1000);
	for (uint32_t i = 0 ; i + 1 < total_representable_values(float_exponent, float_mantissa) ; i += dist(rng)) {
		assert(float_value(i) < float_value(i+1));
	}
	return 0;
}
