/**
*
* Node.hpp : This is the header file for Node.
*
* 09/23/24 - Created by ChatGPT
* 09/23/24 - Modified by jhui
* 11/07/24 - Modified by jhui
*/
#ifndef _BASE_NODE
#define _BASE_NODE

/**
* Node
*
* This struct is the base node to be used in DoublyLinkedList.  There are Linked Lists in the HashTable's buckets needed for 
* collision resolution, as well a Linked List used in the FIFO cache.
*/
struct Node {
	int key;

	Node* prev;
	Node* next;

	Node(int keyValue);
	virtual ~Node();
};



#endif

