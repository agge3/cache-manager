#pragma once

#include "macros.hpp"
#include <tbb/concurrent_unordered_map.h>
#include <tbb/concurrent_queue.h>
#include <tbb/enumerable_thread_specific.h>
#include <tbb/concurrent_vector.h>

#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <thread>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include <list>

#define NDEBUG 1

namespace cm {

// ------------------------- Benchmark Types -------------------------
struct Benchmark {
	size_t hits = 0;
	size_t misses = 0;
	size_t evictions = 0;
	float hit_ratio = 0;
	float calc_hit_ratio() const {
		const float total = static_cast<float>(hits) + misses;
		return total == 0.0f ? 0.0f : static_cast<float>(hits) / total;
	};
};

struct NoneBench {
	static inline void hit() {}
	static inline void miss() {}
	static inline void eviction() {}
	static Benchmark aggregate() { return {}; }
};

struct ThreadBench {
	static inline std::mutex registry_mutex;
	static inline std::vector<Benchmark *> registry;
	static thread_local Benchmark local_bench;

	static void register_thread() {
		[[maybe_unused]] static thread_local bool registered = [] {
			std::lock_guard<std::mutex> g(registry_mutex);
			registry.push_back(&local_bench);
			return true;
		}();
	}

	static inline void hit() {
		register_thread();
		++local_bench.hits;
	}
	static inline void miss() {
		register_thread();
		++local_bench.misses;
	}
	static inline void eviction() {
		register_thread();
		++local_bench.evictions;
	}

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

struct TbbBench {
	static inline tbb::enumerable_thread_specific<Benchmark> ets;

	static inline void hit() { ++ets.local().hits; }
	static inline void miss() { ++ets.local().misses; }
	static inline void eviction() { ++ets.local().evictions; }

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

void printBenchmark(const Benchmark &bm) {
	std::cout << "hits:\t\t" << bm.hits << "\n"
			  << "misses:\t\t" << bm.misses << "\n"
			  << "evictions:\t" << bm.evictions << "\n"
			  << "hit ratio:\t" << bm.hit_ratio << "\n";
}

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

template <typename K, typename V>
using ListEntry = std::pair<K, V>; // cache key, cache value
// ------------------------- CacheManager -------------------------
template <typename K, typename V, typename BenchT = NoneBench>
class CacheManager {
private:
    struct ThreadShard {
        size_t capacity;
        std::list<std::pair<K, V>> lru_list;
        std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator> map;

        ThreadShard(size_t cap) : capacity(cap) {}
    };

    size_t _shard_capacity;
    size_t _global_capacity;
    tbb::concurrent_unordered_map<std::thread::id, ThreadShard> _shards;
    tbb::concurrent_queue<std::pair<K, V>> _global_queue;
    std::mutex _evict_mutex;

public:
    CacheManager(size_t global_capacity, size_t shard_capacity = 1024)
        : _global_capacity(global_capacity), _shard_capacity(shard_capacity) {}

private:
    ThreadShard& getShard() {
        auto tid = std::this_thread::get_id();
        auto it = _shards.find(tid);
        if (it != _shards.end()) return it->second;

        // lazily construct shard
        auto [new_it, inserted] = _shards.emplace(tid, ThreadShard(_shard_capacity));
        return new_it->second;
    }

	void evictGlobal() {
	    std::lock_guard<std::mutex> g(_evict_mutex);
	
	    while (_global_queue.unsafe_size() > _global_capacity) {
	        std::pair<K, V> dummy;
	        _global_queue.try_pop(dummy); // ✅ provide a reference
	        BenchT::eviction();
	    }
	}

public:
    std::optional<V> getItem(const K& key) {
        auto& shard = getShard();
        auto it = shard.map.find(key);
        if (it == shard.map.end()) {
            BenchT::miss();
            return std::nullopt;
        }
        // move to front for LRU
        shard.lru_list.splice(shard.lru_list.begin(), shard.lru_list, it->second);
        BenchT::hit();
        return it->second->second;
    }

    bool add(const K& key, const V& value) {
        auto& shard = getShard();
        auto it = shard.map.find(key);

        if (it != shard.map.end()) {
            // update value & move to front
            it->second->second = value;
            shard.lru_list.splice(shard.lru_list.begin(), shard.lru_list, it->second);
            BenchT::hit();
            return true;
        }

        BenchT::miss();

        // insert new
        shard.lru_list.push_front({key, value});
        shard.map[key] = shard.lru_list.begin();

        if (shard.lru_list.size() > shard.capacity) {
            auto last = shard.lru_list.back();
            _global_queue.push(last); // push evicted item to global queue
            shard.map.erase(last.first);
            shard.lru_list.pop_back();
            evictGlobal();
        }

        return true;
    }

    bool contains(const K& key) {
        auto& shard = getShard();
        auto it = shard.map.find(key);
        if (it != shard.map.end()) {
            BenchT::hit();
            return true;
        }
        BenchT::miss();
        return false;
    }

    bool remove(const K& key) {
        auto& shard = getShard();
        auto it = shard.map.find(key);
        if (it == shard.map.end()) {
            BenchT::miss();
            return false;
        }

        BenchT::hit();
        shard.lru_list.erase(it->second);
        shard.map.erase(it);
        return true;
    }

    void clear() {
        for (auto& [tid, shard] : _shards) {
            shard.lru_list.clear();
            shard.map.clear();
        }
        while (!_global_queue.empty()) _global_queue.try_pop();
    }

    static Benchmark benchmark() { return BenchT::aggregate(); }
};
} // namespace cm

