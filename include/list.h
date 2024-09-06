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
// xxx template
class Node {
public:
	Node(int element) : _element(element), _next(nullptr) {}
	~Node() {}
	int get_element() const { return _data; }
	Node* get_next() const { return _next; }
	void set_element(int element) { _element = element; }
	void set_next(Node* next) { _next = next; }
private:
	int _element;
	Node* _next;
};

/**
* @class LinkedList
* A double-linked list.
*/
class List {
public:
	/**
	 * Default constructor.
	 */
	List() : _head(nullptr), _tail(nullptr), _count(0) {}
	/** 
	 * Destructor.
	 */
	~List();
	/**
	 * Assignment operator.
	 */
	List& operator=(const List& rhs);
	/**
	 * Copy constructor.
	 */
	List(const List& src);
	/*
	 * Move constructor.
	 */
	List(List&& src);
	/**
	 * Move assignment operator.
	 */
	List& operator=(List&& rhs);

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
	 * Removes the element at the front of the list. Throws an exception if the
	 * list is empty.
	 */
	void pop_front();

	/**
	 * Adds a new node at the end of the list.
	 *
	 * @param int element
	 * The element to be inserted.
	 */
	void push_back(int element);
	/**
	 * Removes the element at the back of the list. Throws an exception if the
	 * list is empty.
	 */
	void pop_back();

	/**
	 * Inserts an element at the specified index.
	 *
	 * @param int element
	 * The element to be inserted.
	 * @param int index
	 * The index to insert at (inserted after the index).
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
	 * Returns a node pointer to the element, if it's contained in the list.
	 *
	 * @param int element
	 * The element to get a node pointer to.
	 */
	std::unique_ptr<Node> get(int element) const;

	/**
	 * Checks whether the list contains the element.
	 *
	 * @return TRUE if the list contains the element. FALSE if the list does not
	 * contain the element.
	 */
	bool contains(int element) const;

	/**
	 * Returns a node pointer to the beginning (head) of the list.
	 */
	std::unique_ptr<Node> begin() const;
	/**
	 * Returns a node pointer to the end (tail) of the list.
	 */
	std::unique_ptr<Node> end() const;
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
	
	// xxx maybe
	// returns a reverse iterator to the beginning
	Node* rbegin() const; // xxx reverse iterator
	// returns a reverse iterator to the end/
	Node* rend() const; // xxx reverse iterator
	// Non-member functions:
	// operator==operator!=operator<operator<=operator>operator>=operator<=>	
private:
	void copy_calling_list_empty(const List& src);
	void copy_lists_same_length(const List& src);
	void copy_calling_list_longer(const List& src);
	void copy_calling_list_short(const List& src);

	Node* _head;
	Node* _tail;
	int _count;
};
}
