#pragma once

#include "macros.hpp"
#include <tbb/concurrent_queue.h>
#include <tbb/concurrent_unordered_map.h>
#include <tbb/concurrent_vector.h>
#include <tbb/enumerable_thread_specific.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
#include <optional>
#include <set>
#include <thread>
#include <unordered_map>
#include <vector>

#define NDEBUG 1

namespace cm {

/**
 * Metrics produced by the cache manager.
 */
struct Benchmark {
	size_t hits = 0; /* amount of successful hits on the cache */
	size_t misses = 0; /* amount of failed hits on the cache */
	size_t evictions = 0; /* amount of evictions in the cache */
	float hit_ratio = 0; /* percentage of hits on the cache vs misses */
	float calc_hit_ratio() const {
		const float total = static_cast<float>(hits) + misses;
		return total == 0.0f ? 0.0f : static_cast<float>(hits) / total;
	};
};

/**
 * No-op bench for benchless runs.
 */
struct NoneBench {
	static inline void hit() {}
	static inline void miss() {}
	static inline void eviction() {}
	static Benchmark aggregate() { return {}; }
};

/**
 * Threaded benchmark.
 */
struct ThreadBench {
	/* mutex for adding to the registry */
	static inline std::mutex registry_mutex;
	
	/* current threads running */
	static inline std::vector<Benchmark *> registry;
	
	/* benchmark per thread */
	static thread_local Benchmark local_bench;

	/**
	 * Registers thread to the registry.
	 */
	static void register_thread() {
		[[maybe_unused]] static thread_local bool registered = [] {
			std::lock_guard<std::mutex> g(registry_mutex);
			registry.push_back(&local_bench);
			return true;
		}();
	}

	/**
	 * Logs if we had a hit.
	 */
	static inline void hit() {
		register_thread();
		++local_bench.hits;
	}

	/**
	 * Logs if we had a miss.
	 */
	static inline void miss() {
		register_thread();
		++local_bench.misses;
	}

	/**
	 * Logs if we had an eviction.
	 */
	static inline void eviction() {
		register_thread();
		++local_bench.evictions;
	}

	/**
	 * Accumulates all metrics for the given thread.
	 *
	 * @return The combined benchmark metric.
	 */
	static Benchmark aggregate() {
		std::lock_guard<std::mutex> g(registry_mutex);
		Benchmark bm{};
		for (auto &p : registry) {
			bm.hits += p->hits;
			bm.misses += p->misses;
			bm.evictions += p->evictions;
		}
		bm.hit_ratio = bm.calc_hit_ratio();
		return bm;
	}
};

/**
 * Tbb based benchmark.
 */
struct TbbBench {
	/* container for local benchmark metrics */
	static inline tbb::enumerable_thread_specific<Benchmark> ets;

	/* logs if we had a hit */
	static inline void hit() { ++ets.local().hits; }
	/* logs if we had a miss */
	static inline void miss() { ++ets.local().misses; }
	/* logs if we had an eviction */
	static inline void eviction() { ++ets.local().evictions; }

	/**
	 * Accumulates all benchmark metrics of the current thread.
	 */
	static Benchmark aggregate() {
		Benchmark bm{};
		for (auto &t : ets) {
			bm.hits += t.hits;
			bm.misses += t.misses;
			bm.evictions += t.evictions;
		}
		bm.hit_ratio = bm.calc_hit_ratio();
		return bm;
	}
};

template <typename BenchT> Benchmark benchmark() { return BenchT::aggregate(); }

/**
 * Prints the current benchmark metrics to std::out.
 */
void printBenchmark(const Benchmark &bm) {
	std::cout << "hits:\t\t" << bm.hits << "\n"
			  << "misses:\t\t" << bm.misses << "\n"
			  << "evictions:\t" << bm.evictions << "\n"
			  << "hit ratio:\t" << bm.hit_ratio << "\n";
}

/**
 * Writes benchmark to JSON file. Appends to it.
 */
void writeBenchmark(const Benchmark &bm) {
	std::string filename = "benchmark.jsonl";

	int run_number = 1;
	if (std::filesystem::exists(filename)) {
		std::ifstream infile(filename);
		std::string line;
		while (std::getline(infile, line)) {
			if (!line.empty()) {
				run_number++;
			}
		}
		infile.close();
	}

	nlohmann::json bench;

	bench["run"] = run_number;
	bench["hits"] = bm.hits;
	bench["misses"] = bm.misses;
	bench["evictions"] = bm.evictions;
	bench["hit_ratio"] = bm.hit_ratio;

	std::ofstream file(filename, std::ios::app);
	if (file.is_open()) {
		file << bench.dump() << "\n";
		file.close();
	}
}

template <typename K, typename V> using ListEntry = std::pair<K, V>;

/**
 * LRU Concurrent Cache Manager using TBB.
 */
template <typename K, typename V, typename BenchT = NoneBench>
class CacheManager {
  private:
	struct ThreadShard {
		size_t capacity;
		std::list<std::pair<K, V>> lru_list;
		std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator>
			map;

