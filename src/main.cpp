#include <atomic>
#include <chrono>
#include <climits>
#include <gtest/gtest.h>
#include <iomanip>
#include <iostream>
#include <latch>
#include <limits>
#include <mutex>
#include <oneapi/tbb/concurrent_unordered_map.h>
#include <random>
#include <set>
#include <tbb/concurrent_unordered_map.h>
#include <thread>
#include <vector>

#include "cache-manager.hpp"
#include "concurrent-list.hpp"

namespace cm {

static const int LEN = std::numeric_limits<int>::max() >> 12;

class CoarseTest : public testing::Test {
  protected:
	CoarseTest() {
		std::random_device rd;

		std::mt19937 genInt(rd());
		std::uniform_int_distribution<int> distInt(0, 9);

		std::mt19937 genStr(rd());
		std::string charset =
			"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
		std::uniform_int_distribution<std::string::size_type> distStr(
			0, charset.length() - 1);

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
	int thread_count = 2;
	std::vector<std::thread> threads;
	for (int t = 0; t < thread_count; ++t) {
		threads.emplace_back([this]() {
			for (int i = 0; i < LEN; ++i) {
				auto node = std::make_shared<int>(i);

				_coarseIntEmpty.contains(*node);
				_coarseIntEmpty.get(*node);

				_coarseIntEmpty.popBack();
			}
		});
	}

	for (auto &th : threads) {
		th.join();
	}

	EXPECT_TRUE(_coarseIntEmpty.isEmpty());
}

class CacheManagerTest : public ::testing::Test {
  protected:
	static constexpr size_t CACHE_CAPACITY = 100;
	static constexpr size_t NUM_THREADS = 4;
	static constexpr size_t OPERATIONS_PER_THREAD = 1000;

	CacheManager<int, std::string, TbbBench> cache{CACHE_CAPACITY};
};

// Test 1: Concurrent puts and gets (safe for raw pointer cache)
TEST_F(CacheManagerTest, ConcurrentPutsAndGets) {
	std::latch sync_point(NUM_THREADS);

	auto worker = [&](int thread_id) {
		sync_point.arrive_and_wait();

		size_t base_key = thread_id * OPERATIONS_PER_THREAD;
		for (size_t i = 0; i < OPERATIONS_PER_THREAD; ++i) {
			int key = base_key + i;
			std::string value = "value_" + std::to_string(key);

			cache.add(key, value);

			auto item = cache.getItem(key);
			auto contains = cache.contains(key);
			auto empty = cache.isEmpty();
			auto size = cache.getNumberOfItems();
			if (item) {
				EXPECT_EQ(*item, value);
				EXPECT_EQ(contains, true);
				EXPECT_EQ(empty, false);
				EXPECT_FALSE(empty);
				EXPECT_TRUE(size > 0);

				//auto remove = cache.remove(key);
				//EXPECT_EQ(remove, true);
				EXPECT_FALSE(cache.contains(key));
			} else {
				EXPECT_EQ(contains, false);
			}
		}
	};

	std::vector<std::thread> threads;
	for (size_t i = 0; i < NUM_THREADS; ++i) {
		threads.emplace_back(worker, i);
	}

	for (auto &th : threads) {
		th.join();
	}

	auto bm = cache.benchmark();
	printBenchmark(bm);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}

} // namespace cm
