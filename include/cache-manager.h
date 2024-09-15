#pragma once

#include <memory>

template <typename K, typename V>
class CacheManager {
public:
	/** 
	 * Return an instance of CacheManager.
	 */
	static CacheManager* instance();

    /**
     * Retrieves the value associated with the key, and updates its position.
     *
     * @param key The key to lookup.
     * @return The value associated with the key, or throws an exception if not found.
     */
    V get(const K& key);

    /**
     * Inserts or updates the key-value pair in the cache.
     *
     * @param key The key to insert/update.
     * @param value The value to associate with the key.
     */
    void put(const K& key, const V& value);

	/**
	 * void set(const K& key, const V& value);
	 * void add(const K& key, const V& value);
	 * void replace(const K& key, const V& value);
	 * V get(const K& key);
	 * V delete(const K& key);
protected:
    /**
     * CacheManager is a singleton. Constructor with a specified capacity.
     *
     * @param capacity The maximum number of items the cache can hold.
     */
	CacheManager(std::size_t capacity);
private:
	static CacheManager *_instance;
	std::size_t _capacity
	std::unique_ptr<csc::HashMap<K, V>> _cache;
	std::unique_ptr<csc::LinkedList<K>> _order;

    /**
     * Removes the least recently used item from the cache.
     */
    void evict();
};
