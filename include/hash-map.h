/**
 * @file hash-map.h
 * @class HashMap
 *
 * @author Tyler Baxter
 * @version 1.0
 * @since 2024-08-30
 *
 * HashMap and HashMap helpers.
 */

#pragma once

#include "singly-linked-list.h"

#include <cmath>
#include <cstddef>
#include <iterator>
#include <string>
#include <memory>
#include <iostream>
#include <optional>

/**
* @namespace csc
* Namespace for CacheManager-specific packages.
*/
namespace csc {

/**
* Generic Hash function.
*/
template <typename K>
struct Hash {
	std::size_t operator()(const K& key) const;
};

/**
* C-String Hash function.
*/
template <>
struct Hash<unsigned char *> {
	std::size_t operator()(unsigned char *str) const;
};

/**
* C++ string Hash function.
*/
template <>
struct Hash<std::string> {
	std::size_t operator()(const std::string& str) const;
};

// Forward declaration for overloaded operators with template class.
template <typename K, typename V>
class HashNode;
template <typename K, typename V>
constexpr bool operator==(const HashNode<K, V>&, const HashNode<K, V>&);
template <typename K, typename V>
constexpr bool operator!=(const HashNode<K, V>&, const HashNode<K, V>&);

/**
 * @class HashNode
 * HashNode is a key-value pair for HashMap.
 */
template <typename K, typename V>
class HashNode {
public:
	HashNode(const K& key) : _key(key) {}
	HashNode(const K& key, const V& value) : _key(key), _value(value) {}

	friend constexpr bool operator== <>(const HashNode<K, V>& lhs, const
		HashNode<K, V>& rhs);
	friend constexpr bool operator!= <>(const HashNode<K, V>& lhs, const
		HashNode<K, V>& rhs);

	bool operator>(const HashNode<K, V>& src);

	const K& getKey() const { return _key; }
	const V& getItem() const { return _value; }
	void setItem(const V& value) { _value = value; }
protected:
    // Disallow copy and assignment.
    //HashNode(const HashNode& other);
    //HashNode& operator=(const HashNode& other);
private:
	//const std::size_t _hash;
	const K _key;
	V _value;
};

// Forward declaration of HashMap for reliant template declarations.
template <typename K, typename V, typename F>
class HashMap;

// Forward declaration of template overloaded insertion operator.
template <typename K, typename V, typename F>
std::ostream& operator<<(std::ostream&, const HashMap<K, V, F>&);

/* 
 * @enum MapIteratorType
 * MapIterator can have multiple states that need to be handled differently.
 * Simple wrapper for state deduction of MapIterator.
 */
enum class MapIteratorType : bool {
	EmptyBucket = false,
	FullBucket = true
};

/**
 * @class MapIterator<V>
 * HashMap Iterator.
 */
template <typename K, typename V, typename F>
class MapIterator {
public:
	using iterator_category = std::forward_iterator_tag;
	using value_type = V;
	using difference_type = std::ptrdiff_t;
	using pointer = V*;
	using reference = V&;
	using const_reference = const V&;

    explicit MapIterator(typename HashMap<K, V, F>::ListPtr *table,
						 std::size_t buckets, std::size_t index) :
		_table(table), _buckets(buckets), _index(index), _listIt(advance()) {}

	std::optional<V> operator*();
	pointer operator->();
    MapIterator& operator++();
	MapIterator operator++(int);
	bool operator==(const MapIterator& other) const;
    bool operator!=(const MapIterator& other) const;

	MapIteratorType getType() const;
	std::size_t getIndex() const;
private:
	/**
	 * Advances index until next valid bucket (has a list), and returns an
	 * iterator to the list.
	 */
	SLLIterator<HashNode<K, V>> advance();

	void setType();

