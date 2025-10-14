#include <climits>
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
#include <limits>

#include "concurrent-list.hpp"
#include "cache-manager.hpp"

namespace cm {

static const int LEN = std::numeric_limits<int>::max() >> 4;

class CoarseTest : public testing::Test {
protected:
	CoarseTest() 
	{
		std::random_device rd;

		std::mt19937 genInt(rd());
		std::uniform_int_distribution<int> distInt(0, 9);
		
		std::mt19937 genStr(rd());
		std::string charset = 
			"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
		std::uniform_int_distribution<std::string::size_type> distStr(0,
			charset.length() - 1);

		for (int i = 0; i < LEN; ++i) {
			int randInt = distInt(genInt);
			_coarseInt.pushBack(randInt);

			char c = charset[distStr(genStr)];
			std::string randStr(1, c);
			_coarseStr.pushBack(randStr);
		}
	}

	CoarseConcurrentList<int> _emptyInt;
	CoarseConcurrentList<std::string> _emptyStr;
	CoarseConcurrentList<int> _coarseInt;
	CoarseConcurrentList<int> _coarseIntEmpty;
	CoarseConcurrentList<std::string> _coarseStr;
	CoarseConcurrentList<std::string> _coarseStrEmpty;
};

TEST_F(CoarseTest, isEmpty) {
	EXPECT_TRUE(_emptyInt.isEmpty());
	EXPECT_TRUE(_emptyStr.isEmpty());
	EXPECT_FALSE(_coarseStr.isEmpty());
	EXPECT_FALSE(_coarseInt.isEmpty());
}

TEST_F(CoarseTest, popBack) {
	for (int i = 0; i < LEN; ++i) {
		_coarseInt.popBack();
		_coarseStr.popBack();
	}

	EXPECT_TRUE(_coarseInt.isEmpty());
	EXPECT_TRUE(_coarseStr.isEmpty());
}

TEST_F(CoarseTest, pushBack) {
	int thread_count = 8;
	std::vector<std::thread> threads;
	for (int t = 0; t < thread_count; ++t) {
		threads.emplace_back([this]() {
			for (int i = 0; i < LEN; ++i) {
				_coarseIntEmpty.pushBack(i);
				_coarseIntEmpty.popBack();
			}
		});
	}

	for (auto &th : threads) {
		th.join();
	}

	EXPECT_TRUE(_coarseIntEmpty.isEmpty());
}

int main(int argc, char **argv) {
	std::cout << "Running test suite:\n";
	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}

} // namespace cm
