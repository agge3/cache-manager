/**
 * @file cache-manager.cppm
 * @class CacheManager<K, V>
 *
 * @author agge3, kpowkitty
 * @version 1.0
 * @since 2025-11-04
 *
 * CacheManager API.
 */

#include "benchmark.hpp"
#include "macros.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <thread>
#include <unordered_map>
#include <vector>

#define NDEBUG 1

namespace cache {

/**
 * LRU Concurrent Cache Manager using TBB.
 */
template <typename K, typename V, typename BenchT = bench::NoneBench>
class CacheManager {
public:
	CacheManager(size_t shard_capacity = 1024);
	
	/**
	 * Gets an item from the cache.
	 *
	 * @param const K &key The requested key to retrieve from the cache.
	 *
	 * @return Either the requested key if found, or std::nullopt if not.
	 */
	std::optional<V> getItem(const K &key);

	/**
	 * Adds an item into the cache.
	 *
	 * @param const K &key The item's key.
	 * @param const V &value The item's value.
	 *
	 * @return True upon success.
	 */
	bool add(const K &key, const V &value);

	/**
	 * Checks whether a given key exists in the cache manager.
	 *
	 * @param const K &key The key to look for.
	 *
	 * @return True if found, false otherwise.
	 */
	bool contains(const K &key);

	/**
	 * Removes a given key from the cache manager.
	 *
	 * @param const K &key The key to remove.
	 *
	 * @return True if removed, false if not removed.
	 */
	bool remove(const K &key);

	/**
	 * Clears all values in the cache manager.
	 */
	void clear();

	static bench::Benchmark benchmark();
private:
	struct ThreadShard {
		size_t capacity;
		std::list<std::pair<K, V>> lru_list;
		std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator>
			map;

		ThreadShard(size_t cap) : capacity(cap) {}
	};

	/**
	 * The amount of shards for the cache manager.
	 */
	size_t _shard_capacity;
	
	/**
	 * Hashmap holding the shards of the cache manager.
	 *
	 * Each thread gets an assigned shard. Reduces serialization
	 * and increases parallelization.
	 */
	tbb::concurrent_unordered_map<std::thread::id, ThreadShard> _shards;

	/**
	 * Retrieves the shard of the current thread, or creates one for it
	 * if it does not exist already.
	 *
	 * @return The shard of the current thread.
	 */
	ThreadShard &getShard();
  };
} // namespace cache
