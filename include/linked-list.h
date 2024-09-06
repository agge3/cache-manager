/**
 * @file linked-list.h
 * @class LinkedList
 *
 * @author Tyler Baxter
 * @version 1.0
 * @since 2024-08-30
 *
 * Single-linked list.
 */

#pragma once

/**
* @namespace csc
* csc is the project namespace, for project-specific implementations.
*/
namespace csc {

/**
* @class Node
* Node(s) for the linked list.
*/
class Node {
public:
	Node(int data) : _data(data), _next(nullptr) {}
	int getData() const { return _data; }
	Node* getNext() const { return _next; }
	void setData(int data) { _data = data; }
	void setNext(Node* next) { _next = next; }
private:
	int _data;
	Node* _next;
};

/**
* @class LinkedList
* A single-linked list.
*/
class LinkedList {
public:
	LinkedList() : _head(nullptr) {}
	~LinkedList();

	/**
	 * Adds a new node at the beginning of the list.
	 *
	 * @param int val
	 * The value to be inserted.
	 */
	void insertAtBeginning(int val);

	/**
	 * Adds a new node at the end of the list.
	 *
	 * @param int val
	 * The value to be inserted.
	 */
	void insertAtEnd(int val);

	/**
	 * Searches for a node with a specific value and deletes it from the list.
	 *
	 * @param int val
	 * The value to be deleted.
	 *
	 * @return TRUE if deleted, FALSE if not deleted
	 */
	bool deleteNode(int val);

	/**
	 * Prints the entire Linked List
	 */
	void printList() const;

	/**
	 * Gets the data at the index.
	 *
	 * @param int idx
	 * The index to get data from.
	 */
	int get(int idx) const;
private:
	Node* _head;
};
}
