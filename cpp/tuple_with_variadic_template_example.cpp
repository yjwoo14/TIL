#include <iostream>
#include <tuple>
#include <typeinfo>

#include <experimental/tuple>

// some type that has construct() method
template <typename T>
struct Type {
	typedef T type;
	T x;
	Type(T x) : x(x) {}
	T construct() {
		return x;
	}
};

// Problem: Want to have a class such that given an arbitrary number of Type<>
// items, move the return value of items.construct() to the internal turple,
// and do something with each element of the tuple.

////////////////
// Solution 1 //
////////////////

template <typename T>
struct is_tuple : std::integral_constant<bool, false> {};
template <typename... Tp>
struct is_tuple<std::tuple<Tp...>> : std::integral_constant<bool, true> {};

// reference: https://stackoverflow.com/questions/1198260/iterate-over-tuple
template<std::size_t I = 0, typename T, typename Funct>
typename std::enable_if<
    is_tuple<typename std::decay<T>::type>::value &&
	I == std::tuple_size<typename std::decay<T>::type>::value,
    void>::type
for_each(T &&, Funct) // Unused arguments are given no names.
{ }

template<std::size_t I = 0, typename T, typename Funct>
typename std::enable_if<
    is_tuple<typename std::decay<T>::type>::value &&
	I < std::tuple_size<typename std::decay<T>::type>::value,
    void>::type
for_each(T &&t, Funct f)
{
	f(std::get<I>(t));
	for_each<I + 1, T, Funct>(std::forward<T>(t), f);
}

template <std::size_t I = 0, typename T1, typename T2, typename Funct>
typename std::enable_if<
    is_tuple<typename std::decay<T1>::type>::value &&
	is_tuple<typename std::decay<T2>::type>::value &&
	I == std::tuple_size<typename std::decay<T1>::type>::value,
    void>::type
mutual_for_each(T1 &&, T2 &&, Funct) {
	static_assert(std::tuple_size<typename std::decay<T1>::type>::value ==
	                  std::tuple_size<typename std::decay<T2>::type>::value,
	              "Tuple size must be the same");
}

template <std::size_t I = 0, typename T1, typename T2, typename Funct>
typename std::enable_if<
    is_tuple<typename std::decay<T1>::type>::value &&
	is_tuple<typename std::decay<T2>::type>::value &&
	I < std::tuple_size<typename std::decay<T1>::type>::value,
    void>::type
mutual_for_each(T1 &&t1, T2 &&t2, Funct f) {
	static_assert(std::tuple_size<typename std::decay<T1>::type>::value ==
	                  std::tuple_size<typename std::decay<T2>::type>::value,
	              "Tuple size must be the same");
	f(std::get<I>(t1), std::get<I>(t2));
	mutual_for_each<I + 1, T1, T2, Funct>(std::forward<T1>(t1), std::forward<T2>(t2), f);
}

template <typename ...Args>
struct Solution {
	Solution(Args... items)
	    : items1(items.construct()...) {
		for_each(items1, [](auto x) { std::cout << x << std::endl; });
		mutual_for_each(items1, items2, [](auto const &a, auto & b) { b = a; });
	}

	std::tuple<typename Args::type...> const items1;
	std::tuple<typename Args::type...> items2;
	static_assert(is_tuple<typename std::decay<decltype(items1)>::type>::value, "items1 must be tuple type.");
	static_assert(!is_tuple<int>::value, "int must not be tuple type.");
	static_assert(is_tuple<std::decay<std::tuple<int> &>::type>::value, "referenced tuple must be tuple type.");
};

////////////////
// Solution 2 //
////////////////

template <typename ...Args>
struct Simpler {
	Simpler(Args... items)
	    : items1(items.construct()...) {
		auto go = [](auto &&...items) {
			auto print = [](auto item) { std::cout << item << std::endl; return 1;};
			std::initializer_list<int> x{print(items)...};
			reinterpret_cast<void *>(&x); // can be replaced by [[maybe unused]] in c++17
		};
		// this will be std in c++17
		std::experimental::apply(go, items1);
		auto go2 = [](auto &&... items) {
			return std::forward_as_tuple(items...);
		};
		items2 = std::experimental::apply(go2, items1);
	}

	std::tuple<typename Args::type...> const items1;
	std::tuple<typename Args::type...> items2;
};


int main(int argc, const char *argv[])
{
	{
		// without std::apply
		Solution<Type<int>,Type<char>> items(Type<int>(1),Type<char>('a'));
		std::cout << typeid(items.items1).name() << std::endl;
		std::cout << std::get<1>(items.items2) << std::endl;
	}

	{
		// with std::apply
		Simpler<Type<int>,Type<char>> items(Type<int>(1),Type<char>('a'));
		std::cout << typeid(items.items1).name() << std::endl;
		std::cout << std::get<1>(items.items2) << std::endl;
	}
	return 0;
}
