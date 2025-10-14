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

namespace cm {

template <typename K, typename V>
using ListEntry = std::pair<K, V>;	// cache key, cache value

template <typename T>
struct Less {
	constexpr bool operator()(const T &lhs, const T &rhs) {
		return lhs.value.second < rhs.value.second;
	}
};

template <typename T>
struct Greater {
	constexpr bool operator()(const T &lhs, const T &rhs) {
		return lhs.value.second > rhs.value.second;
	}
};

template <
	typename K,
	typename V,
	typename ConcurrentListT = CoarseConcurrentList<ListEntry<K, V>>,
	typename ListNodePtrT = std::shared_ptr<CoarseListNode<ListEntry<K, V>>>,
	typename ConcurrentHashMapT = tbb::concurrent_unordered_map<K, ListNodePtrT>,
	typename Cmp = Less<ListNodePtrT>,
	typename ConcurrentBstT = tbb::concurrent_set<ListNodePtrT, Cmp>
>
class CacheManager {
private:
	size_t _capacity;

	std::mutex _mutex;

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
		auto it = _map.find(key);
		if (it == _map.end()) {
			return std::nullopt;
		}

		auto node = it->second;

		_cache.removeAndPushFront(node.get());

		return node->ele.second;
	}

	bool add(const K& key, const V& value) {
		auto it = _map.find(key);
		if (it != _map.end()) {
			// update
			auto node = it->second;

			// atomic synchronization of containers
			std::lock_guard<std::mutex> g(_mutex);
			node->ele.second = value;
			_cache.removeAndPushFront(node.get());

			return true;
		}

		{
			// atomic synchronization of containers
			std::lock_guard<std::mutex> g(_mutex);
			auto ptr = std::make_shared<ListNodePtr>(pushFront(value));
			_map.insert({key, ptr});
			_sorted.insert(ptr);
		}

		if (_cache.unsafeSize() >= _capacity) {
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
			assert(_cache.contains(it->second.get()));
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
		return _cache.unsafeSize();
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

		if (!_cache.remove(node.get())) {
			throw std::runtime_error(std::format(
				"Cache failed to pop (key, value): ({}, {})",
				key, node->ele.second));
		}
		assert(!_cache.contains(node.get()));

		assert(_cache.unsafeSize() == _map.unsafe_size() &&
			_map.unsafe_size() == _sorted.unsafe_size());

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
