#include <iostream>
#include <type_traits>
#include <vector>

struct A {
	void push(int) {}
	int pull() {}
};

struct B {
	void push(int) {}
	void push(float) {}
	int pull() {}
	int pull() const {}
};

struct C {
	template <typename T>
	void push(const T&) {}
	template <typename T>
	T pull() {}
};

struct D {
	typedef int item_type;
	void push(int) {}
	void push(float) {}
	template <typename T>
	int pull() {}
};

struct E {
	void push(const std::vector<int> &) {
	}
};

struct F {
	char push(const int A, char B) {
	}
};

template <typename T>
struct push_traits {};

template <typename ClassType, typename ArgType>
struct push_traits< void(ClassType::*)(ArgType) > {
	typedef ArgType type;
};

template <typename T>
struct pull_traits {};

template <typename ClassType, typename RetType>
struct pull_traits< RetType(ClassType::*)() > {
	typedef RetType type;
};

template <typename T, typename default_type, bool has_push_method, bool has_item_type>
struct push_type_help {
	typedef default_type type;
};

template <typename T, typename default_type, bool has_push_method>
struct push_type_help<T, default_type, has_push_method, true> {
	typedef typename T::item_type type;
};

template <typename T, typename default_type>
struct push_type_help<T, default_type, true, false> {
	typedef typename push_traits<decltype(&T::push)>::type type_;
	typedef typename std::decay<type_>::type type;
};

template <typename T, typename default_type, bool has_push_method, bool has_item_type>
struct pull_type_help {
	typedef default_type type;
};

template <typename T, typename default_type, bool has_pull_method>
struct pull_type_help<T, default_type, has_pull_method, true> {
	typedef typename T::item_type type;
};

template <typename T, typename default_type>
struct pull_type_help<T, default_type, true, false> {
	typedef typename pull_traits<decltype(&T::pull)>::type type_;
	typedef typename std::decay<type_>::type type;
};

template <typename T>
struct has_itemtype {
	typedef char yes[1];
	typedef char no[2];

	template <typename C>
	static yes& test(typename C::item_type*);

	template <typename>
	static no& test(...);
	//static_assert(sizeof(test<T>(nullptr)) == sizeof(yes), "WTF");
	static const bool value = sizeof(test<T>(nullptr)) == sizeof(yes);
};

template <typename T>
using void_if_valid = void;

template <typename T>
struct has_push_method {
	typedef char yes[1];
	typedef char no[2];

	template <typename C>
	static yes& test(void_if_valid<typename push_traits<decltype(&C::push)>::type> *);
	// Couldn't use the following definition in case where push_traits::type is reference type...
	// what the hell is &* type!!!!
	// static yes& test(typename push_traits<decltype(&C::push)>::type *);

	template <typename>
	static no& test(...);
	//static_assert(sizeof(test<T>(nullptr)) == sizeof(yes), "WTF");
	static const bool value = sizeof(test<T>(nullptr)) == sizeof(yes);
};

template <typename T>
struct has_pull_method {
	typedef char yes[1];
	typedef char no[2];

	template <typename C>
	static yes& test(void_if_valid<typename pull_traits<decltype(&C::pull)>::type> *);

	template <typename>
	static no& test(...);
	//static_assert(sizeof(test<T>(nullptr)) == sizeof(yes), "WTF");
	static const bool value = sizeof(test<T>(nullptr)) == sizeof(yes);
};

template <typename T, typename default_type = void>
struct push_type {
	typedef typename std::decay<T>::type node_type;
	typedef typename push_type_help<node_type, default_type, has_push_method<node_type>::value, has_itemtype<node_type>::value>::type type;
	static_assert(!std::is_void<type>::value, "Could not deduce push type.");
};

template <typename T, typename default_type = void>
struct pull_type {
	typedef typename std::decay<T>::type node_type;
	typedef typename pull_type_help<node_type, default_type, has_pull_method<node_type>::value, has_itemtype<node_type>::value>::type type;
	static_assert(!std::is_void<type>::value, "Could not deduce pull type.");
};

// pull

int main(int argc, const char *argv[])
{
	push_type<A>::type a;
//	push_type<B>::type b;
//	push_type<C>::type c;
	push_type<D>::type d;
//	std::cout << sizeof(push_type<A, char>::type) << std::endl;
//	std::cout << sizeof(push_type<B, char>::type) << std::endl;
//	std::cout << sizeof(push_type<C, char>::type) << std::endl;
//	std::cout << sizeof(push_type<D, char>::type) << std::endl;
//	std::cout << sizeof(push_type<A>::type) << std::endl;
//	std::cout << sizeof(push_type<B>::type) << std::endl;
//	std::cout << sizeof(push_type<C>::type) << std::endl;
//	std::cout << sizeof(push_type<D>::type) << std::endl;
	std::cout << sizeof(push_type<E>::type) << std::endl;
	std::cout << sizeof(push_type<F>::type) << std::endl;

	std::cout << sizeof(pull_type<A, char>::type) << std::endl;
	std::cout << sizeof(pull_type<B, char>::type) << std::endl;
	std::cout << sizeof(pull_type<C, char>::type) << std::endl;
	std::cout << sizeof(pull_type<D, char>::type) << std::endl;
	std::cout << sizeof(pull_type<A>::type) << std::endl;
	std::cout << sizeof(pull_type<B>::type) << std::endl;
	std::cout << sizeof(pull_type<C>::type) << std::endl;
	std::cout << sizeof(pull_type<D>::type) << std::endl;

	return 0;
}
