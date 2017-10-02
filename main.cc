
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <functional>

#include "thread_pool/thread_pool.hpp"

// Simple timing class
class Clock {
public:
	void start() {
		time_start = std::chrono::high_resolution_clock::now();
	}

	void stop() {
		time_end = std::chrono::high_resolution_clock::now();
	}

	double seconds() const {
		std::chrono::duration<double> delta = time_end - time_start;
		return delta.count();
	}
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> time_start, time_end;
};

namespace task
{
	template <typename Iter, typename In> void map(Iter it, Iter end, std::function<void(In)> func)
	{
		thread_pool</*Out,*/ In> pool(std::thread::hardware_concurrency());
		for (; it != end; ++it)
		{
			pool.schedule(std::bind(func,(*it)));
		}
	}

}

void heavy_computation(int number)
{
	volatile bool is_prime = true;
	for (volatile int i = 2; i <= sqrt(number); i++)
	{
		if (number % i == 0) {
			is_prime = false;
			return;
		}
	}
	return;
}

int main()
{
	std::random_device rnd_device;
	std::mt19937 mersenne_engine(rnd_device());
	std::uniform_int_distribution<int> dist(100000, INT32_MAX);
	auto gen = std::bind(dist, mersenne_engine);

	std::vector<int> v(200000);
	std::generate(v.begin(), v.end(), gen);
	Clock clock;
	int x = 0;
	//warmup
	{
		clock.start();
		for (auto i : v)
			heavy_computation(i);
		clock.stop();
		std::cout << "warmup time: " << clock.seconds() << '\n';
	}

	//single thread
	{
		clock.start();
		for (auto i : v)
			heavy_computation(i);
		clock.stop();
		std::cout << "single thread time: " << clock.seconds() << '\n';
	}

	//concurrent thread
	{
		clock.start();
		task::map(v.begin(), v.end(), std::function<void(int)>(heavy_computation));
		clock.stop();
		std::cout << "concurrent time: " << clock.seconds() << '\n';
	}

	return 0;
}