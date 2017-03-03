#include <iostream>
#include <random>
#include <cmath>
#include <iomanip>
#include <functional>

/* Variable constants */
constexpr size_t N = 9;
constexpr size_t M = 7;
/* Variable constants done */

const double PI = 3.14159265359;

/* General functions */
typedef std::uniform_real_distribution<double> distribution;
double random_coordinate(double r) {
	static std::random_device rd;
	static std::mt19937 rng{rd()};
	distribution dist(-r, r);
	return dist(rng);
}

double distance(double a, double b) {
	return std::abs(a - b);
}

template <typename Funct>
void repeat(size_t trials, Funct f) {
	while(trials--) f();
}
/* General functions done */

class Experiment {
public:
	Experiment(double r) : r(r) {}
	size_t operator()() {
		double x = random_coordinate(r), y = random_coordinate(r);
		hits += inside_circle(r, x, y) ? 1 : 0;
		return ++trials;
	}

	double estimated_pi() const { return probability() * 4; }
	size_t num_trials() const { return trials; }
private:
	double probability() const { return (double)hits/trials; }
	bool inside_circle(double r, double x, double y) const {
		return std::hypot(x, y) < r;
	}

	double r;
	size_t hits = 0, trials = 0;
};

void table(double MAX_RADIUS, size_t MAX_N_PTS) {
	for (double r = 1 ; r <= MAX_RADIUS ; r *= 10) {
		repeat(MAX_N_PTS, [experiment = Experiment(r), next = 1]() mutable {
			if (experiment() < next) return;
			std::cout << std::setw(14) << distance(experiment.estimated_pi(), PI);
			next *= 10;
		});
		std::cout << std::endl;
	}
}

int main(int argc, const char *argv[])
{
	table(std::pow(10, N), std::pow(10, M));
	return 0;
};
