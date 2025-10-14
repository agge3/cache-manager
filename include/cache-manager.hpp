#pragma once

#include "concurrent-list.hpp"

#include <tbb/concurrent_unordered_map.h>
#include <tbb/concurrent_set.h>

#include <functional>
#include <memory>
#include <optional>
#include <cassert>
#include <mutex>
#include <exception>
#include <utility>
#include <format>
#include <optional>

#define NDEBUG 1

namespace cm {

template <typename K, typename V>
using ListEntry = std::pair<K, V>;	// cache key, cache value

template <typename T>
struct Less {
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

template <typename T>
struct Greater {
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

template <
	typename K,
	typename V,
	typename ConcurrentListT = CoarseConcurrentList<ListEntry<K, V>>,
	typename ListNodePtrT = const CoarseListNode<ListEntry<K, V>> *,
	typename ConcurrentHashMapT = tbb::concurrent_unordered_map<K, ListNodePtrT>,
	typename Cmp = Less<ListNodePtrT>,
	typename ConcurrentBstT = tbb::concurrent_set<ListNodePtrT, Cmp>
>
class CacheManager {
private:
	size_t _capacity;

	mutable std::mutex _mutex;

	ConcurrentListT _cache;
	ConcurrentHashMapT _map;
	ConcurrentBstT _sorted;
public:
	using ListNodePtr = ListNodePtrT;

	explicit CacheManager(size_t capacity, Cmp cmp = Cmp()) :
		_capacity(capacity),
		_map(capacity),
		_sorted(cmp)
	{}

	std::optional<V> getItem(const K& key) {
		std::lock_guard<std::mutex> g(_mutex);
		
		auto it = _map.find(key);
		if (it == _map.end()) {
			return std::nullopt;
		}
		
		auto node = it->second;

		if (!_cache.removeAndPushFront(node)) {
			return std::nullopt;
		}

		return node->ele.second;
	}

	bool add(const K& key, const V& value) {
		// xxx can be more fine-grained. was causing races
		{
			std::lock_guard<std::mutex> g(_mutex);
			auto it = _map.find(key);
			if (it != _map.end()) {
				// update
				auto node = const_cast<CoarseListNode<ListEntry<K, V>> *>(it->second);
				node->ele.second = value;
				_cache.removeAndPushFront(const_cast<const CoarseListNode<ListEntry<K , V>> *>(node));
				return true;
			}

			auto node = _cache.pushFront(ListEntry<K, V>{key, value});
			_map.insert({key, node});
			_sorted.insert(node);
		}

		// loose lock check
		if (_cache.size() >= _capacity) {
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

	bool contains(const K& key) const {
#ifndef NDEBUG
		std::lock_guard<std::mutex> g(_mutex);
		auto it = _map.find(key);
		if (it != _map.end()) {
			assert(_cache.contains(it->second));
			assert(_sorted.contains(it->second));
		}
#endif
		return _map.find(key) != _map.end();
	}

	size_t getNumberOfItems() const {
#ifndef NDEBUG
		std::lock_guard<std::mutex> g(_mutex);
		assert(_cache.unsafeSize() == _map.unsafe_size() &&
			_map.unsafe_size() == _sorted.unsafe_size());
#endif
		return _cache.size();
	}

	bool remove(const K&key) {
		auto it = _map.find(key);
		if (it == _map.end()) {
			return false;
		}

		auto node = it->second;
		
		// atomic synchronization of containers
		std::lock_guard<std::mutex> lk(_mutex);

		_map.unsafe_erase(key);
		assert(!_map.contains(key));

		_sorted.unsafe_erase(node);
		assert(!_sorted.contains(node));

		if (!_cache.remove(node)) {
			throw std::runtime_error(std::format(
				"Cache failed to pop (key, value): ({}, {})",
				key, node->ele.second));
		}
		assert(!_cache.contains(node));

		assert(_cache.size() == _map.size() &&
			_map.size() == _sorted.size());

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
