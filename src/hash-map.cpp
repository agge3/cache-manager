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

template <typename K>
std::size_t Hash<K>::operator()(const K& key) const 
{
	return djb2(key);
}

template <>
std::size_t Hash<unsigned char *>::operator()(unsigned char *str) const
{
	return djb2(str);
}

template <>
std::size_t Hash<std::string>::operator()(const std::string& str) const
{
	return djb2(str);
}

template <typename K, typename V>
bool HashNode<K, V>::operator(

template <typename K, typename V, typename F>
HashMap<K, V, F>::HashMap() : 
	_buckets(TABLE_BUCKETS),
	_table(new ListPtr[TABLE_BUCKETS]),
	_size(0),
	_hash()
{
	// do nothing
}

template <typename K, typename V, typename F>
HashMap<K, V, F>::HashMap(std::size_t buckets) : 
	_buckets(buckets), 
	_table(new ListPtr[buckets]),
	_size(0),
	_hash()
{
	// do nothing
}

// Copy constructor
template <typename K, typename V, typename F>
HashMap<K, V, F>::HashMap(const HashMap<K, V, F>& src) 
    : _buckets(src._buckets), _size(src._size)
{
    // Copy elements
	// xxx
}

// Move constructor
template <typename K, typename V, typename F>
HashMap<K, V, F>::HashMap(HashMap<K, V, F>&& src) noexcept : 
	_buckets(src._buckets),
	_table(std::move(src._table)),
	_size(src._size)
{
    // xxx
}

// Copy assignment operator
template <typename K, typename V, typename F>
HashMap<K, V, F>& HashMap<K, V, F>::operator=(const HashMap<K, V, F>& rhs)
{
    if (this != &rhs) {
        clear();
        _buckets = rhs._buckets;
        _size = rhs._size;
        // Copy elements
		// xxx
    }
    return *this;
}

// Move assignment operator
template <typename K, typename V, typename F>
HashMap<K, V, F>& HashMap<K, V, F>::operator=(HashMap<K, V, F>&& rhs) noexcept
{
    if (this != &rhs) {
        clear();
		_buckets = rhs._buckets;
        _table = std::move(rhs._table);
    }
    return *this;
}

template <typename K, typename V, typename F>
void HashMap<K, V, F>::add(const K& key, const V& value)
{
	auto ptr = _table[_hash(key) % _buckets].get();
	if (ptr == nullptr) {
		ListPtr lptr = std::make_unique<SinglyLinkedList<HashNode<K, V>>>();
		lptr->pushFront(HashNode<K, V>(key, value));
	} else {
		ptr->pushFront(HashNode<K, V>(key, value));
	}
}

template <typename K, typename V, typename F>
bool HashMap<K, V, F>::remove(const K& key)
{
	if (isEmpty()) {
		return false;
	}
	auto ptr = _table[_hash(key) % _buckets].get();
	if (ptr == nullptr) {
		return false;
	}
	return ptr->remove(HashNode<K, V>{key});
}

template <typename K, typename V, typename F>
std::optional<V> HashMap<K, V, F>::getItem(const K& key) const
{
	if (isEmpty()) {
		return std::nullopt;
	}

	auto ptr = _table[_hash(key) % _buckets].get();
	if (ptr == nullptr) {
		return std::nullopt;
	}

	auto node = ptr->find(HashNode<K, V>{key});
	return node.has_value() ? std::optional<V>(node->getItem()) : std::nullopt;
}

template <typename K, typename V, typename F>
bool HashMap<K, V, F>::contains(const K& key) const
{
	if (isEmpty()) {
		return false;
	}
	auto ptr = _table[_hash(key) % _buckets].get();
	if (ptr == nullptr) {
		return false;
	}
	return ptr->contains(HashNode<K, V>{key});
}

template <typename K, typename V, typename F>
bool HashMap<K, V, F>::replace(const K& key, const V& value)
{
	if (isEmpty()) {
		return false;
	}

	auto ptr = _table[_hash(key) % _buckets].get();
	if (!ptr) {
		return false;
	}

	HashNode<K, V> *node = ptr->find(HashNode<K, V>{key});
	if (node == nullptr) {
		return false;
	}

	node->setValue(value);
	node == nullptr;
	return true;
}

template <typename K, typename V, typename F>
bool HashMap<K, V, F>::isEmpty() const
{
	return _table == nullptr && _size == 0;
}

template <typename K, typename V, typename F>
std::size_t HashMap<K, V, F>::getNumberOfItems() const
{
	return _size;
}

template <typename K, typename V, typename F>
void HashMap<K, V, F>::clear()
{
	if (!isEmpty()) {
		// When table is deleted, its smart ListPtrs will lose scope and call
		// their destructors.
		delete[] _table;
		_table = nullptr;
	}
	// else, do nothing
}
