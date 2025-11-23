#include "cache-manager.hpp"
#include "benchmark.hpp"
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
#include <condition_variable>
#include <nlohmann/json.hpp>
#include <optional>
#include <set>
#include <thread>
#include <unordered_map>
#include <vector>
#include <algorithm>

#define NDEBUG 1

using namespace cache;

namespace {
    constexpr size_t DEFAULT_NUM_SHARDS = 31;
}

template <typename K, typename V, typename BenchT>
struct ThreadShard {
    size_t capacity;
    std::list<std::pair<K, V>> lru_list;
    std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator> map;
    std::mutex mutex;

    ThreadShard(size_t cap) : capacity(cap) {}
};

template <typename K, typename V, typename BenchT>
struct CacheManager<K, V, BenchT>::Impl {
    size_t shard_capacity;
    tbb::concurrent_unordered_map<size_t, ThreadShard<K, V, BenchT>> shards;
    
    Impl(size_t cap) : shard_capacity(cap) {}
	
	ThreadShard<K, V, BenchT>& getShard(const K &key) {
        size_t hash = std::hash<K>{}(key);
        size_t shard_id = hash % DEFAULT_NUM_SHARDS;
        
        auto [shard_it, inserted] = shards.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(shard_id),
            std::forward_as_tuple(shard_capacity)
        );
        
        return shard_it->second;
    }
};

template <typename K, typename V, typename BenchT>
CacheManager<K, V, BenchT>::CacheManager(size_t shard_capacity)
    : pImpl(std::make_unique<Impl>(shard_capacity)) {
}

template <typename K, typename V, typename BenchT>
CacheManager<K, V, BenchT>::~CacheManager() = default;

template <typename K, typename V, typename BenchT>
CacheManager<K, V, BenchT>::CacheManager(CacheManager&&) noexcept = default;

template <typename K, typename V, typename BenchT>
CacheManager<K, V, BenchT>& CacheManager<K, V, BenchT>::operator=(CacheManager&&) noexcept = default;

template <typename K, typename V, typename BenchT>
std::optional<V> CacheManager<K, V, BenchT>::getItem(const K &key) {
	auto &shard = pImpl->getShard(key);
	
	std::lock_guard<std::mutex> lock(shard.mutex);
	auto it = shard.map.find(key);
	if (it == shard.map.end()) {
		BenchT::miss();
		return std::nullopt;
	}

	shard.lru_list.splice(shard.lru_list.begin(), shard.lru_list, it->second);
	BenchT::hit();
	return it->second->second;
}

template <typename K, typename V, typename BenchT>
bool CacheManager<K, V, BenchT>::add(const K &key, const V &value) {
	auto &shard = pImpl->getShard(key);

	std::lock_guard<std::mutex> lock(shard.mutex);
	auto it = shard.map.find(key);
	if (it != shard.map.end()) {
		it->second->second = value;
		shard.lru_list.splice(shard.lru_list.begin(), shard.lru_list, 
							  it->second);
		return true;
	}

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

template <typename K, typename V, typename BenchT>
bool CacheManager<K, V, BenchT>::contains(const K &key) {
	auto &shard = pImpl->getShard(key);
	
	std::lock_guard<std::mutex> lock(shard.mutex);
	auto it = shard.map.find(key);
	
	if (it != shard.map.end()) {
		BenchT::hit();
		return true;
	}

	BenchT::miss();
	return false;
}

template <typename K, typename V, typename BenchT>
bool CacheManager<K, V, BenchT>::remove(const K &key) {
	auto &shard = pImpl->getShard(key);
	
	std::lock_guard<std::mutex> lock(shard.mutex);
	auto it = shard.map.find(key);
	if (it == shard.map.end()) {
		return false;
	}

	shard.lru_list.erase(it->second);
	shard.map.erase(it);
	return true;
}

template <typename K, typename V, typename BenchT>
void CacheManager<K, V, BenchT>::clear() {
    for (auto &[shard_id, shard] : pImpl->shards) {
        std::lock_guard<std::mutex> lock(shard.mutex);
        shard.lru_list.clear();
        shard.map.clear();
    }
}

template <typename K, typename V, typename BenchT>
bench::Benchmark CacheManager<K, V, BenchT>::benchmark() {
	return BenchT::aggregate();
}

/*
 * Explicit instantiations exported (for compiled .o).
 */
template class cache::CacheManager<int, int>;
template class cache::CacheManager<int, int, bench::ThreadBench>;
template class cache::CacheManager<int, int, bench::TbbBench>;
template class cache::CacheManager<int, std::string>;
template class cache::CacheManager<int, std::string, bench::ThreadBench>;
template class cache::CacheManager<int, std::string, bench::TbbBench>;
template class cache::CacheManager<std::string, std::string>;
template class cache::CacheManager<std::string, std::string, bench::ThreadBench>;
template class cache::CacheManager<std::string, std::string, bench::TbbBench>;
template class cache::CacheManager<double, double>;
template class cache::CacheManager<double, double, bench::ThreadBench>;
template class cache::CacheManager<double, double, bench::TbbBench>;
