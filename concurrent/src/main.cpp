#include <gtest/gtest.h>
#include <oneapi/tbb/concurrent_unordered_map.h>
#include <tbb/concurrent_unordered_map.h>
#include <chrono>
#include <random>
#include <thread>
#include <vector>
#include <iostream>
#include <set>
#include <mutex>
#include <atomic>
#include <iomanip>

#include "concurrent-list.hpp"

namespace cm {

int main(int argc, char **argv) {
	IConcurrentList<int> *coarse = new CoarseConcurrentList<int>;
	IConcurrentList<int> *fine = new FineConcurrentList<int>;
	auto m = tbb::concurrent_unordered_map<int, int>{};

	return 0;
}

} // namespace cm