		ThreadShard(size_t cap) : capacity(cap) {}
	};

	/**
	 * The amount of shards for the cache manager.
	 */
	size_t _shard_capacity;
	
	/**
	 * Hashmap holding the shards of the cache manager.
	 *
	 * Each thread gets an assigned shard. Reduces serialization
	 * and increases parallelization.
	 */
	tbb::concurrent_unordered_map<std::thread::id, ThreadShard> _shards;

  public:
	CacheManager(size_t shard_capacity = 1024)
		: _shard_capacity(shard_capacity) {}

  private:
	
	/**
	 * Retrieves the shard of the current thread, or creates one for it
	 * if it does not exist already.
	 *
	 * @return The shard of the current thread.
	 */
	ThreadShard &getShard() {
		auto tid = std::this_thread::get_id();
		auto it = _shards.find(tid);
		if (it != _shards.end())
			return it->second;

		auto [new_it, inserted] =
			_shards.emplace(tid, ThreadShard(_shard_capacity));
		return new_it->second;
	}

  public:
	
	/**
	 * Gets an item from the cache.
	 *
	 * @param const K &key The requested key to retrieve from the cache.
	 *
	 * @return Either the requested key if found, or std::nullopt if not.
	 */
	std::optional<V> getItem(const K &key) {
		auto &shard = getShard();
		auto it = shard.map.find(key);
		if (it == shard.map.end()) {
			BenchT::miss();
			return std::nullopt;
		}

		shard.lru_list.splice(shard.lru_list.begin(), shard.lru_list,
							  it->second);
		BenchT::hit();
		return it->second->second;
	}

	/**
	 * Adds an item into the cache.
	 *
	 * @param const K &key The item's key.
	 * @param const V &value The item's value.
	 *
	 * @return True upon success.
	 */
	bool add(const K &key, const V &value) {
		auto &shard = getShard();
		auto it = shard.map.find(key);

		if (it != shard.map.end()) {
			it->second->second = value;
			shard.lru_list.splice(shard.lru_list.begin(), shard.lru_list,
								  it->second);
			BenchT::hit();
			return true;
		}

		BenchT::miss();

		shard.lru_list.push_front({key, value});
		shard.map[key] = shard.lru_list.begin();

		if (shard.lru_list.size() > shard.capacity) {
			auto last = shard.lru_list.back();
			shard.map.erase(last.first);
			shard.lru_list.pop_back();
			BenchT::eviction();
		}

		return true;
	}

	/**
	 * Checks whether a given key exists in the cache manager.
	 *
	 * @param const K &key The key to look for.
	 *
	 * @return True if found, false otherwise.
	 */
	bool contains(const K &key) {
		auto &shard = getShard();
		auto it = shard.map.find(key);
		if (it != shard.map.end()) {
			BenchT::hit();
			return true;
		}
		BenchT::miss();
		return false;
	}

	/**
	 * Removes a given key from the cache manager.
	 *
	 * @param const K &key The key to remove.
	 *
	 * @return True if removed, false if not removed.
	 */
	bool remove(const K &key) {
		auto &shard = getShard();
		auto it = shard.map.find(key);
		if (it == shard.map.end()) {
			BenchT::miss();
			return false;
		}

		BenchT::hit();
		shard.lru_list.erase(it->second);
		shard.map.erase(it);
		BenchT::eviction();
		return true;
	}

	/**
	 * Clears all values in the cache manager.
	 */
	void clear() {
		for (auto &[tid, shard] : _shards) {
			shard.lru_list.clear();
			shard.map.clear();
		}
	}

	static Benchmark benchmark() { return BenchT::aggregate(); }
};
} // namespace cm
