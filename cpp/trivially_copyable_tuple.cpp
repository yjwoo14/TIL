#include <iostream>
#include <type_traits>
#include <tuple>
#include <utility>

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
	typedef decltype(T::template _get<I>(T())) type;
};

int main(int argc, const char *argv[]) {
	std::cout << std::is_trivially_copyable<std::tuple<int, int>>::value
	          << std::endl;
	std::cout << std::is_trivially_copyable<TriviallyCopyableTuple<int, float, double>>::value
	          << std::endl;
	std::cout << std::is_same<int, TriviallyCopyableTupleElement<1, TriviallyCopyableTuple<int, float, double>>>::value << std::endl;
	std::cout << std::is_same<float, TriviallyCopyableTupleElement<1, TriviallyCopyableTuple<int, float, double>>>::value << std::endl;
	TriviallyCopyableTuple<int, float, double> A;
	get<0>(A) = 1;
	std::cout << get<0>(A) << std::endl;
//	TriviallyCopyableTuple<std::tuple<int>> c; // error
	return 0;
}
