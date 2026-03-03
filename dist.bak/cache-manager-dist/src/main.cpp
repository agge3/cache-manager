#include "cache-manager.hpp"
#include "benchmark.hpp"

#include <iostream>
#include <string>

#define TEST_KEY 1
#define TEST_VAL 1
#define TEST_SZ 4096

int main(int argc, char **argv) {
	cache::CacheManager<int, int> cm;
	std::cout << "Hello, World!\n";

	cm.add(TEST_KEY, TEST_VAL);
	auto opt = cm.getItem(TEST_KEY);
	bool found = false;
	if (opt) {
		std::cout << "v: " << *opt << "\n";
		found = true;
	} else {
		std::cout << "v: FAILED\n";
	}

	if (found != cm.contains(TEST_KEY)) {
		std::cout << "contains: FAILED\n";
	} else {
		std::cout << "contains: SUCCESS\n";
	}

	cm.clear();
	if (cm.contains(TEST_KEY)) {
		std::cout << "clear: FAILED\n";
	} else {
		std::cout << "clear: SUCCESS\n";
	}

	cache::CacheManager<int, std::string, bench::TbbBench> cm2;
	for (auto i = 0; i < TEST_SZ; ++i) {
		std::string s(1, static_cast<char>(i % 256));
		cm2.add(i, s);
		auto opt2 = cm2.getItem(i);
		bool found2 = cm2.contains(i);
	}
	cm2.clear();
	auto bm = cm2.benchmark();
	bench::printBenchmark(bm);

	return 0;
}
