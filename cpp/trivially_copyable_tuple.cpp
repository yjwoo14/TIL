#include <iostream>
#include <type_traits>
#include <tuple>
#include <utility>
#include <typeinfo>
#include <cassert>
#include <cstring>
#include <numeric>

// Reference: http://coliru.stacked-crooked.com/view?id=d51ff6c809c9d6fabede11d0fa67a19a-f0d9bbac4ab033ac5f4ce440d21735ee

namespace TriviallyCopyable {
namespace {

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
struct Element {
	static_assert(std::is_trivially_copyable<T>::value, "Required trivially copyable element.");
	T value;
};

template <typename seq, typename... Ts>
struct TupleImpl;

template <unsigned... Is, typename... Ts>
struct TupleImpl<ReverseIntegerSequence<Is...>, Ts...>
    : Element<Is, Ts>... {
	using BaseType = TupleImpl;
	TupleImpl() = default;
	TupleImpl(TupleImpl &&) = default;
	TupleImpl(const TupleImpl &) = default;
	template <typename... Us,
			  typename = std::enable_if_t<(sizeof...(Us) == sizeof...(Ts))>>
	TupleImpl(Us &&... us)
	    : Element<Is, Ts>{static_cast<Us &&>(us)}... {}
	TupleImpl & operator=(const TupleImpl &) = default;
	TupleImpl & operator=(TupleImpl &&) = default;

};

template <typename T, typename BinaryOperation, typename ...Ts>
T accumulate(T init, BinaryOperation op, Ts... item) {
	typedef std::common_type_t<Ts...> CommonType;
	std::initializer_list<CommonType> list {item...};
	return std::accumulate(std::begin(list), std::end(list), init, op);
}

template <typename T>
bool compare(const T & head) {
	return head(head.l, head.r);
}

template <typename T1, typename ...T2>
bool compare(const T1 & head, const T2 &... tail) {
	if (head(head.l, head.r)) return true;
	if (!head(head.r, head.l)) return compare(tail...);
	return false;
}

} // namespace

template <typename... Ts>
struct Tuple;

template <unsigned I, typename T>
struct TupleElement;

template <unsigned I, typename Head, typename ... Tail>
struct TupleElement<I, Tuple<Head, Tail...>>
	: TupleElement<I-1, Tuple<Tail...>> {};

template <typename Head, typename ... Tail>
struct TupleElement<0, Tuple<Head, Tail...>> {
	typedef Head type;
};

template <typename... Ts>
struct Tuple
    : TupleImpl<
          typename MakeReverseIntegerSequence<sizeof...(Ts)>::type, Ts...> {
	typedef typename MakeReverseIntegerSequence<sizeof...(Ts)>::type Ris;

	Tuple() = default;
	Tuple(const Tuple &) = default;
	Tuple(Tuple &&) = default;

	template <typename... Us, 
			  typename = std::enable_if_t<(sizeof...(Us) == sizeof...(Ts))>>
	Tuple(Us &&... us)
	    : Tuple::BaseType(static_cast<Us &&>(us)...) {}
	Tuple &operator=(const Tuple &) = default;
	Tuple &operator=(Tuple &&) = default;

	template <typename... Us>
	Tuple &operator=(const Tuple<Us...> &o) {
		static_assert(!std::is_same<Tuple<Ts...>, Tuple<Us...>>::value, "tuple type must be different");
		static_assert(sizeof...(Ts) == sizeof...(Us), "tuple object can be assigned if they have equal sizes.");
		copyAssignmentOperatorImpl(o, Ris{});
		return *this;
	}

	template <typename... Us, unsigned ...Is>
	void copyAssignmentOperatorImpl(const Tuple<Us...> &o,
	                                ReverseIntegerSequence<Is...>) {
		std::initializer_list<int>{static_cast<int>(this->template _get<Is>(*this) = o.template _get<Is>(o), 0)...};
	}

	template <unsigned I, typename T>
	static const T & _get(const Element<I, T> &e) {
		return e.value;
	}

	template <unsigned I, typename T>
	static T &_get(Element<I, T> &e) {
		return e.value;
	}

	template <typename T>
	struct LessCompare {
		const T &l, &r;
		LessCompare(const T &l, const T &r): l(l), r(r) {}
		bool operator()(const T &l, const T &r) const {
			return std::less<T>()(l, r);
		}
	};

	template <typename T>
	struct GreaterCompare {
		const T &l, &r;
		GreaterCompare(const T &l, const T &r): l(l), r(r) {}
		bool operator()(const T &l, const T &r) const {
			return std::greater<T>()(l, r);
		}
	};

	template <typename T>
	struct LessEqualCompare {
		const T &l, &r;
		LessEqualCompare(const T &l, const T &r): l(l), r(r) {}
		bool operator()(const T &l, const T &r) const {
			if (std::equal_to<T>()(l, r)) return true;
			return std::less<T>()(l, r);
		}
	};

	template <typename T>
	struct GreaterEqualCompare {
		const T &l, &r;
		GreaterEqualCompare(const T &l, const T &r): l(l), r(r) {}
		bool operator()(const T &l, const T &r) const {
			if (std::equal_to<T>()(l, r)) return true;
			return std::greater<T>()(l, r);
		}
	};


	template <typename... Us, unsigned... Is>
	bool lessImpl(const Tuple<Us...> &r,
	              ReverseIntegerSequence<Is...>) const {
		return compare(LessCompare<typename TupleElement<
		                   Is, Tuple>::type>(
		    (*this).template _get<Is>(*this), r.template _get<Is>(r))...);
	}

	template <typename... Us, unsigned... Is>
	bool greaterImpl(const Tuple<Us...> &r,
	                 ReverseIntegerSequence<Is...>) const {
		return compare(GreaterCompare<typename TupleElement<
		                   Is, Tuple>::type>(
		    (*this).template _get<Is>(*this), r.template _get<Is>(r))...);
	}

	template <typename... Us, unsigned... Is>
	bool lessEqualImpl(const Tuple<Us...> &r,
	                   ReverseIntegerSequence<Is...>) const {
		return compare(LessEqualCompare<typename TupleElement<
		                   Is, Tuple>::type>(
		    (*this).template _get<Is>(*this), r.template _get<Is>(r))...);
	}

	template <typename... Us, unsigned... Is>
	bool greaterEqualImpl(const Tuple<Us...> &r,
	                      ReverseIntegerSequence<Is...>) const {
		return compare(
		    GreaterEqualCompare<typename TupleElement<
		        Is, Tuple>::type>(
		        (*this).template _get<Is>(*this), r.template _get<Is>(r))...);
	}


	template <typename... Us, unsigned... Is>
	bool equalImpl(const Tuple<Us...> &r,
	               ReverseIntegerSequence<Is...>) const {
		return accumulate(
				true, [](bool a, bool b) { return a && b; },
				std::equal_to<typename TupleElement<Is, Tuple>::type>()(
					(*this).template _get<Is>(*this),
					r.template _get<Is>(r))...);
	}

	template <typename... Us>
	bool operator<(const Tuple<Us...> &o) const {
		static_assert(sizeof...(Ts) == sizeof...(Us), "tuple objects can only be compared if they have equal sizes.");
		return lessImpl(o, Ris{});
	}

	template <typename... Us>
	bool operator<=(const Tuple<Us...> &o) const {
		static_assert(sizeof...(Ts) == sizeof...(Us), "tuple objects can only be compared if they have equal sizes.");
		return lessEqualImpl(o, Ris{});
	}

	template <typename... Us>
	bool operator==(const Tuple<Us...> &o) const {
		static_assert(sizeof...(Ts) == sizeof...(Us), "tuple objects can only be compared if they have equal sizes.");
		return equalImpl(o, Ris{});
	}

	template <typename... Us>
	bool operator!=(const Tuple<Us...> &o) const {
		static_assert(sizeof...(Ts) == sizeof...(Us), "tuple objects can only be compared if they have equal sizes.");
		return !equalImpl(o, Ris{});
	}

	template <typename... Us>
	bool operator>(const Tuple<Us...> &o) const {
		static_assert(sizeof...(Ts) == sizeof...(Us), "tuple objects can only be compared if they have equal sizes.");
		return greaterImpl(o, Ris{});
	}

	template <typename... Us>
	bool operator>=(const Tuple<Us...> &o) const {
		static_assert(sizeof...(Ts) == sizeof...(Us), "tuple objects can only be compared if they have equal sizes.");
		return greaterEqualImpl(o, Ris{});
	}

	template <unsigned... Is>
	std::tuple<Ts...> makeStdTupleImpl(ReverseIntegerSequence<Is...>) const {
		return std::tuple<Ts...>(_get<Is>(*this)...);
	}

	template <typename... Us, unsigned... Is>
	Tuple<Us...> makeTupleImpl(ReverseIntegerSequence<Is...>) const {
		return Tuple<Us...>(_get<Is>(*this)...);
	}

	operator std::tuple<Ts...>() const {
		return makeStdTupleImpl(Ris{});
	}

	template <typename... Us>
	operator Tuple<Us...>() const {
		return makeTupleImpl<Us...>(Ris{});
	}
};

template <unsigned I, typename... Ts>
auto get(const Tuple<Ts...> &tuple)
    -> decltype(tuple.template _get<I>(tuple)) {
	return tuple.template _get<I>(tuple);
}

template <unsigned I, typename... Ts>
auto get(Tuple<Ts...> &tuple)
    -> decltype(tuple.template _get<I>(tuple)) {
	return tuple.template _get<I>(tuple);
}

} // namespace TriviallyCopyable

