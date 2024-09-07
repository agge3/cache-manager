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
    : _buckets(INITIAL_BUCKET_COUNT), _count(0) {
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

// Insert method
void HashMap::insert(int key, int value) {
    if (static_cast<float>(_count) / _buckets.size() > LOAD_FACTOR_THRESHOLD) {
        rehash();
    }

    size_t index = hash_function(key) % _buckets.size();
    for (auto& entry : _buckets[index]) {
        if (entry.first == key) {
            entry.second = value;
            return;
        }
    }
    _buckets[index].emplace_back(key, value);
    ++_count;
}

// Remove method
int HashMap::remove(int key) {
    size_t index = hash_function(key) % _buckets.size();
    for (auto it = _buckets[index].begin(); it != _buckets[index].end(); ++it) {
        if (it->first == key) {
            int value = it->second;
            _buckets[index].erase(it);
            --_count;
            return value;
        }
    }
    throw std::runtime_error("Key not found");
}

// Remove method (with value check)
bool HashMap::remove(int key, int value) {
	std::size_t index = hash_function(key) % _buckets.size();
    for (auto it = _buckets[index].begin(); it != _buckets[index].end(); ++it) {
        if (it->first == key && it->second == value) {
            _buckets[index].erase(it);
            --_count;
            return true;
        }
    }
    return false;
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
int HashMap::get(int key) const {
    size_t index = hash_function(key) % _buckets.size();
    for (const auto& entry : _buckets[index]) {
        if (entry.first == key) {
            return entry.second;
        }
    }
    throw std::runtime_error("Key not found");
}

const int* HashMap::find(int key) const {
    size_t index = hash_function(key) % _buckets.size();
    for (const auto& entry : _buckets[index]) {
        if (entry.first == key) {
			return *entry.second;
        }
    }
    return nullptr;
}

// Contains key method
bool HashMap::contains_key(int key) const {
	std::size_t index = hash_function(key) % _buckets.size();
    for (const auto& entry : _buckets[index]) {
        if (entry.first == key) {
            return true;
        }
    }
    return false;
}

// Contains value method
bool HashMap::contains_value(int value) const {
    for (const auto& bucket : _buckets) {
        for (const auto& entry : bucket) {
            if (entry.second == value) {
                return true;
            }
        }
    }
    return false;
}

// Replace method
bool HashMap::replace(int key, int value) {
	std::size_t index = hash_function(key) % _buckets.size();
    for (auto& entry : _buckets[index]) {
        if (entry.first == key) {
            entry.second = value;
            return true;
        }
    }
	return false;
}

// Replace method (with old value check)
bool HashMap::replace(int key, int old_value, int new_value) {
	bool found = false;
	std::size_t index = hash_function(key) % _buckets.size();
    for (auto& entry : _buckets[index]) {
        if (entry.first == key && entry.second == old_value) {
            entry.second = new_value;
			return true;
        }
    }
	return false;
}

// Empty method
bool HashMap::empty() const {
    return _count == 0;
}

// Size method
std::size_t HashMap::size() const {
    return _count;
}

// Clear method
void HashMap::clear() {
    for (auto& bucket : _buckets) {
        bucket.clear();
    }
    _count = 0;
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

} // namespace csc
