#include <cstddef>
#include <cassert>

template <typename K, typename V>
using NodePtr = typename CacheManager<K, V>::NodePtr;

template <typename K, typename V>
using CachePtr = typename CacheManager<K, V>::CachePtr;

template <typename K, typename V>
using MapPtr = typename CacheManager<K, V>::MapPtr;

template <typename K, typename V>
bool CacheManager<K, V>::isEmpty() const
{
	return _size == 0 && _cache->empty() && _map->isEmpty();
}

template <typename K, typename V>
bool CacheManager<K, V>::contains(const K& key) const
{
	auto v = _map->getItem(key);
	return v.has_value() && v.value() != nullptr;
}

template <typename K, typename V>
std::optional<V> CacheManager<K, V>::getItem(const K& key)
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

template <typename K, typename V>
std::size_t CacheManager<K, V>::getNumberOfItems() const
{
	assert(_size == _cache->size());
	return _size;
}

template <typename K, typename V>
bool CacheManager<K, V>::add(const K& key, const V& value)
{
	NodePtr ptr = _cache->pushFront(value);
	_map->add(key, ptr);
	++_size;
	if (_cache->size() >= _capacity) {
		evict();
	}
	return true;
}

template <typename K, typename V>
bool CacheManager<K, V>::remove(const K& key)
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

template <typename K, typename V>
void CacheManager<K, V>::clear()
{
	_cache = CACHE_ALLOC();
	_map = MAP_ALLOC();
	assert(_cache != nullptr);
	assert(_map != nullptr);

	_size = 0;
	assert(_size == _cache->size());
	assert(_size == _map->getNumberOfItems());
}

template <typename K, typename V>
csc::HashMap<K, typename CacheManager<K, V>::NodePtr> *CacheManager<K, V>::getTable()
{
	return _map.get();
}

template <typename K, typename V>
csc::DoublyLinkedList<V> *CacheManager<K, V>::getFifoList()
{
	return _cache.get();
}

template <typename K, typename V>
void CacheManager<K, V>::evict()
{
	_cache->popBack();
	--_size;
}

template <typename K, typename V>
void CacheManager<K, V>::update(NodePtr ptr)
{
	_cache->removeAndPushFront(ptr);
}
