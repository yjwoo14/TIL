#include <iostream>
#include <type_traits>
#include <tuple>
#include <utility>
#include <typeinfo>
#include <cassert>
#include <cstring>

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
	TriviallyCopyableTupleImpl(TriviallyCopyableTupleImpl &) = default;
	TriviallyCopyableTupleImpl(TriviallyCopyableTupleImpl &&) = default;
	TriviallyCopyableTupleImpl(const TriviallyCopyableTupleImpl &) = default;
	template <typename... Us>
	TriviallyCopyableTupleImpl(Us &&... us)
	    : TriviallyCopyableElement<Is, Ts>{static_cast<Us &&>(us)}... {}
	TriviallyCopyableTupleImpl & operator=(const TriviallyCopyableTupleImpl &) = default;
	TriviallyCopyableTupleImpl & operator=(TriviallyCopyableTupleImpl &&) = default;

};

template <typename... Ts>
struct TriviallyCopyableTuple;

template <unsigned I, typename T>
struct TriviallyCopyableTupleElement;

template <unsigned I, typename Head, typename ... Tail>
struct TriviallyCopyableTupleElement<I, TriviallyCopyableTuple<Head, Tail...>>
	: TriviallyCopyableTupleElement<I-1, TriviallyCopyableTuple<Tail...>> {};

template <typename Head, typename ... Tail>
struct TriviallyCopyableTupleElement<0, TriviallyCopyableTuple<Head, Tail...>> {
	typedef Head type;
};

template <typename... Ts>
struct TriviallyCopyableTuple
    : TriviallyCopyableTupleImpl<
          typename MakeReverseIntegerSequence<sizeof...(Ts)>::type, Ts...> {
	typedef typename MakeReverseIntegerSequence<sizeof...(Ts)>::type Ris;

	TriviallyCopyableTuple() = default;
	TriviallyCopyableTuple(TriviallyCopyableTuple &) = default;
	TriviallyCopyableTuple(const TriviallyCopyableTuple &) = default;
	TriviallyCopyableTuple(TriviallyCopyableTuple &&) = default;

	template <typename... Us>
	TriviallyCopyableTuple(Us &&... us)
	    : TriviallyCopyableTuple::BaseType(static_cast<Us &&>(us)...) {}
	TriviallyCopyableTuple &operator=(const TriviallyCopyableTuple &) = default;
	TriviallyCopyableTuple &operator=(TriviallyCopyableTuple &&) = default;

	template <typename... Us>
	TriviallyCopyableTuple &operator=(const TriviallyCopyableTuple<Us...> &o) {
		static_assert(!std::is_same<TriviallyCopyableTuple<Ts...>, TriviallyCopyableTuple<Us...>>::value, "tuple type must be different");
		static_assert(sizeof...(Ts) == sizeof...(Us), "tuple object can be assigned if they have equal sizes.");
		copyAssignmentOperatorImpl(o, Ris{});
		return *this;
	}

	template <typename... Us, unsigned ...Is>
	void copyAssignmentOperatorImpl(const TriviallyCopyableTuple<Us...> &o,
	                                ReverseIntegerSequence<Is...>) {
		std::initializer_list<int>{static_cast<int>(this->template _get<Is>(*this) = o.template _get<Is>(o), 0)...};
	}

	template <unsigned I, typename T>
	static const T & _get(const TriviallyCopyableElement<I, T> &e) {
		return e.value;
	}

	template <unsigned I, typename T>
	static T &_get(TriviallyCopyableElement<I, T> &e) {
		return e.value;
	}

	static bool allTrue(bool head) {
		return head;
	}

	template <typename ...X>
	static bool allTrue(bool head, X... tail) {
		if (!head) return false;
		return allTrue(tail...);
	}

	template <typename T>
	struct LessCompare {
		const T &l, &r;
		std::less<T> comp;
		LessCompare(const T &l, const T &r): l(l), r(r) {}
	};

	template <typename T>
	struct GreaterCompare {
		const T &l, &r;
		std::greater<T> comp;
		GreaterCompare(const T &l, const T &r): l(l), r(r) {}
	};

	template <typename T>
	static bool compare(const T & head) {
		return head.comp(head.l, head.r);
	}

	template <typename T1, typename ...T2>
	static bool compare(const T1 & head, const T2 &... tail) {
		if (head.comp(head.l, head.r)) return true;
		if (!head.comp(head.r, head.l)) return compare(tail...);
		return false;
	}

	template <typename... Us, unsigned... Is>
	bool lessImpl(const TriviallyCopyableTuple<Us...> &r,
	              ReverseIntegerSequence<Is...>) const {
		return compare(LessCompare<typename TriviallyCopyableTupleElement<
		                   Is, TriviallyCopyableTuple>::type>(
		    (*this).template _get<Is>(*this), r.template _get<Is>(r))...);
	}

	template <typename... Us, unsigned... Is>
	bool greaterImpl(const TriviallyCopyableTuple<Us...> &r,
	                 ReverseIntegerSequence<Is...>) const {
		return compare(GreaterCompare<typename TriviallyCopyableTupleElement<
		                   Is, TriviallyCopyableTuple>::type>(
		    (*this).template _get<Is>(*this), r.template _get<Is>(r))...);
	}

	template <typename... Us, unsigned... Is>
	bool equalImpl(const TriviallyCopyableTuple<Us...> &r,
	               ReverseIntegerSequence<Is...>) const {
		return allTrue(std::equal_to<typename TriviallyCopyableTupleElement<
		                   Is, TriviallyCopyableTuple>::type>()(
		    (*this).template _get<Is>(*this), r.template _get<Is>(r))...);
	}

	template <typename... Us>
	bool operator<(const TriviallyCopyableTuple<Us...> &o) const {
		static_assert(sizeof...(Ts) == sizeof...(Us), "tuple objects can only be compared if they have equal sizes.");
		return lessImpl(o, Ris{});
	}

	template <typename... Us>
	bool operator==(const TriviallyCopyableTuple<Us...> &o) const {
		static_assert(sizeof...(Ts) == sizeof...(Us), "tuple objects can only be compared if they have equal sizes.");
		return equalImpl(o, Ris{});
	}

	template <typename... Us>
	bool operator!=(const TriviallyCopyableTuple<Us...> &o) const {
		static_assert(sizeof...(Ts) == sizeof...(Us), "tuple objects can only be compared if they have equal sizes.");
		return !equalImpl(o, Ris{});
	}

	template <typename... Us>
	bool operator>(const TriviallyCopyableTuple<Us...> &o) const {
		static_assert(sizeof...(Ts) == sizeof...(Us), "tuple objects can only be compared if they have equal sizes.");
		return greaterImpl(o, Ris{});
	}

};

