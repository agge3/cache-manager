#include "hash-map.h"

#include <vector>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility> // for std::move

using namespace csc;

// Hash functions, to exist in this scope only.
namespace {
	std::size_t djb2(unsigned char *str)
    {
		std::size_t hash = 5381;
        int c;

        while (c = *str++)
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

        return hash;
    }	
	std::size_t djb2(const std::string& str)
	{
		std::size_t hash = 5381;
		for (char c : str) {
			hash = ((hash << 5) + hash) + static_cast<unsigned char>(c);
		}
		return hash;
	}
	template <typename T>
	std::size_t djb2(const T& key)
	{
		std::size_t hash = 5381;
		const unsigned char *ptr = 
			reinterpret_cast<const unsigned char *>(&key);
		for (std::size_t i = 0; i < sizeof(T); ++i) {
			hash = ((hash << 5) + hash) + ptr[i];
		}
		return hash;
	}
	std::size_t sdbm(unsigned char *str)
    {
		std::size_t hash = 0;
        int c;

        while (c = *str++)
            hash = c + (hash << 6) + (hash << 16) - hash;

        return hash;
    }
}

template <>
std::size_t Hash<unsigned char*>::operator()(unsigned char *str) const
{
	return djb2(str);
}

template <>
Hash<std::string>::std::size_t operator()(const std::string& str) const
{
	return djb2(str);
}

template <typename K>
std::size_t Hash<K>::operator()(const T& key) const 
{
	return djb2(key);
}

template <typename K, typename V, typename F = Hash>
HashMap<K, V, F>::HashMap() : 
	_buckets(TABLE_BUCKETS),
	_table(new SinglyLinkedList<V>[TABLE_BUCKETS]),
	_size(0),
	_hash()
{
	// do nothing
}

template <typename K, typename V, typename F = Hash>
HashMap<K, V, F>::HashMap(std::size_t buckets) : 
	_buckets(buckets), 
	_table(new SinglyLinkedList<V>[buckets]),
	_size(0),
	_hash()
{
	// do nothing
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

template <typename K, typename V, typename F = Hash>
bool HashMap<K, V, F>::insert(const K& key, const V& value) const
{
	ListPtr ptr = _table[_hash{}(key) % _buckets];
	// xxx 
}

template <typename K, typename V, typename F = Hash>
bool HashMap<K, V, F>::remove(const K& key) const
{
	ListPtr ptr = _table[_hash{}(key) % _buckets];
	// xxx 
}

template <typename K, typename V, typename F = Hash>
V* HashMap<K, V, F>::get(const K& key) const
{
	ListPtr ptr = _table[_hash{}(key) % _buckets];
	// xxx 
}

template <typename K, typename V, typename F = Hash>
bool HashMap<K, V, F>::contains(const K& key) const
{
	ListPtr ptr = _table[_hash{}(key) % _buckets];
	// xxx 
}

template <typename K, typename V, typename F = Hash>
bool HashMap<K, V, F>::replace(const K& key, const V& value)
{
	ListPtr ptr = _table[_hash{}(key) % _buckets];
	// xxx
}

template <typename K, typename V, typename F = Hash>
bool HashMap<K, V, F>::empty() const
{
	return _table == nullptr && _size == 0;
}

template <typename K, typename V, typename F = Hash>
std::size_t HashMap<K, V, F>::size() const
{
	return _size;
}

template <typename K, typename V, typename F = Hash>
void HashMap<K, V, F>::clear()
{
	if (!empty()) {
		// Destroy all buckets one by one.
		for (int i = 0; i < _buckets; ++i) {
			delete _table[i];
			_table[i] = nullptr;
		}
		// Destroy the hash table.
		delete[] _table;
		_table = nullptr;
	}
	// else, do nothing
}
