template <typename K, typename V>
CacheManager<K, V>* CacheManager::_instance = 0;

template <typename K, typename V>
CacheManager<K, V>* CacheManager<K, V>::instance()
{
	if (_instance == 0) {
		_instance = new CacheManager;
	}
	return _instance;
}

template <typename K, typename V>
CacheManager<K, V>::CacheManager(std::size_t capacity) :
	_capacity(capacity),
	_map(std::make_unique<csc::HashMap<K, V>()),
	_order(std::make_unique<csc::LinkedList<K>()) 
{
	// do nothing
}

template <typename K, typename V>
V CacheManager<K, V>get(const K& key)
{
    if (!_map->contains_key(key)) {
        throw std::runtime_error("Key not found");
    }
    // Retrieve the value.
    V value = _map->get(key);
    // Move the accessed key to the front of the list.
    _order->remove(key);
    _order->push_front(key);
    return value;
}

template <typename K, typename V>
void CacheManager<K, V>put(const K& key, const V& value)
{
    if (_map->contains_key(key)) {
        // Update existing value
        _map->replace(key, value);
        // Move the key to the front
        _order->remove(key);
        _order->push_front(key);
    } else {
        if (_map->size() >= _capacity) {
            evict();
        }
        // Insert new key-value pair
        _map->insert(key, value);
        // Add the key to the front of the list
        _order->push_front(key);
    }
}

template <typename K, typename V>
void CacheManager<K, V>evict()
{
    if (_order->empty()) {
        throw std::runtime_error("Cache is empty");
    }
    // Get the least recently used key (at the end of the list)
    K key_to_evict = _order->back();
    // Remove it from the map and list
    _map->remove(key_to_evict);
    _order->pop_back();
}
