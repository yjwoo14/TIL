#include <iostream>
#include <unordered_set>
#include <set>
#include <random> // used for making random status
#include <algorithm>

struct status {
	mutable size_t whites;
	mutable size_t blacks;
	mutable bool resolved;

	status() : whites(0), blacks(0), resolved(true) {}

	static size_t resolve(size_t x) {
		const size_t mask = 0x1FF;
		std::vector<int> v(0);
		v.reserve(6);
		while (x > 0) {
			v.push_back(x & mask);
			x >>= 9;
		}
		std::sort(v.begin(), v.end());

		size_t ret = 0;
		for (const auto l : v) {
			ret <<= 9;
			ret |= l;
		}
		return ret;
	}

	void resolve() const {
		whites = resolve(whites);
		blacks = resolve(blacks);
		resolved = true;
	}

	void add(int loc, int dep, bool white) { 
		size_t & target = (white)?whites:blacks;
		size_t bits = dep * 9;
		target &= ~((0x1FF) << bits);
		target |= loc << bits;
		resolved = false;
	}

	struct hash {
		size_t operator()(const status & s) const {
			if (!s.resolved) s.resolve();
			return s.whites ^ s.blacks;
		}
	};

	bool operator==(const status & s) const {
		if (!resolved) resolve();
		if (!s.resolved) s.resolve();
		return whites == s.whites && blacks == s.blacks;
	}

	bool operator<(const status & s) const {
		if (!resolved) resolve();
		if (!s.resolved) s.resolve();
		if (whites != s.whites) return whites < s.whites;
		return blacks < s.blacks;
	}
};


// set vs. unordered set
//typedef std::unordered_set<status, status::hash> status_set;
typedef std::set<status> status_set;

status make_random_status() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 391);
	status ret;

	for (int i = 0 ; i < 6 ; ++i) {
		int a = dis(gen);
		int b = dis(gen);
		if (b % 2) continue;
		ret.add(a, i, true);
	}
	for (int i = 0 ; i < 6 ; ++i) {
		int a = dis(gen);
		int b = dis(gen);
		if (b % 2) continue;
		ret.add(a, i, false);
	}
	return ret;
}

status make_status(size_t n) {
	status ret;
	size_t tn = n;
	for (int i = 0 ; i < 6 ; ++i) {
		if (n <= 0) break;
		ret.add(n % 391, i, true);
		n /= 391;
	}

	n = ~tn;
	for (int i = 0 ; i < 6 ; ++i) {
		if (n <= 0) break;
		ret.add(n % 391, i, false);
	}
	return ret;
}

int main(int argc, const char *argv[])
{
	status_set set;
	int hit_count = 0;
	const int total_trial = 10000000;
	for (int i = 0 ; i < total_trial ; ++i)  {
		status s = make_status(i);
		//status s = make_random_status();
		set.emplace(s);
	}

	for (int i = 0 ; i < total_trial ; ++i) {
		status s = make_status(i);
		//status s = make_random_status();
		if (set.count(s))
			hit_count++;
	}
	
	std::cout << "set size: " << set.size() << " total hit count: " << hit_count << std::endl;
	return 0;
}
