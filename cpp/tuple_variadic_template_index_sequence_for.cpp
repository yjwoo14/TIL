#include <iostream>
#include <tuple>
#include <utility>
#include <vector>

template <typename T1, typename ...T2>
class fill_specific_indices_of_vector_with_tuple {
private:
	typedef std::tuple<T2...> tuple_type;

	const std::vector<size_t> & ids;

	template <size_t N>
	static void fill(
			std::vector<T1> & ret, 
			const std::vector<size_t> & ids, 
			const tuple_type & tuple) {
		ret[ids[N]] = std::get<N>(tuple);
	}
	
	template <size_t ...Is>
	static void fill_values(
			std::vector<T1> & ret, 
			const std::vector<size_t> & ids, 
			const tuple_type & tuple, 
			std::index_sequence<Is...>) {
		// initalizer list trick. 
		// Note initalizer list gurantees the order of augment evaluation
		// (x, 1) return 1, which is int type.
		std::initializer_list<int> x{(fill<Is>(ret, ids, tuple),1)...}; 
	}

public:
	static std::vector<T1> fill(const std::vector<size_t> & ids, const tuple_type & tuple) {
		// Assumption: The type of elements in the tuple is T
		//			   The type T has default constructor
		size_t vector_size = 0;
		for (const auto x : ids) 
			vector_size = std::max(vector_size, x+1);
		std::vector<T1> ret(vector_size);
		fill_values(ret, ids, tuple, std::index_sequence_for<T2...>{});
		return ret;
	}
};


int main(int argc, const char *argv[])
{
	std::tuple<int, int, int, int> tuple{1,2,3,4};
	const auto vec = fill_specific_indices_of_vector_with_tuple<int, int, int, int, int>::fill({4,6,2,1}, tuple);
	for (const auto & x : vec) 
		std::cout << x << " ";
	std::cout << std::endl;
	return 0;
}
