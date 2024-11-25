/**
*
* DoublyLinkedList.hpp : This is the header file for DoublyLinkedList.
*
* 09/23/24 - Created by ChatGPT
* 09/23/24 - Modified by jhui
* 11/07/24 - Modified by jhui
*/

#ifndef _DOUBLY_LINKED_LIST
#define _DOUBLY_LINKED_LIST

#include "Node.hpp"

/**
* DoublyLinkedList
* 
* This class is used by both the HashTable (for collision resolution) and FIFO cache
*/ 
class DoublyLinkedList {
private:
	Node* head;                                                         // Pointer to the head of the list
	Node* tail;                                                         // Pointer to the tail of the list


public:
	// Default constructor
	DoublyLinkedList();

	// insertAtHead
	void insertAtHead(Node* newNode);

	// insertAtTail
	void insertAtTail(Node* newNode);

	// findNode
	Node* findNode(int value);

	// deleteNode
	void deleteNode(Node* existingNode);

	// moveToHead
	void moveToHead(Node* existingNode);

	// moveToTail
	void moveToTail(Node* existingNode);

	// deleteHeadNode
	void deleteHeadNode();

	// deleteTailNode
	void deleteTailNode();

	// deleteList
	void deleteList();

	Node* getHead();

}; // end DoublyLinkedList
#endif	// _DOUBLY_LINKED_LIST