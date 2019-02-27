#include <tuple>
#include <iostream>
#include <vector>
#include <optional>

template <typename... IT>
class zip_iterator {
public:
	zip_iterator(std::tuple<IT...> curIts)
		: curIts(std::move(curIts)) {}

	zip_iterator & operator++() {
		auto increment = [](auto &&... its) { std::initializer_list{(++its, 0)...}; };
		std::apply(increment, curIts);
		return *this;
	}

	bool operator != (const zip_iterator & other) const noexcept {
		return std::get<0>(curIts) != std::get<0>(other.curIts);
	}

	bool operator == (const zip_iterator & other) const noexcept {
		return std::get<0>(curIts) == std::get<0>(other.curIts);
	}

	auto operator*() const noexcept {
		auto dereference = [](auto &&... its) { return std::forward_as_tuple(*its...); };
		return std::apply(dereference, curIts);
	}

private:
	std::tuple<IT...> curIts;
};

template <typename... IT>
class zip_range {
public:
	using TupleType = typename std::tuple<std::decay_t<IT>...>;
	using FirstType = typename std::tuple_element<0, TupleType>::type;

	zip_range(FirstType end, IT... begins)
		: begins(begins...)
		, ends(this->begins) {
		std::get<0>(ends) = end;
	}

	zip_iterator<IT...> begin() const { return {begins}; }
	zip_iterator<IT...> end() const { return {ends}; }

private:
	TupleType begins, ends;
};

template <typename... T>
auto zip(T &&... containers) {
	auto first_end = std::get<0>(std::tuple(containers.end()...));
	return zip_range(first_end, containers.begin()...);
}

int main() {
	std::vector<int> a = {1,2,3,4,5};
	std::array<int, 5> b = {3,4,5,6,7};
	std::array<float, 6> c = {1.1,2.2,3.3,4.4,5.5};

	for (const auto & [aa, bb, cc] : zip(a, b, c)) { std::cout << aa << " " << bb << " " << cc << std::endl; }
	std::cout << std::endl;

	for (auto [aa, bb] : zip(a,b)) {
		aa += bb;
	}

	for (const auto & [aa, bb] : zip(a, b)) { std::cout << aa << " " << bb << std::endl; }
}