	typename HashMap<K, V, F>::ListPtr *_table;	// reference to the hash table
	std::size_t _buckets;
	std::size_t _index;
	SLLIterator<HashNode<K, V>> _listIt;
	MapIteratorType _type;
};

/**
* @class HashMap
* Chained HashMap.
*/
template <typename K, typename V, typename F = Hash<K>>
class HashMap {
public:
	/**
	 * @typedef std::unique_ptr<SinglyLinkedList<HashNode<K, V>>> ListPtr
	 * ListPtr is a pointer to a SinglyLinkedList of HashNodes. HashMap has
	 * exclusive ownership of any ListPtrs.
	 */
	using ListPtr = std::unique_ptr<SinglyLinkedList<HashNode<K, V>>>;

	/**
	 * Default constructor.
	 */
	HashMap();

	/**
	 * Overloaded constructor for client-specified table buckets.
	 */
	HashMap(std::size_t buckets);

	/** 
	 * Destructor.
	 */
	~HashMap() { clear(); }

	/**
	 * Copy constructor.
	 */
	HashMap(const HashMap<K, V, F>& src);

	/*
	 * Move constructor.
	 */
	HashMap(HashMap<K, V, F>&& src) noexcept;

	/**
	 * Assignment operator.
	 */
	HashMap<K, V, F>& operator=(const HashMap<K, V, F>& rhs);

	/**
	 * Move assignment operator.
	 */
	HashMap<K, V, F>& operator=(HashMap<K, V, F>&& rhs) noexcept;

	/**
	 * Friend declaration of MapIterator.
	 */	
	friend class MapIterator<K, V, F>;

	/**
	 * Overloaded ostream operator, '<<'.
	 */
	friend std::ostream& operator<< <>(std::ostream& out,
		const HashMap<K, V, F>& map);

	/**
	 * Associates the specified value with the specified key in this map.
	 *
	 * @param int key
	 * The key to be inserted.
	 * @param int value
	 * The value to be inserted.
	 */
	void add(const K& key, const V& value);	

	/**
	 * Removes the mapping for the specified key from this map if present.
	 *
	 * @param int key
	 * The key to remove the value.
	 *
	 * @return The value that was removed.
	 */
	bool remove(const K& key);

	/**
	 * Removes the entry for the specified key only if it is currently mapped to 
	 * the specified value.
	 *
	 * @param int key
	 * The key to remove.
	 * @param int value
	 * The value to remove.
	 *
	 * @return TRUE if the key/value was from the hash map. FALSE if the 
	 * key/value was not removed from the hash map.
	 */
	bool remove(const K& key, const V& value);

	/**
	 * Gets the value associated with the key.
	 *
	 * @param K key The key to get the value.
	 */
	std::optional<V> getItem(const K& key) const;

	/**
	 * Checks whether HashMap contains the key.
	 *
	 * @return TRUE if the hash map contains the key; FALSE if the hash map does 
	 * not contain the key.
	 */
	bool contains(const K& key) const;

	/*
	 * Replaces the value associated with the key.
	 *
	 * @param K key The key to replace the mapped value.
	 * @param V value The new value.
	 */
	bool replace(const K& key, const V& value);

	//void traverse(void visit(V&)) const;

	/**
	* Returns the size of HashMap.
	*
	* @return std::size_t The size.
	*/
	std::size_t getNumberOfItems() const;

	std::size_t capacity() const;

	/**
	* Check whether HashMap is empty or not.
	*
	* @return TRUE if empty; FALSE if not empty.
	*/
	bool isEmpty() const;

	/**
	 * xxx
	 */
	MapIterator<K, V, F> begin() const;

	/**
	 * xxx
	 */
	MapIterator<K, V, F> end() const;

	/**
	 * Clears the contents and deallocates memory of HashMap.
	 */
	void clear();
private:
	const std::size_t TABLE_BUCKETS = 16;	// Power of two for DJR % 2^k.

	std::size_t _buckets;		
	ListPtr *_table;
	std::size_t _size;
	F _hash;
};
}
#include "hash-map.cpp"
