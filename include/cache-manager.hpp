#pragma once

#include "doubly-linked-list.hpp"
#include "hash-map.hpp"

#include <cstddef>
#include <memory>
#include <cassert>
#include <optional>

template <typename K, typename V>
class CacheManager {
public:
	using NodePtr = const DLLNode<V>*;

    using CachePtr = std::unique_ptr<csc::DoublyLinkedList<V>>;
	// NOTE: Macros always pollute global namespace!
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
     * @return The value associated with the key, or std::nullopt if not found.
     */
	std::optional<V> getItem(const K& key);

	/**
     * Inserts or updates the key-value pair in the cache.
     *
     * @param key The key to insert/update.
     * @param value The value to associate with the key.
     */
	bool add(const K& key, const V& value);

	bool isEmpty() const;
	bool contains(const K& key) const;
	std::size_t getNumberOfItems() const;
  	bool remove(const K& key);
	void clear();
	csc::HashMap<K, NodePtr> *getTable();
	csc::DoublyLinkedList<V> *getFifoList();
private:
	std::size_t _capacity;
	std::size_t _size;
	CachePtr _cache;
	MapPtr _map;

    /**
     * Removes the least recently used item from the cache.
     */
    void evict();
	
	void update(NodePtr ptr);
};
#include "impl/cache-manager-impl.hpp"
