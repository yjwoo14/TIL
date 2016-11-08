#include <iostream>
#include <typeinfo>

template <typename T>
struct has_const_iterator {
	typedef char yes[1];
	typedef char no[2];
	template <typename C>
	static yes& test(typename C::const_iterator *);

	template <typename>
	static no& test(...);

	static const bool value = sizeof(test<T>(nullptr)) == sizeof(yes);
};

template <typename T>
struct has_begin {
	typedef char yes[1];
	typedef char no[2];

	template <typename C>
	static yes& test(typename std::enable_if_t<
						 std::is_same<
							decltype(
								static_cast<typename C::const_iterator (C::*)() const>(&C::begin)),
									typename C::const_iterator(C::*)() const>::value> *);

	template <typename>
	static no& test(...);
	static const bool value = sizeof(test<T>(nullptr)) == sizeof(yes);
};

template <typename T>
struct has_end {
	typedef char yes[1];
	typedef char no[2];

	template <typename C>
	static yes& test(typename std::enable_if_t<
						 std::is_same<
							decltype(
								static_cast<typename C::const_iterator (C::*)() const>(&C::end)),
									typename C::const_iterator(C::*)() const>::value> *);

	template <typename>
	static no& test(...);
	static const bool value = sizeof(test<T>(nullptr)) == sizeof(yes);
};


template<typename T> 
struct is_container 
: std::integral_constant<bool, has_const_iterator<T>::value && has_begin<T>::value && has_end<T>::value> {};

#include <vector>
#include <set>
#include <list>
#include <map>

template <typename T, typename Functor>
void extract(const T & item, Functor functor, std::enable_if_t<!is_container<T>::value> * = 0) {
	functor(item);
}

template <typename T, typename Functor>
void extract(const T & items, Functor functor, std::enable_if_t<is_container<T>::value> * = 0) {
	for (const auto & item : items)
		extract(item, functor);
}

int main(int argc, const char *argv[])
{
	
	std::cout << is_container<std::vector<int>>::value << std::endl; //true
	std::cout << is_container<std::list<int>>::value << std::endl;   //true 
	std::cout << is_container<std::map<int, int>>::value << std::endl;    //true
	std::cout << is_container<std::set<int>>::value << std::endl;    //true
	std::cout << is_container<int>::value << std::endl;              //false

	int sum1, sum2;
	
	sum1 = sum2 = 0;
	{ 
		std::vector<int> v1 = {1,2,3,4,5,6,7,8,9,10};
		std::vector<std::vector<int>> v2 = {{1,2,3,4,5}, {6,7,8,9,10}};
		extract(v1, [&sum1](int x){ sum1 += x; });
		extract(v2, [&sum2](int x){ sum2 += x; });
		std::cout << sum1 << " " << sum2 << std::endl;
	}

	sum1 = sum2 = 0;
	{
		std::set<int> s1 = {1,2,3,4,5,6,7,8,9,10};
		std::set<std::set<int>> s2 = {{1,2,3,4,5},{6,7,8,9,10}};
		extract(s1, [&sum1](int x){ sum1 += x; });
		extract(s2, [&sum2](int x){ sum2 += x; });
	}
	std::cout << sum1 << " " << sum2 << std::endl;

	return 0;
}
