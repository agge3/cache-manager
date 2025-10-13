#pragma once

#include "concurrent-list.hpp"
#include "concurrent-map.hpp"

#include <functional>
#include <memory>
#include <optional>
#include <cassert>
#include <mutex>
#include <exception>

namespace cm {

using NodePtr = const DLLNode<V>*;

template <typename K, typename V>
struct ListEntry {
	K key;
	V val;
};

template <typename K, typename Ptr>
struct BstEntry {
	K key;
	Ptr ptr;
};
	
using BstEntry = std::pair<K, Ptr>;

struct NodeLess {
	constexpr bool operator()(const Node& lhs, const Node& rhs) const noexcept {
		return lhs.value < rhs.value;
	}
};

template <
	typename K,
	typename V,
	typename Cmp = NodeLess,
	typename ConcurrentListT = CoarseGrainedList<ListEntry>
	typename ConcurrentHashMapT = tbb::concurrent_unordered_map<K, NodePtr>,
	typename ConcurrentBstT = tbb::concurrent_multiset<BstEntry,
		std::function<bool(const NodePtr&, const NodePtr&)>
>
class CacheManager {
private:
	size_t _capacity;
	size_t _size;

	std::mutex _mutex;

	ConcurrentListT _cache;
	ConcurrentHashMapT _map;
	ConcurrentBstT _sorted;
public:
	explicit CacheManager(size_t capacity, Cmp cmp = Cmp()) :
		_capacity(capacity),
		_sorted(cmp)
	{}

	std::optional<V> getItem(const K& key) {
		auto it = _map.find(key);
		if (it == _map.end()) {
			return std::nullopt;
		}

		NodePtr node = it->second;

		_cache.removeAndPushFront(node);

		return node->value;
	}

	bool add(const K& key, const V& value) {
		NodePtr node;

		auto it = _map.find(key);
		if (it != _map.end()) {
			// update
			node = it->second;

			// atomic synchronization of containers
			std::lock_guard<std::mutex> lk(_mutex);
			node->value = value;
			_cache.removeAndPushFront(node);

			return true;
		}

		node = std::make_shared<Node>(Node{key, value});
		{
			// atomic synchronization of containers
			std::lock_guard<std::mutex> lk(_mutex);
			_map.insert({key, node});
			_sorted.insert(node);
			_cache.pushFront(node);
		}

		if (_cache.unsafeSize() >= _capacity) {
			evict();
		}

		return true;
	}

	bool isEmpty() const {
#ifndef NDEBUG
		std::lock_guard<std::mutex> lk(_mutex);
		assert(_cache.isEmpty() == _map.empty() &&
			_map.empty() == _sorted.empty());
#endif
		return _cache.isEmpty();
	}

	bool contains(const K& key) const {
#ifndef NDEBUG
		std::lock_guard<std::mutex> lk(_mutex);
		auto it = _map.find(key);
		if (it != _map.end()) {
			assert(_cache.contains(it->second));
			assert(_sorted.contains({key, it->second}));
		}
#endif
		return _map.find(key) != _map.end();
	}

	size_t getNumberOfItems() const {
#ifndef NDEBUG
		std::lock_guard<std::mutex> lk(_mutex);
		assert(_cache.unsafeSize() == _map.unsafe_size() &&
			_map.unsafe_size() == _sorted.unsafe_size());
#endif
		return _cache.unsafeSize();
	}

	bool remove(const K&key) {
		auto it = _map.find(key);
		if (it != _map.end()) {
			return false;
		}

		NodePtr node = it->second;
		
		// atomic synchronization of containers
		std::lock_guard<std::mutex> lk(_mutex);

		_map.unsafe_erase(key);
		assert(!_map.contains(key));

		_sorted.unsafe_erase({key, node});
		assert(!_sorted.contains({key, it->second}));

		if (!_cache.remove(node)) {
			throw std::runtime_error(std::format(
				"Cache failed to pop (key, value): ({}, {})",
				key, node->value));
		}
		assert(!_cache.contains(it->second));
		assert(!node);

		assert(_cache.unsafeSize() == _map.unsafe_size() &&
			_map.unsafe_size() == _sorted.unsafe_size());

		return true;
	}

	void clear() {
		// atomic synchronization of containers
		std::lock_guard<std::mutex> lk(_mutex);

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

		std::optional<ListEntry> opt = _cache.back();
		if (!opt) {
			throw std::runtime_error(
				"Cache expected to evict, but nothing to evict");
		}
		ListEntry lentry = *opt;

		auto it = _cache.find(lentry.key);
		if (it == _cache.end()) {
			throw std::runtime_error(std::format(
				"Cache attempted to evict key {}, but not found in map",
				lentry.key);
		}
		auto node = it->second;

		_sorted.unsafe_erase({lentry.key, node});
		_map.unsafe_erase(lentry.key);
		_cache.popBack();
	}
};
