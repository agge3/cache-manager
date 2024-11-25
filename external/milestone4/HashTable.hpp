/**
*
* HashTable.hpp : This is the header file for HashTable.
*
* 09/23/24 - Created by ChatGPT
* 10/17/24 - Modified by jhui
* 11/07/24 - Modified by jhui
*/

#ifndef _HASH_TABLE
#define _HASH_TABLE
#define _HASH_TABLE_SIZE 101

#include "HashNode.hpp"
#include <string>


/**
* HashTable
*
* This class is used for O(1) access into the FIFO cache.
*/
class HashTable {
private:
	// HashNode* table[_HASH_TABLE_SIZE];
	HashNode** table;
	int numberOfItems;

public:
	// Default constructor
	HashTable();

	/**
	*
	* getTable
	*
	* Method to return the hash table.  This is a sample implementation of this method to be copied to your HashTable.cpp.  
	* The method, getTable(), would generally not be in your implementation, but is needed here so that the contents can be 
	* printed out from main() to verify the contents of your hashTable.
	*
	* param: none
	*
	* returns: the hash table array
	*/
	HashNode** getTable();

	// getSize
	int getSize();

	// isEmpty
	bool isEmpty();

	// getNumberOfItems
	int getNumberOfItems();

	// add(searchKey, newItem)
	bool add(int, HashNode*);

	// remove(int)
	bool remove(int);

	// clear()
	void clear();

	// getItem(int)
	HashNode* getItem(int);

	// contains(int)
	bool contains(int);
}; // end HashTable



#endif 