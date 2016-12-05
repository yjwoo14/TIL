#include <iostream>
#include <type_traits>
#include <tuple>
#include <utility>
#include <typeinfo>

// Reference: http://coliru.stacked-crooked.com/view?id=d51ff6c809c9d6fabede11d0fa67a19a-f0d9bbac4ab033ac5f4ce440d21735ee

template <unsigned...>
struct ReverseIntegerSequence {
	using type = ReverseIntegerSequence;
};

template <unsigned I, unsigned... Is>
struct MakeReverseIntegerSequence
    : MakeReverseIntegerSequence<I - 1, I - 1, Is...>::type {};
template <unsigned... Is>
struct MakeReverseIntegerSequence<0, Is...> : ReverseIntegerSequence<Is...> {};

template <unsigned I, typename T>
struct TriviallyCopyableElement {
	static_assert(std::is_trivially_copyable<T>::value, "Required trivially copyable element.");
	T value;
};

template <typename seq, typename... Ts>
struct TriviallyCopyableTupleImpl;

template <unsigned... Is, typename... Ts>
struct TriviallyCopyableTupleImpl<ReverseIntegerSequence<Is...>, Ts...>
    : TriviallyCopyableElement<Is, Ts>... {
	using BaseType = TriviallyCopyableTupleImpl;
	TriviallyCopyableTupleImpl() = default;
	template <typename... Us>
	TriviallyCopyableTupleImpl(Us &&... us)
	    : TriviallyCopyableElement<Is, Ts>{static_cast<Us &&>(us)}... {}
};

template <typename... Ts>
struct TriviallyCopyableTuple
    : TriviallyCopyableTupleImpl<
          typename MakeReverseIntegerSequence<sizeof...(Ts)>::type, Ts...> {
	TriviallyCopyableTuple() = default;

	template <typename... Us>
	TriviallyCopyableTuple(Us &&... us)
	    : TriviallyCopyableTuple::BaseType(static_cast<Us &&>(us)...) {}

	template <unsigned I, typename T>
	static const T &_get(const TriviallyCopyableElement<I, T> &e) {
		return e.value;
	}

	template <unsigned I, typename T>
	static T &_get(TriviallyCopyableElement<I, T> &e) {
		return e.value;
	}
};

template <unsigned I, typename... Ts>
auto get(const TriviallyCopyableTuple<Ts...> &tuple)
    -> const decltype(tuple.template _get<I>(tuple)) {
	return tuple.template _get<I>(tuple);
}

template <unsigned I, typename... Ts>
auto get(TriviallyCopyableTuple<Ts...> &tuple)
    -> decltype(tuple.template _get<I>(tuple)) {
	return tuple.template _get<I>(tuple);
}

template <unsigned I, typename T>
struct TriviallyCopyableTupleElement {
	// TODO: check or make it work with const element....
	typedef typename std::decay<decltype(T::template _get<I>(T()))>::type type;
};

int main(int argc, const char *argv[]) {
	std::cout << std::is_trivially_copyable<const int>::value
	          << std::endl;
	std::cout << std::is_trivially_copyable<std::tuple<int, int>>::value
	          << std::endl;
	std::cout << std::is_trivially_copyable<TriviallyCopyableTuple<int, float, double>>::value
	          << std::endl;

	std::cout << typeid(TriviallyCopyableTupleElement<0, TriviallyCopyableTuple<int, float, double>>::type).name() << std::endl;
	std::cout << typeid(TriviallyCopyableTupleElement<1, TriviallyCopyableTuple<int, float, double>>::type).name() << std::endl;
	std::cout << typeid(TriviallyCopyableTupleElement<2, TriviallyCopyableTuple<int, float, double>>::type).name() << std::endl;
	std::cout << typeid(int).name() << " " << typeid(float).name() << " " << typeid(double).name() << std::endl;
	std::cout << std::boolalpha;
	std::cout << std::is_same<int, int>::value << std::endl;
	std::cout << std::is_same<int, TriviallyCopyableTupleElement<0, TriviallyCopyableTuple<int, float, double>>::type>::value << std::endl;
	std::cout << std::is_same<float, TriviallyCopyableTupleElement<1, TriviallyCopyableTuple<int, float, double>>::type>::value << std::endl;
	std::cout << std::is_same<double, TriviallyCopyableTupleElement<2, TriviallyCopyableTuple<int, float, double>>::type>::value << std::endl;
	TriviallyCopyableTuple<int, float, double> A;
	const TriviallyCopyableTuple<int, float, double> B = {1,2.f,3.};
	get<0>(A) = 1;
	get<1>(A) = 1.1;
	get<2>(A) = 1.2;
	std::cout << get<0>(A) << " " << get<1>(A) << " " << get<2>(A) << std::endl;
	std::cout << get<0>(B) << " " << get<1>(B) << " " << get<2>(B) << std::endl;

	std::tuple<const int, int> X(1,2);
	std::cout << std::is_same<int, std::tuple_element<0, std::tuple<const int, int>>::type>::value << std::endl;
	std::cout << std::is_same<int, std::tuple_element<1, std::tuple<const int, int>>::type>::value << std::endl;

//	TriviallyCopyableTuple<std::tuple<int>> c; // error
	return 0;
}
