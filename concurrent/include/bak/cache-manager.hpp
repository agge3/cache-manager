#pragma once

#include "concurrent-list.hpp"
#include "concurrent-map.hpp"

#include <memory>
#include <optional>
#include <cassert>
#include <mutex>
#include <exception>

namespace cm {


static constexpr less(const Node& lhs, const Node& rhs) noexcept {
	return lhs.value < rhs.value;
}

template <typename K, typename V, typename Cmp = less>
class ICacheManager {
private:
	using NodePtr = const DLLNode<V>*;
	
	size_t _capacity;
	size_t _size;

	std::mutex _mutex;

	IConcurrentList<V> _cache;
	IConcurrentHashMap<K, NodePtr> _map;
	IConcurrentBst<K, Cmp> _sorted;
public:
	explicit CacheManager(size_t capacity, Cmp cmp = Cmp()) :
		_capacity(capacity),
		_sorted(cmp)
	{
	}

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

	size_t getNumberOfItems() const {
		return _cache.unsafeSize();
	}

	bool remove(const K&key) {
		auto it = _map.find(key);
		if (it != _map.end()) {
			return false;
		}

		NodePtr node = it->second;
		{
			// atomic synchronization of containers
			std::lock_guard<std::mutex> lk(_mutex);

			if (!_cache.try_pop(node)) {
				throw std::runtime_error(std::format(
					"Cache failed to pop (key, value): ({}, {})",
					key, node->value));
			}

			_map.unsafe_erase(key);
			_sorted.unsafe_erase(node);
			--_size;
		}
		assert(!node);

		return true;
	}

private:
	void evict() {
	}
};

template <typename K, typename V, typename Cmp = less>
class CoarseCacheManager {
private:
};

template <typename K, typename V, typename Cmp = less>
class FineCacheManager {
private:
};
