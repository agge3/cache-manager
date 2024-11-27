#pragma once

#include "doubly-linked-list.hpp"
#include "hash-map.hpp"

#include <algorithm>
#include <cstddef>
#include <memory>
#include <cassert>

template <typename K, typename V>
class CacheManager {
public:
	using NodePtr = const DLLNode<V>*;

    using CachePtr = std::unique_ptr<csc::DoublyLinkedList<V>>;
    #define CACHE_ALLOC(...) \
        std::make_unique<csc::DoublyLinkedList<V>>(__VA_ARGS__)

    using MapPtr = std::unique_ptr<csc::HashMap<K, NodePtr>>;
    #define MAP_ALLOC(...) \
        std::make_unique<csc::HashMap<K, NodePtr>>(__VA_ARGS__)

    CacheManager(std::size_t capacity) :
        _capacity(capacity), _size(0), 
        _cache(CACHE_ALLOC()),
        _map(MAP_ALLOC()) {}

    /**
     * Retrieves the value associated with the key, and updates its position.
     *
     * @param key The key to lookup.
     * @return The value associated with the key, or throws an exception if not found.
     */
    //V get(const K& key);

    /**
     * Inserts or updates the key-value pair in the cache.
     *
     * @param key The key to insert/update.
     * @param value The value to associate with the key.
     */
    //void put(const K& key, const V& value);

	bool isEmpty() const
	{
		return _size == 0 && _cache->empty() && _map->isEmpty();
	}
	bool contains(const K& key) const
	{
		auto v = _map->getItem(key);
		return v.has_value() && v.value() != nullptr;
	}

	std::optional<V> getItem(const K& key)
	{
		std::optional<NodePtr> v = _map->getItem(key);
		if (v.has_value()) {
			NodePtr ptr = v.value();
			if (ptr == nullptr) {
				// Prune HashMap, since the cache can be evicted and keys
				// remain.
				_map->remove(key);
				return std::nullopt;
			}
			update(ptr);
			return ptr->getElement();
		}
		return std::nullopt;
	}
	std::size_t getNumberOfItems() const
	{
		assert(_size == _cache->size());
		return _size;
	}
	bool add(const K& key, const V& value)
	{
		NodePtr ptr = _cache->pushFront(value);
		_map->add(key, ptr);
		++_size;
		if (_cache->size() >= _capacity) {
			evict();
		}
		return true;
	}
	bool remove(const K& key)
	{
		auto v = _map->getItem(key);
		if (!v.has_value()) {
			return false;
		}

		NodePtr ptr = v.value();
		if (ptr == nullptr) {
			_map->remove(key);
			return false;
		}

		// xxx workaround with the current API, should just have remove(node)
		_cache->removeAndPushFront(ptr);
		_cache->popFront();
		_map->remove(key);
		--_size;
		return true;
	}
	void clear()
	{
		_cache = CACHE_ALLOC();
		_map = MAP_ALLOC();
		assert(_cache != nullptr);
		assert(_map != nullptr);

		_size = 0;
		assert(_size == _cache->size());
		assert(_size == _map->getNumberOfItems());
	}

	csc::HashMap<K, NodePtr> *getTable()
	{
		return _map.get();
	}

	csc::DoublyLinkedList<V> *getFifoList()
	{
		return _cache.get();
	}
private:
	std::size_t _capacity;
	std::size_t _size;
	CachePtr _cache;
	MapPtr _map;

    /**
     * Removes the least recently used item from the cache.
     */
    void evict()
	{
		_cache->popBack();
		--_size;
	}
	
	void update(NodePtr ptr)
	{
		_cache->removeAndPushFront(ptr);
	}
};

/**
 * void set(const K& key, const V& value);
 * void add(const K& key, const V& value);
 * void replace(const K& key, const V& value);
 * V get(const K& key);
 * V delete(const K& key);
 */

/*
template <typename K, typename V>
CacheManager<K, V>::CacheManager(std::size_t capacity) :
	_capacity(capacity),
	_map(std::make_unique<csc::HashMap<K, V>()),
	_queue(std::make_unique<csc::DoublyLinkedList<K>()) 
{
	// do nothing
}

template <typename K, typename V>
V* CacheManager<K, V>::get(const K& key)
{
    if (!_map->contains(key)) {
		// xxx handle cache misses
		return nullptr;
    }
    // Get a pointer to the value.
    V *v = _map->get(key);
    // Move the accessed key to the front of the queue.
    _queue->remove(key);
    _queue->push_front(key);
    return v;
}

template <typename K, typename V>
void CacheManager<K, V>::insert(const K& key, const V& value)
{
    if (_map->contains(key)) {
        // Update existing value.
        _map->replace(key, value);
        // Move the key to the front.
        _queue->remove(key);
        _queue->push_front(key);
	} else {
        if (_map->size() >= _capacity) {
            evict();
        }
        // Insert new key-value pair.
        _map->insert(key, value);
        // Add the key to the front of the queue.
        _queue->push_front(key);
    }
}

template <typename K, typename V>
void CacheManager<K, V>::evict()
{
    if (!_queue->empty()) {
    	// Get the least recently used key (at the end of the queue).
    	K k = _queue->back();
    	// Remove it from the map and queue.
    	_map->remove(k);
    	_queue->pop_back();
	}
	// else, do nothing
}
*/
