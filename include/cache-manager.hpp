#pragma once

#include "concurrent-list.hpp"

#include <tbb/concurrent_set.h>
#include <tbb/concurrent_unordered_map.h>
#include <tbb/enumerable_thread_specific.h>

#include <cassert>
#include <exception>
#include <format>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <utility>
#include <iostream>

#define NDEBUG 1

namespace cm {

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
		static thread_local bool registered = [] {
			std::lock_guard<std::mutex> g(registry_mutex);
			registry.push_back(&local_bench);
			return true;
		}();
		[[maybe_unused]] registered;
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

template <typename BenchT> Benchmark benchmark() {
	return BenchT::aggregate();
}

void printBenchmark(const Benchmark& bm) {
	std::cout << "hits:\t" << bm.hits << "\n"
		<< "misses:\t" << bm.misses << "\n"
		<< "evictions:\t" << bm.evictions << "\n"
		<< "hit ratio:\t" << bm.hit_ratio << "\n";
}

void writeBenchmark(const Benchmark &bm) {
}

template <typename K, typename V>
using ListEntry = std::pair<K, V>; // cache key, cache value

template <typename T> struct Less {
	constexpr bool operator()(const T &lhs, const T &rhs) const {
		// tbb requires strict weak ordering
		if (lhs->ele.second != rhs->ele.second) {
			return lhs->ele.second < rhs->ele.second;
		}
		if (lhs->ele.first != rhs->ele.first) {
			return lhs->ele.first < rhs->ele.first;
		}
		// tie breaker for strict weak ordering
		return lhs < rhs;
	}
};

template <typename T> struct Greater {
	constexpr bool operator()(const T &lhs, const T &rhs) const {
		// tbb requires strict weak ordering
		if (lhs->ele.second != rhs->ele.second) {
			return lhs->ele.second > rhs->ele.second;
		}
		if (lhs->ele.first != rhs->ele.first) {
			return lhs->ele.first > rhs->ele.first;
		}
		// tie breaker for strict weak ordering
		return lhs > rhs;
	}
};

template <typename K, typename V, typename BenchT = NoneBench,
		  typename ConcurrentListT = CoarseConcurrentList<ListEntry<K, V>>,
		  typename ListNodePtrT = const CoarseListNode<ListEntry<K, V>> *,
		  typename Cmp = Less<ListNodePtrT>,
		  typename ConcurrentHashMapT =
			  tbb::concurrent_unordered_map<K, ListNodePtrT>,
		  typename ConcurrentBstT = tbb::concurrent_set<ListNodePtrT, Cmp>>
class CacheManager {
  private:
	size_t _capacity;

	mutable std::mutex _mutex;

	ConcurrentListT _cache;
	ConcurrentHashMapT _map;
	ConcurrentBstT _sorted;

  public:
	using ListNodePtr = ListNodePtrT;

	explicit CacheManager(size_t capacity, Cmp cmp = Cmp())
		: _capacity(capacity), _map(capacity), _sorted(cmp) {}

	void unsafeWarmCache(std::vector<std::pair<K, V>> data) {
		size_t size = data.size();
		for (auto i = 0; i < 2 * size; ++i) {
			add(data[i % size].first, data[i % size].second);
		}
	}

	std::optional<V> getItem(const K &key) {
		std::lock_guard<std::mutex> g(_mutex);

		auto it = _map.find(key);
		if (it == _map.end()) {
			BenchT::miss();
			return std::nullopt;
		}

		auto node = it->second;

		if (!_cache.removeAndPushFront(node)) {
			BenchT::miss();
			return std::nullopt;
		}

		BenchT::hit();
		return node->ele.second;
	}

	bool add(const K &key, const V &value) {
		// xxx can be more fine-grained. was causing races
		{
			std::lock_guard<std::mutex> g(_mutex);	// locked here
			auto it = _map.find(key);
			if (it != _map.end()) {
				// update
				auto node =
					const_cast<CoarseListNode<ListEntry<K, V>> *>(it->second);
				node->ele.second = value;
				_cache.removeAndPushFront(
					const_cast<const CoarseListNode<ListEntry<K, V>> *>(node));
				BenchT::hit();
				return true;
			}

			auto node = _cache.pushFront(ListEntry<K, V>{key, value});
			_map.insert({key, node});	// xxx th
			_sorted.insert(node);
			BenchT::miss();
		}

		// loose lock check
		if (_cache.size() >= _capacity) {
			BenchT::eviction();
			evict();
		}

		return true;
	}

	bool isEmpty() const {
#ifndef NDEBUG
		std::lock_guard<std::mutex> g(_mutex);
		assert(_cache.isEmpty() == _map.empty() &&
			   _map.empty() == _sorted.empty());
#endif
		return _cache.isEmpty();
	}

	bool contains(const K &key) const {
#ifndef NDEBUG
		std::lock_guard<std::mutex> g(_mutex);
		auto it = _map.find(key);
		if (it != _map.end()) {
			assert(_cache.contains(it->second));
			assert(_sorted.contains(it->second));
		}
#endif
		std::lock_guard<std::mutex> g(_mutex);
		auto it = _map.find(key);	// xxx th
		if (it != _map.end()) {
			BenchT::hit();
			return true;
		} else {
			BenchT::miss();
			return false;
		}
	}

	size_t getNumberOfItems() const {
#ifndef NDEBUG
		std::lock_guard<std::mutex> g(_mutex);
		assert(_cache.unsafeSize() == _map.unsafe_size() &&
			   _map.unsafe_size() == _sorted.unsafe_size());
#endif
		return _cache.size();
	}

	bool remove(const K &key) {
		// xxx better granularity
		std::lock_guard<std::mutex> g(_mutex);
		auto it = _map.find(key);
		if (it == _map.end()) {
			BenchT::miss();
			return false;
		}
		BenchT::hit();

		auto node = it->second;

		_map.unsafe_erase(key);
		assert(!_map.contains(key));

		_sorted.unsafe_erase(node);
		assert(!_sorted.contains(node));

		if (!_cache.remove(node)) {
			throw std::runtime_error(
				std::format("Cache failed to pop (key, value): ({}, {})", key,
							node->ele.second));
		}
		assert(!_cache.contains(node));

		assert(_cache.size() == _map.size() && _map.size() == _sorted.size());

		return true;
	}

	void clear() {
		// atomic synchronization of containers
		std::lock_guard<std::mutex> g(_mutex);

		_cache.clear();
		_map.clear();
		_sorted.clear();

		assert(_cache.isEmpty());
		assert(_map.empty());
		assert(_sorted.empty());
	}

	static Benchmark benchmark() { return BenchT::aggregate(); }

  private:
	void evict() {
		// atomic synchronization of containers
		std::lock_guard<std::mutex> lk(_mutex);

		std::optional<ListEntry<K, V>> opt = _cache.back();
		if (!opt) {
			throw std::runtime_error(
				"Cache expected to evict, but nothing to evict");
		}
		ListEntry<K, V> lentry = *opt;

		auto it = _map.find(lentry.first);
		if (it == _map.end()) {
			throw std::runtime_error(std::format(
				"Cache attempted to evict key {}, but not found in map",
				lentry.first));
		}

		_sorted.unsafe_erase(it->second);
		_map.unsafe_erase(it);
		_cache.popBack();
	}
};

} // end namespace cm
