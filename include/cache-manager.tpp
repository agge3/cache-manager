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
	_queue(std::make_unique<csc::LinkedList<K>()) 
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
