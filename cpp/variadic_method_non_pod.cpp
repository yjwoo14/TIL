#include <iostream>
#include <type_traits>

void f(...) {
	std::cout << "A variadic method is called." << std::endl;
}

struct POD {
	int a;
};
static_assert(std::is_pod<POD>::value, "POD must be pod type.");
static_assert(std::is_trivial<POD>::value, "POD must be trivial type.");

struct NONPOD {
	int a = 1;
};
static_assert(!std::is_pod<NONPOD>::value, "NONPOD must be non-pod type.");
static_assert(!std::is_trivial<NONPOD>::value, "NONPOD must be non-trivial type.");
static_assert(std::is_trivially_copyable<NONPOD>::value, "NONPOD must be trivial-copyable type.");

struct NONTRIVIALLYCOPYABLE {
	int a;
	NONTRIVIALLYCOPYABLE() = default;
	NONTRIVIALLYCOPYABLE(NONTRIVIALLYCOPYABLE const &) : a(1) {}
};
static_assert(!std::is_trivially_copyable<NONTRIVIALLYCOPYABLE>::value, "NONTRIVIALLYCOPYABLE must be non-trivially-copyable type.");

int main(int argc, const char *argv[])
{
	f();
	f(POD());
	f(NONPOD());

	// https://stackoverflow.com/questions/10083844/passing-non-pod-type-to-variadic-function-is-undefined-behavior
	// calling variadic method with non-trivially-copyable-type type arguments is IMPLEMENTED BEHAVIOR. 
	// clang-4 reject non-trivially-copyable type. (gcc-7 is okay)

	// C++11 5.2.2/7:
	// Passing a potentially-evaluated argument of class type having a
	// non-trivial copy constructor, a non-trivial move contructor, or a
	// non-trivial destructor, with no corresponding parameter, is
	// conditionally-supported with implementation-defined semantics.
	
	//f(NONTRIVIALLYCOPYABLE()); // g++-7 can compile this but not clang.

	return 0;
}