int main(int argc, const char *argv[]) {
	static_assert(std::is_trivially_copyable<const int>::value, "Const int is trivially copyable");
	static_assert(std::is_trivially_copyable<std::tuple<int, int>>::value == false, "tuple is not trivially copyable");
	static_assert(std::is_trivially_copyable<TriviallyCopyable::Tuple<int, float, double>>::value, "new tuple is trivially copyable");

	static_assert(std::is_same<int, std::tuple_element<0, std::tuple<int, float, double>>::type>::value, "type mismatched 0");
	static_assert(std::is_same<float, std::tuple_element<1, std::tuple<int, float, double>>::type>::value, "type mismatched 1");
	static_assert(std::is_same<double, std::tuple_element<2, std::tuple<int, float, double>>::type>::value, "type mismatched 2");
	static_assert(std::is_same<const int, std::tuple_element<0, std::tuple<const int, float, double>>::type>::value, "type mismatched 0");
	static_assert(std::is_same<const float, std::tuple_element<1, std::tuple<int, const float, double>>::type>::value, "type mismatched 1");
	static_assert(std::is_same<const double, std::tuple_element<2, std::tuple<int, float, const double>>::type>::value, "type mismatched 2");

	static_assert(std::is_same<int, TriviallyCopyable::TupleElement<0, TriviallyCopyable::Tuple<int, float, double>>::type>::value, "type mismatched 0");
	static_assert(std::is_same<float, TriviallyCopyable::TupleElement<1, TriviallyCopyable::Tuple<int, float, double>>::type>::value, "type mismatched 1");
	static_assert(std::is_same<double, TriviallyCopyable::TupleElement<2, TriviallyCopyable::Tuple<int, float, double>>::type>::value, "type mismatched 2");
	static_assert(std::is_same<const int, TriviallyCopyable::TupleElement<0, TriviallyCopyable::Tuple<const int, float, double>>::type>::value, "type mismatched 0");
	static_assert(std::is_same<const float, TriviallyCopyable::TupleElement<1, TriviallyCopyable::Tuple<int, const float, double>>::type>::value, "type mismatched 1");
	static_assert(std::is_same<const double, TriviallyCopyable::TupleElement<2, TriviallyCopyable::Tuple<int, float, const double>>::type>::value, "type mismatched 2");

	std::cout << std::boolalpha;
	{
		auto funct = [](){return TriviallyCopyable::Tuple<int, float, double>();};
		TriviallyCopyable::Tuple<int, float, double> A;
		const TriviallyCopyable::Tuple<int, float, double> B = {1,2.f,3.};
		TriviallyCopyable::get<0>(A) = 1;
		TriviallyCopyable::get<1>(A) = 1.1;
		TriviallyCopyable::get<2>(A) = 1.2;
		assert(TriviallyCopyable::get<0>(A) == (int)1);
		assert(TriviallyCopyable::get<1>(A) == (float)1.1);
		assert(TriviallyCopyable::get<2>(A) == (double)1.2);

		auto C = A;
		auto D = TriviallyCopyable::Tuple<int, float, double>{};
		D = B;
		assert(TriviallyCopyable::get<0>(D) == TriviallyCopyable::get<0>(B));
		assert(TriviallyCopyable::get<1>(D) == TriviallyCopyable::get<1>(B));
		assert(TriviallyCopyable::get<2>(D) == TriviallyCopyable::get<2>(B));

		D = std::move(A);

		assert(TriviallyCopyable::get<0>(D) == (int)1);
		assert(TriviallyCopyable::get<1>(D) == (float)1.1);
		assert(TriviallyCopyable::get<2>(D) == (double)1.2);

		std::memcpy(&D, &B, sizeof(B));
		assert(TriviallyCopyable::get<0>(D) == TriviallyCopyable::get<0>(B));
		assert(TriviallyCopyable::get<1>(D) == TriviallyCopyable::get<1>(B));
		assert(TriviallyCopyable::get<2>(D) == TriviallyCopyable::get<2>(B));

		TriviallyCopyable::Tuple<int, float, double> E(B);
		assert(E == B);
		const TriviallyCopyable::Tuple<int, float, double> F(A);
		assert(F == A);
	}

	{
		TriviallyCopyable::Tuple<int> A(1);
		auto C = A;
		assert(TriviallyCopyable::get<0>(A) == TriviallyCopyable::get<0>(C));
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

		typedef TriviallyCopyable::Tuple<const int, int> Type;
		Type X(3,1), Y(4,2), Z(3,1), Z1(3,2), Z2(2,-1);
		assert(X < Y);
		assert(!(Y < X));
		assert(!(X > Y));
		assert(Y > X);
		assert(!(X == Y));
		assert(Z == X);
		assert(Z <= X);
		assert(Z >= X);
		assert(Z != Y);
		assert(X < Z1);
		assert(X <= Z1);
		assert(X > Z2);
		assert(X >= Z2);
	}

	{
		typedef std::tuple<int, std::tuple<int, int>> Type1;
		Type1 a(1, std::tuple<int,int>(2,3));
	}

	{
		typedef TriviallyCopyable::Tuple<int, TriviallyCopyable::Tuple<int, int>> Type1;
		Type1 a(1, TriviallyCopyable::Tuple<int,int>(2,3));
	}

	{
		typedef std::tuple<int, int> Type1;
		typedef std::tuple<uint32_t, uint32_t> Type2;
		Type1 a(1,2);
		Type2 b(1u,2u);
		Type1 c(a);
		Type2 d(a);
		b = a;
	}

	{
		typedef TriviallyCopyable::Tuple<int, int> Type1;
		typedef TriviallyCopyable::Tuple<uint32_t, uint32_t> Type2;
		Type1 a(1,2);
		Type2 b(1u,2u);
		Type1 c(a);
		Type2 d(a);
		b = a;
		assert(TriviallyCopyable::get<0>(d) == TriviallyCopyable::get<0>(a));
		assert(TriviallyCopyable::get<1>(d) == TriviallyCopyable::get<1>(a));
	}

	{
		enum E {
			A = 0,
			B = 1,
		};
		typedef TriviallyCopyable::Tuple<int, E> Type1;
		Type1 a(1,A), b;
		std::memcpy(&b, &a, sizeof(Type1));
		std::cout << TriviallyCopyable::get<0>(b) << ' ' << TriviallyCopyable::get<1>(b) << std::endl;
	}

	{
		TriviallyCopyable::Tuple<int, double>a(1, 2.);
		std::tuple<int, double> b = a;
		assert(TriviallyCopyable::get<0>(a) == std::get<0>(b));
		assert(TriviallyCopyable::get<1>(a) == std::get<1>(b));
	}

	{
		std::tuple<int> x(1);
		std::tuple<std::tuple<uint32_t>> y(std::move(x));
		std::tuple<uint32_t> z(std::move(x));
	}

	{
		TriviallyCopyable::Tuple<int> x(1);
		// TriviallyCopyable::Tuple<TriviallyCopyable::Tuple<uint32_t>> y(std::move(x)); // TODO
		// TriviallyCopyable::Tuple<uint32_t> z(std::move(x)); // TODO
	}
//	TriviallyCopyable::Tuple<std::tuple<int>> c; // error
	return 0;
}
