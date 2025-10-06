#pragma once

#include "concurrent-list.hpp"

#include "tbb/concurrent_unbounded_queue.h"
#include "tbb/concurrent_unordered_map.h"
#include "tbb/concurrent_multiset.h"

#include <memory>
#include <optional>
#include <cassert>
#include <mutex>
#include <exception>

namespace cm {

using NodePtr = const DLLNode<V>*;
using CachePtr = 

static constexpr less(const Node& lhs, const Node& rhs) noexcept {
	return lhs.value < rhs.value;
}

template <typename K, typename V, typename Cmp = less>
class CacheManager {
	size_t _capacity;
	size_t _size;

	std::mutex _mutex;

	cm::concurrent_list<V> _cache;
	tbb::concurrent_unordered_map<K, NodePtr> _map;
	tbb::concurrent_multiset<NodePtr,
		std::function<bool(const NodePtr&, const NodePtr&)>> _sorted;

public:
	explicit CacheManager(size_t capacity, Cmp cmp = Cmp()) :
		_capacity(capacity),
		_sorted(cmp)
	{}

	std::optional<V> getItem(const K& key) {
		auto it = map.find(key);
		if (it == map.end()) {
			return std::nullopt;
		}

		NodePtr node = it->second;

		queue.push(node);

		return node->value;
	}

	bool add(const K& key, const V& value) {
		NodePtr node;

		auto it = map.find(key);
		if (it != map.end()) {
			// update
			node = it->second;
			{
				// atomic synchronization of containers
				std::lock_guard<std::mutex> lk(_mutex);
				node->value = value;
				queue.push(node);
			}
			return true;
		}

		node = std::make_shared<Node>(Node{key, value});
		{
			// atomic synchronization of containers
			std::lock_guard<std::mutex> lk(_mutex);
			map.insert({key, node});
			sorted.insert(node);
			queue.push(node);
			++_size;
		}

		if (_size >= _capacity) {
			evict();
		}

		return true;
	}

	size_t getNumberOfItems() const {
		return _size;
	}

	bool remove(const K&key) {
		auto it = _map.find(key);
		if (it != _map.end()) {
			assert(!_cache.try_pop(node));
			assert(!_sorted.contains(node));
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
}:
