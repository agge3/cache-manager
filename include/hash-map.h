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

#include <cstddef>
#include <string>
#include <memory>

/**
* @namespace csc
* Namespace for CacheManager-specific packages.
*/
namespace csc {

/**
* C-String Hash function.
*/
template <>
struct Hash<unsigned char*> {
	std::size_t operator()(unsigned char *str) const;
};

/**
* C++ string Hash function.
*/
template <>
struct Hash<std::string> {
	std::size_t operator()(const std::string& str) const;
};

/**
* Generic Hash function.
*/
template <typename K>
struct Hash<K> {
	std::size_t operator()(const K& key) const;
};

/**
 * @class HashNode
 * HashNode is a key-value pair for HashMap.
 */
template <typename K, typename V>
class HashNode {
public:
	HashNode(const K& key) : _key(key), _value(nullptr) {}
	HashNode(const K& key, const V& value) : _key(key), _value(value) {}
	K getKey() const { return _key; }
	V getValue() const { return _value; }
	void setValue(const V& value) { _value = value; }
protected:
    // Disallow copy and assignment.
    HashNode(const HashNode& other);
    HashNode& operator=(const HashNode& other);
private:
	const std::size_t _hash;
	const K _key;
	V _value;
};

/**
* @class HashMap
* Chained HashMap.
*/
template <typename K, typename V, typename F = Hash>
class HashMap {
public:
    /**
     * @typedef std::unique_ptr<SinglyLinkedList<HashNode<K, V>>> ListPtr
	 * ListPtr is a pointer to a SinglyLinkedList of HashNodes. HashMap has
	 * exclusive ownership of any ListPtrs.
     */
    typedef std::unique_ptr<SinglyLinkedList<HashNode<K, V>>> ListPtr;

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
	HashMap(const HashMap& src);

	/*
	 * Move constructor.
	 */
	HashMap(HashMap&& src);

	/**
	 * Assignment operator.
	 */
	HashMap& operator=(const HashMap& rhs);

	/**
	 * Move assignment operator.
	 */
	HashMap& operator=(HashMap&& rhs);

	/**
	 * Overloaded ostream operator, '<<'.
	 */
	friend ostream& operator<<(ostream& out, const SinglyLinkedList& sll) const;

 	/**
	 * Overloaded istream operator, '>>'.
	 */
	friend istream& operator>>(istream& in, SinglyLinkedList& sll) const;

	/**
	 * Associates the specified value with the specified key in this map.
	 *
	 * @param int key
	 * The key to be inserted.
	 * @param int value
	 * The value to be inserted.
	 */
	void insert(const K& key, const V& value);	

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
	 * Gets the value (a copy) associated with the key.
	 *
	 * @param K key The key to get the value.
	 */
	V get(const K& key) const;

	/**
	 * Gets a pointer (a reference) to the value associated with the key.
	 *
	 * @param K key The key to get the value.
	 */
	V* get(const K& key) const;

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
	void replace(const K& key, const V& value);

	/**
	* Returns the size of HashMap.
	*
	* @return std::size_t The size.
	*/
	std::size_t size() const;

	/**
	* Check whether HashMap is empty or not.
	*
	* @return TRUE if empty; FALSE if not empty.
	*/
	bool empty() const;
private:
	/**
	 * Clears the contents and deallocates memory of HashMap.
	 */
	void clear();

	constexpr std::size_t TABLE_BUCKETS = 16;	// Power of two for DJR % 2^k.

	std::size_t _buckets;		
	ListPtr[] _table;
	std::size_t _size;
	F _hash;
};
}
#include "hash-map.cpp"
