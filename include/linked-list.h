/**
 * @file linked-list.h
 * @class LinkedLinkedList
 *
 * @author Tyler Baxter
 * @version 1.0
 * @since 2024-08-30
 *
 * Double-linked list.
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
	Node(int element) : _element(element), _next(nullptr), _prev(nullptr) {}
	Node(int element, Node* next, Node* prev) : 
		_element(element), _next(next), _prev(prev) {}
	~Node() {}
	int get_element() const { return _element; }
	Node* get_next() const { return _next; }
	Node* get_prev() const { return _prev; }
	void set_element(int element) { _element = element; }
	void set_next(Node* next) { _next = next; }
	void set_prev(Node* prev) { _prev = prev; }
private:
	int _element;
	Node* _next;
	Node* _prev;
};

/**
* @class LinkedList
* A double-linked list.
*/
class LinkedList {
public:
	/**
	 * Default constructor.
	 */
	LinkedList() : _head(nullptr), _tail(nullptr), _count(0) {}
	/** 
	 * Destructor.
	 */
	~LinkedList();
	/**
	 * Assignment operator.
	 */
	LinkedList& operator=(const LinkedList& rhs);
	/**
	 * Copy constructor.
	 */
	LinkedList(const LinkedList& para);
	/*
	 * Move constructor.
	 */
	LinkedList(LinkedList&& para);
	/**
	 * Move assignment operator.
	 */
	LinkedList& operator=(LinkedList&& rhs);

	/**
	 * Returns the element at the front of the list. Throws an exception if the 
	 * list is empty.
	 */
	int front() const;

	/**
	 * Returns the element at the back of the list. Throws an exception if the
	 * list is empty.
	 */
	int back() const;

	/**
	 * Adds a new node at the beginning of the list.
	 *
	 * @param int element
	 * The element to be inserted.
	 */
	void push_front(int element);

	/**
	 * Returns and removes the element at the front of the list. Throws an 
	 * exception if the list is empty.
	 */
	int pop_front();

	/**
	 * Adds a new node at the end of the list.
	 *
	 * @param int element
	 * The element to be inserted.
	 */
	void push_back(int element);

	/**
	 * Returns and removes the element at the back of the list. Throws an 
	 * exception if the list is empty.
	 */
	int pop_back();

	/**
	 * Inserts an element at the specified index.
	 *
	 * @param int element
	 * The element to be inserted.
	 * @param int index
	 * The index to insert at (inserted before the index, so that the newly 
	 * inserted element is at the specified index).
	 */
	void insert(int element, int index);	

	/**
	 * Searches for a node with a specific elementue and deletes it from the list.
	 *
	 * @param int element
	 * The element to be deleted.
	 *
	 * @return TRUE if deleted, FALSE if not deleted
	 */
	bool remove(int element);

	/**
	 * Prints the entire list.
	 */
	void print() const;

	/**
	 * Returns the element at the specified index in this list.
	 *
	 * @param int index
	 * The index to get the element.
	 */
	int get(int index) const;

	/**
	 * Returns a read-only pointer to the node containing the element, if the 
	 * element's contained in the list.
	 *
	 * @param int element
	 * The element to get a pointer to its node.
	 *
	 * @return A read-only pointer to the node, or nullptr if the element is not
	 * contained in the list.
	 */
	const Node* find(int element) const;

	/**
	 * Checks whether the list contains the element.
	 *
	 * @return TRUE if the list contains the element. FALSE if the list does not
	 * contain the element.
	 */
	bool contains(int element) const;

	/**
	 * Returns a read-only pointer to the beginning node (head) of the list.
	 */
	const Node* begin() const;

	/**
	 * Returns a read-only pointer to the end node (tail) of the list.
	 */
	const Node* end() const;

	/**
	 * Checks whether the list is empty.
	 *
	 * @return TRUE if the list is empty. FALSE if the list is not empty.
	 */
	bool empty() const;
 	
	/**
	 * Returns the number of elements in the list.
	 */
	int size() const;

	/**
	 * Clears the contents of the list.
	 */
	void clear();
	
	// Non-member functions:
	// operator==operator!=operator<operator<=operator>operator>=operator<=>	
private:
	void copy_calling_list_empty(const LinkedList& para);
	void copy_lists_same_length(const LinkedList& para);
	void copy_calling_list_longer(const LinkedList& para);
	void copy_calling_list_shorter(const LinkedList& para);
	void index_out_of_range(int index) const;

	Node* _head;
	Node* _tail;
	int _count;
};
}
