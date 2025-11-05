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
#include <nlohmann/json.hpp>
#include <optional>
#include <set>
#include <thread>
#include <unordered_map>
#include <vector>

#define NDEBUG 1

using namespace cache;

template <typename K, typename V, typename BenchT>
CacheManager<K, V, BenchT>::CacheManager(size_t shard_capacity)
	: _shard_capacity(shard_capacity) {}

template <typename K, typename V, typename BenchT>
typename CacheManager<K, V, BenchT>::ThreadShard &CacheManager<K, V, BenchT>::getShard() {
	auto tid = std::this_thread::get_id();
	auto it = _shards.find(tid);
	if (it != _shards.end())
		return it->second;

	auto [new_it, inserted] =
		_shards.emplace(tid, ThreadShard(_shard_capacity));
	return new_it->second;
}

template <typename K, typename V, typename BenchT>
std::optional<V> CacheManager<K, V, BenchT>::getItem(const K &key) {
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

template <typename K, typename V, typename BenchT>
bool CacheManager<K, V, BenchT>::add(const K &key, const V &value) {
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

template <typename K, typename V, typename BenchT>
bool CacheManager<K, V, BenchT>::contains(const K &key) {
	auto &shard = getShard();
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

template <typename K, typename V, typename BenchT>
void CacheManager<K, V, BenchT>::clear() {
	for (auto &[tid, shard] : _shards) {
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
template class CacheManager<int, int>;
template class CacheManager<int, int, bench::ThreadBench>;
template class CacheManager<int, int, bench::TbbBench>;
template class CacheManager<int, std::string>;
template class CacheManager<int, std::string, bench::ThreadBench>;
template class CacheManager<int, std::string, bench::TbbBench>;
