#include "hash-map.h"

#include <vector>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility> // for std::move

using namespace csc;

// Define the initial bucket count for the hash map
constexpr std::size_t INITIAL_BUCKET_COUNT = 16;
constexpr float LOAD_FACTOR_THRESHOLD = 0.75f;

// Helper function to compute the hash of a key
std::size_t hash_function(int key) {
    return std::hash<int>{}(key);
}

// HashMap constructor
HashMap::HashMap()
{
	_table = new HashNode<K, V>*[TABLE_SIZE]();
}

// HashMap destructor
HashMap::~HashMap() {
    clear();
}

// Copy constructor
HashMap::HashMap(const HashMap& src) 
    : _buckets(src._buckets), _count(src._count) {
    // Copy elements
    for (auto& bucket : _buckets) {
        for (auto& entry : bucket) {
            _buckets[bucket].emplace_back(entry);
        }
    }
}

// Move constructor
HashMap::HashMap(HashMap&& src) noexcept 
    : _buckets(std::move(src._buckets)), _count(src._count) {
    src._count = 0;
}

// Copy assignment operator
HashMap& HashMap::operator=(const HashMap& rhs) {
    if (this != &rhs) {
        clear();
        _buckets = rhs._buckets;
        _count = rhs._count;
        // Copy elements
        for (auto& bucket : _buckets) {
            for (auto& entry : bucket) {
                _buckets[bucket].emplace_back(entry);
            }
        }
    }
    return *this;
}

// Move assignment operator
HashMap& HashMap::operator=(HashMap&& rhs) noexcept {
    if (this != &rhs) {
        clear();
        _buckets = std::move(rhs._buckets);
        _count = rhs._count;
        rhs._count = 0;
    }
    return *this;
}

// Insert method.
// DONE.
void HashMap::insert(const K& key, const V& value)
{
	// First, make sure the table isn't over the load factor threshold, and 
	// resize if neccessary.
	if (static_cast<float>(_size) / _capacity > LOAD_FACTOR_THRESHOLD) {
		resize();
	}

	std::size_t hash_value = _hash(key);
	std::size_t attempt = 0;
	HashNode<K, V>* prev = nullptr;
	HashNode<K, V>* entry = _table[hash_value];

	while (entry != nullptr && entry->get_key() != key) {
		hash_value = _hash.double_hash(key, ++attempt);
	}

	if (entry != nullptr) {
		// If we already have an entry for this key, just replace the value.
		entry->set_value(value);
	} else {
		// We don't already have an entry for this key, create a new HashNode.
		entry = new HashNode<K, V>(key, value);
		if (prev == nullptr) {
			// Insert as first bucket.
			table[hash_value] = entry;
		} else {
			prev->set_next(entry);
		}
	}
}

// Remove method
V HashMap::remove(const K& key)
{
	std::size_t hash_value = _func(key);
	HashNode<K, V>* prev = nullptr;
	HashNode<K, V>* entry = _table[hash_value];

	while (entry != nullptr && entry->get_key() != key) {
		prev = entry;
		entry = entry->get_next();
	}

	if (entry == nullptr) {
    	throw std::runtime_error("Key not found.");
	} else {
		if (prev == nullptr) {
			// Remove first bucket of the list.
			_table[hash_value] = entry->get_next();
		} else {
			prev->set_next(entry->get_next());
		}
		V ele = entry->get_value();
		delete entry;
		entry == nullptr;
		return ele;
	}
}

// Remove method (with value check)
V HashMap::remove(const K& key, const V& value) {
	std::size_t hash_value = _func(key);
	HashNode<K, V>* prev = nullptr;
	HashNode<K, V>* entry = _table[hash_value];

	while (entry != nullptr && entry->get_key() != key && 
		entry->get_value() != value) {
		prev = entry;
		entry = entry->get_next();
	}

	if (entry == nullptr) {
    	throw std::runtime_error("Key/value not found.");
	} else {
		if (prev == nullptr) {
			// Remove first bucket of the list.
			_table[hash_value] = entry->get_next();
		} else {
			prev->set_next(entry->get_next());
		}
		V ele = entry->get_value();
		delete entry;
		entry == nullptr;
		return ele;
	}
}

// Print method
void HashMap::print() const {
    for (const auto& bucket : _buckets) {
        for (const auto& entry : bucket) {
            std::cout << "[" << entry.first << ": " << entry.second << "] ";
        }
    }
    std::cout << std::endl;
}

// Get method (copy)
V HashMap::get(K key) const
{
	std::size_t hash_value = _hash(key);
	HashNode<K, V>* entry = table[hash_value];
	while (entry != nullptr) {
		if (entry->get_key() == key) {
			return entry->get_value();
		}
		entry = entry->get_next();
	}
    throw std::runtime_error("Key not found");
}

const HashNode<K, V>* HashMap::find(const K& key) const
{
	// xxx
}

// Contains key method
bool HashMap::contains_key(K key) const
{
	std::size_t hash_value = _hash(key);
	HashNode<K, V>* entry = table[hash_value];
	while (entry != nullptr) {
		if (entry->get_key() == key) {
			return true;
		}
		entry = entry->get_next();
	}
	return false;
}

// Contains value method
bool HashMap::contains_value(V value) const
{
	std::size_t hash_value = _hash(key);
	HashNode<K, V>* entry = table[hash_value];
	while (entry != nullptr) {
		if (entry->get_value() == value) {
			return true;
		}
		entry = entry->get_next();
	}
	return false;
}

// Replace method
bool HashMap::replace(K key, V value) {
	std::size_t hash_value = _hash(key);
	HashNode<K, V>* entry = table[hash_value];
	while (entry != nullptr) {
		if (entry->get_key() == key) {
			entry->set_value(value);
		}
		entry = entry->get_next();
	}
	return false;
}

// Replace method (with old value check)
bool HashMap::replace(K key, V old_value, V new_value) {
	std::size_t hash_value = _hash(key);
	HashNode<K, V>* entry = table[hash_value];
	while (entry != nullptr) {
		if (entry->get_key() == key && entry->get_value() == old_value) {
			entry->set_value(new_value);
			return true;
		}
		entry = entry->get_next();
	}
	return false;
}

// Empty method
bool HashMap::empty() const
{
	// xxx
}

// Size method
std::size_t HashMap::size() const
{
    // xxx
}

// Clear method
void HashMap::clear()
{
	// Destroy all buckets one by one.
	for (int i = 0; i < TABLE_SIZE; ++i) {
		HashNode<K, V>* entry = _table[i];
		while (entry != nullptr) {
			HashNode<K, V>* prev = entry;
			entry = entry->get_next();
			delete prev;
		}
		table[i] = nullptr;
	}
	// Destroy the hash table.
	delete[] _table;
}

// Rehash method
void HashMap::rehash() {
    std::vector<std::vector<std::pair<int, int>>> new_buckets(_buckets.size() * 2);

    for (const auto& bucket : _buckets) {
        for (const auto& entry : bucket) {
            size_t index = hash_function(entry.first) % new_buckets.size();
            new_buckets[index].emplace_back(entry);
        }
    }

    _buckets = std::move(new_buckets);
}

// Hash function (dummy implementation, could be removed if not needed)
void HashMap::hash_function() const {
    // This function is a placeholder and is not used in the current implementation
}

// Key equality function (dummy implementation, could be removed if not needed)
void HashMap::key_eq() const {
    // This function is a placeholder and is not used in the current implementation
}