template <unsigned I, typename... Ts>
auto get(const TriviallyCopyableTuple<Ts...> &tuple)
    -> decltype(tuple.template _get<I>(tuple)) {
	return tuple.template _get<I>(tuple);
}

template <unsigned I, typename... Ts>
auto get(TriviallyCopyableTuple<Ts...> &tuple)
    -> decltype(tuple.template _get<I>(tuple)) {
	return tuple.template _get<I>(tuple);
}

int main(int argc, const char *argv[]) {
	static_assert(std::is_trivially_copyable<const int>::value, "Const int is trivially copyable");
	static_assert(std::is_trivially_copyable<std::tuple<int, int>>::value == false, "tuple is not trivially copyable");
	static_assert(std::is_trivially_copyable<TriviallyCopyableTuple<int, float, double>>::value, "new tuple is trivially copyable");

	static_assert(std::is_same<int, std::tuple_element<0, std::tuple<int, float, double>>::type>::value, "type mismatched 0");
	static_assert(std::is_same<float, std::tuple_element<1, std::tuple<int, float, double>>::type>::value, "type mismatched 1");
	static_assert(std::is_same<double, std::tuple_element<2, std::tuple<int, float, double>>::type>::value, "type mismatched 2");
	static_assert(std::is_same<const int, std::tuple_element<0, std::tuple<const int, float, double>>::type>::value, "type mismatched 0");
	static_assert(std::is_same<const float, std::tuple_element<1, std::tuple<int, const float, double>>::type>::value, "type mismatched 1");
	static_assert(std::is_same<const double, std::tuple_element<2, std::tuple<int, float, const double>>::type>::value, "type mismatched 2");

	static_assert(std::is_same<int, TriviallyCopyableTupleElement<0, TriviallyCopyableTuple<int, float, double>>::type>::value, "type mismatched 0");
	static_assert(std::is_same<float, TriviallyCopyableTupleElement<1, TriviallyCopyableTuple<int, float, double>>::type>::value, "type mismatched 1");
	static_assert(std::is_same<double, TriviallyCopyableTupleElement<2, TriviallyCopyableTuple<int, float, double>>::type>::value, "type mismatched 2");
	static_assert(std::is_same<const int, TriviallyCopyableTupleElement<0, TriviallyCopyableTuple<const int, float, double>>::type>::value, "type mismatched 0");
	static_assert(std::is_same<const float, TriviallyCopyableTupleElement<1, TriviallyCopyableTuple<int, const float, double>>::type>::value, "type mismatched 1");
	static_assert(std::is_same<const double, TriviallyCopyableTupleElement<2, TriviallyCopyableTuple<int, float, const double>>::type>::value, "type mismatched 2");

	std::cout << std::boolalpha;
	{
		auto funct = [](){return TriviallyCopyableTuple<int, float, double>();};
		TriviallyCopyableTuple<int, float, double> A;
		const TriviallyCopyableTuple<int, float, double> B = {1,2.f,3.};
		get<0>(A) = 1;
		get<1>(A) = 1.1;
		get<2>(A) = 1.2;
		assert(get<0>(A) == (int)1);
		assert(get<1>(A) == (float)1.1);
		assert(get<2>(A) == (double)1.2);

		auto C = A;
		auto D = TriviallyCopyableTuple<int, float, double>{};
		D = B;
		assert(get<0>(D) == get<0>(B));
		assert(get<1>(D) == get<1>(B));
		assert(get<2>(D) == get<2>(B));

		D = std::move(A);

		assert(get<0>(D) == (int)1);
		assert(get<1>(D) == (float)1.1);
		assert(get<2>(D) == (double)1.2);

		std::memcpy(&D, &B, sizeof(B));
		assert(get<0>(D) == get<0>(B));
		assert(get<1>(D) == get<1>(B));
		assert(get<2>(D) == get<2>(B));

		TriviallyCopyableTuple<int, float, double> E(B);
		assert(E == B);
		const TriviallyCopyableTuple<int, float, double> F(A);
		assert(F == A);
	}

	{
		typedef std::tuple<const int, int> Type;
		Type X(3,1), Y(4,2), Z(3,1), Z1(3,2), Z2(2,-1);
		assert(X < Y);
		assert(!(Y < X));
		assert(!(X > Y));
		assert(Y > X);
		assert(!(X == Y));
		assert(Z == X);
		assert(Z != Y);
		assert(X < Z1);
		assert(X > Z2);
	}

	{

		typedef TriviallyCopyableTuple<const int, int> Type;
		Type X(3,1), Y(4,2), Z(3,1), Z1(3,2), Z2(2,-1);
		assert(X < Y);
		assert(!(Y < X));
		assert(!(X > Y));
		assert(Y > X);
		assert(!(X == Y));
		assert(Z == X);
		assert(Z != Y);
		assert(X < Z1);
		assert(X > Z2);
	}

	{
		typedef std::tuple<int, std::tuple<int, int>> Type1;
		Type1 a(1, std::tuple<int,int>(2,3));
	}

	{
		typedef TriviallyCopyableTuple<int, TriviallyCopyableTuple<int, int>> Type1;
		Type1 a(1, TriviallyCopyableTuple<int,int>(2,3));
	}

	{
		typedef std::tuple<int, int> Type1;
		typedef std::tuple<uint32_t, uint32_t> Type2;
		Type1 a(1,2);
		Type2 b(1u,2u);
		b = a;
	}

	{
		typedef TriviallyCopyableTuple<int, int> Type1;
		typedef TriviallyCopyableTuple<uint32_t, uint32_t> Type2;
		Type1 a(1,2);
		Type2 b(1u,2u);
		Type1 c(a);
		b = a;
	}

	{
		enum E {
			A = 0,
			B = 1,
		};
		typedef TriviallyCopyableTuple<int, E> Type1;
		Type1 a(1,A), b;
		std::memcpy(&b, &a, sizeof(Type1));
		std::cout << get<0>(b) << ' ' << get<1>(b) << std::endl;
	}

//	TriviallyCopyableTuple<std::tuple<int>> c; // error
	return 0;
}
