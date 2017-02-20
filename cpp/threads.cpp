#include <future>
#include <iostream>
#include <thread>
#include <chrono>

int future_await() {
	// C# style.. not even in c++17 yet..
	/*
	auto async_call = []() {
		return std::async(std::launch::async,
				[](const std::string & arg) {
					using namespace std::chrono_literals;
					std::cout << arg << std::endl;
					std::this_thread::sleep_for(1s);
					return std::string("Future");
				}, "Thread begin");
	};

	std::cout << await async_call(); << std::endl;
	*/
}

int future_async() {
	std::future<std::string> fut = std::async(std::launch::async,
		[](const std::string & arg) {
			using namespace std::chrono_literals;
			std::cout << arg << std::endl;
			std::this_thread::sleep_for(1s);
			return std::string("Future");
		}, "Thread begin");

	fut.wait(); // fut.get() method effectively call wait, so it's unnecessary to call here
	std::cout << fut.get() << std::endl;
}

int promise_future_thread() {
	std::promise<std::string> prom;
	std::future<std::string> fut = prom.get_future();
	std::thread thr([&prom](const std::string & arg) {
		using namespace std::chrono_literals;
		std::cout << arg << std::endl;
		std::this_thread::sleep_for(1s);
		prom.set_value("Future");
		std::this_thread::sleep_for(1s);
		std::cout << "Done" << std::endl;
	}, "Thread begin");

	fut.wait(); // fut.get() method effectively call wait, so it's unnecessary to call here
	std::cout << fut.get() << std::endl;
	thr.join(); // wait thread..
}

int main(int argc, const char *argv[])
{
	promise_future_thread();
	future_async();
	return 0;
}
