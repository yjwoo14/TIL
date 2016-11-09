#include <iostream>
#include <unordered_set>
#include <set>
#include <random> // used for making random status

size_t boost_hash_combine(size_t a, size_t b) {
	return a ^ (b + 0x9e3779b9 + (a << 6) + (a >> 2));
}

template <int N>
struct status {
	size_t values[N];

	status(){
		std::fill(values, values + N, 0);
	}

	struct hash {
		size_t operator()(const status<N> & s) const {
			size_t ret = 0;
			for (int i = 0 ; i < N ; ++i) {
				ret ^= s.values[i]; // simple xor hash
				// ret = boost_hash_combine(ret, s.values[i]);
			}
			return ret;
		}
	};

	bool operator==(const status & s) const {
		for (int i = 0 ; i < N ; ++i) 
			if (values[i] != s.values[i]) 
				return false;
		return true;
	}

	bool operator<(const status & s) const {
		if (N == 0) return false;
		for (int i = 0 ; i + 1 < N ; ++i) {
			if (values[i] != s.values[i])
				return values[i] < s.values[i];
		}
		return values[N-1] < s.values[N-1];
	}
};


// set vs. unordered set
//typedef std::unordered_set<status<13>, status<13>::hash> status_set;
typedef std::set<status<13>> status_set;

template <int N>
status<N> make_random_status() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, std::numeric_limits<int>::max());
	status<13> ret;
	for (int i = 0 ; i < N ; ++i) {
		size_t a = (size_t)dis(gen);
		size_t b = (size_t)dis(gen);
		ret.values[i] = a << 32 | b;
	}
	return ret;
}

template <int N>
status<N> make_status(size_t n) {
	size_t i = ((n / 64) % 13);
	size_t j = n % 64;
	size_t k = ((n / 64) / 13);

	status<13> ret;
	ret.values[i] = (1 << j);
	ret.values[0] = k;
	return ret;
}

int main(int argc, const char *argv[])
{
	status_set set;
	int hit_count = 0;
	const int total_trial = 10000000;
	for (int i = 0 ; i < total_trial ; ++i)  {
		status<13> s = make_status<13>(i);
		//status<13> s = make_random_status<13>();
		set.emplace(s);
	}

	for (int i = 0 ; i < total_trial ; ++i) {
		status<13> s = make_status<13>(i);
		//status<13> s = make_random_status<13>();
		if (set.count(s))
			hit_count++;
	}
	
	std::cout << "set size: " << set.size() << " total hit count: " << hit_count << std::endl;
	return 0;
}
