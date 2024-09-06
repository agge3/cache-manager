/**
 * @file hash-map.h
 * @class HashMap
 *
 * @author Tyler Baxter
 * @version 1.0
 * @since 2024-08-30
 *
 * Hash map.
 */

#pragma once

/**
* @namespace csc
* csc is the project namespace, for project-specific implementations.
*/
namespace csc {

/**
* @class HashMap
* A hash map.
*/
class HashMap {
public:
	/**
	 * Default constructor.
	 */
	HashMap() : _key(nullptr), _value(nullptr), _count(0) {}
	/** 
	 * Destructor.
	 */
	~HashMap();
	/**
	 * Assignment operator.
	 */
	HashMap& operator=(const HashMap& rhs);
	/**
	 * Copy constructor.
	 */
	HashMap(const HashMap& src);
	/*
	 * Move constructor.
	 */
	HashMap(HashMap&& src);
	/**
	 * Move assignment operator.
	 */
	HashMap& operator=(HashMap&& rhs);

	/**
	 * Associates the specified value with the specified key in this map.
	 *
	 * @param int key
	 * The key to be inserted.
	 * @param int value
	 * The value to be inserted.
	 */
	void insert(int key, int value);	

	/**
	 * Removes the mapping for the specified key from this map if present.
	 *
	 * @param int key
	 * The key to remove the value.
	 *
	 * @return The value that was removed.
	 */
	int remove(int key);

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
	bool remove(int key, int value);

	/**
	 * Prints the entire hash map.
	 */
	void print() const;

	/**
	 * Returns the value (a copy) to which the specified key is mapped, or xxx
	 *
	 * @param int key
	 * The key to get the value.
	 */
	int get(int key) const;

	/**
	 * Returns a pointer to the value (a reference) to which the specified key 
	 * is mapped, or xxx
	 *
	 * @param int key
	 * The key to get the value.
	 */
	std::unique_ptr<int> get(int key) const;

	/**
	 * Checks whether the hash map contains the key.
	 *
	 * @return TRUE if the hash map contains the key. FALSE if the hash map does 
	 * not contain the key.
	 */
	bool contains_key(int key) const;

	/**
	 * Checks whether the hash map contains the value.
	 *
	 * @return TRUE if the hash map contains the value. FALSE if the hash map does 
	 * not contain the value.
	 */
	bool contains_value(int value) const;

	/*
	 * Replaces the entry for the specified key only if it is currently mapped 
	 * to some value.
	 *
	 * @param int key
	 * The key to replace the mapped value.
	 * @param int value
	 * The new value.
	 */
	void replace(int key, int value);

	/**
	 * Replaces the entry for the specified key only if currently mapped to the 
	 * specified value.
	 *
	 * @param int key
	 * The key to replace the mapped value.
	 * @param int old_value
	 * The specified value to be replaced (only replaced if this is the old 
	 * value).
	 * @param int new_value
	 * The new value.
	 */
	void replace(int key, int old_value, int new_value);

	/**
	 * Checks whether the hash map is empty.
	 *
	 * @return TRUE if the hash map is empty. FALSE if the hash map is not empty.
	 */
	bool empty() const;
 	
	/**
	 * Returns the number of elements in the hash map.
	 */
	int size() const;

	/**
	 * Clears the contents of the hash map.
	 */
	void clear();
	
	// Non-member functions:
	// operator==operator!=operator<operator<=operator>operator>=operator<=>	
private:
	/**
	 * Regenerates the hash table.
	 */
	void rehash();
	/**
	 * Returns function used to hash the keys.
	 */
	void hash_function() const;
	/**
	 * Returns the function used to compare keys for equality.
	 */
	void key_eq() const;

	size_t _count;
};
}
