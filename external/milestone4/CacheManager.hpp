/**
*
* CacheManager.hpp : This is the header file for CacheManager.
*
* 09/23/24 - Created by ChatGPT
* 09/23/24 - Modified by jhui
* 11/07/24 - Modified by jhui
*/


#ifndef _CACHE_MANAGER
#define _CACHE_MANAGER

#include "FifoNode.hpp"
#include "DoublyLinkedList.hpp"
#include "HashTable.hpp"
#include <string>

class CacheManager  {
private:
	size_t _maxSize;
	size_t _curSize;

	DoublyLinkedList* _fifoCache;
	HashTable* _hashTable;

public:
	CacheManager(size_t capacity);

	// getTable
	HashTable* getTable();

	// getFifoList
	DoublyLinkedList* getFifoList();

	// getItem
	Node* getItem(int key);

	// getSize
	size_t getSize();

	// isEmpty
	bool isEmpty();

	// getNumberOfItems
	size_t getNumberOfItems();

	// add(searchKey, newItem)
	bool add(int, Data*);

	// remove(int)
	bool remove(int);

	// clear()
	void clear();

	// contains(int)
	bool contains(int);

};

#endif	// _CACHE_MANAGER

