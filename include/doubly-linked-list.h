/**
 * @file doubly-linked-list.h
 * @class DoublyLinkedList
 *
 * @author Tyler Baxter
 * @version 1.0
 * @since 2024-08-30
 *
 * DoublyLinkedList and DoublyLinkedList helpers.
 */

#pragma once

#include <cstddef>
#include <iostream>

/**
* @namespace csc
* Namespace for CacheManager-specific packages.
*/
namespace csc {

/**
* @class DLLNode
* DoublyLinkedList Node.
*/
template <typename T>
class DLLNode {
public:
	Node(const T& element) : 
		_element(element), _next(nullptr), _prev(nullptr) {}
	Node(const T& element, DLLNode* next, DLLNode* prev) : 
		_element(element), _next(next), _prev(prev) {}
	~Node() {}

	T get_element() const { return _element; }
	DLLNode* get_next() const { return _next; }
	DLLNode* get_prev() const { return _prev; }

	void set_element(T element) { _element = element; }
	void set_next(Node* next) { _next = next; }
	void set_prev(Node* prev) { _prev = prev; }
private:
	T _element;
	DLLNode* _next;
	DLLNode* _prev;
};

template <typename T>
class DLLIterator : public csc::Iterator<T> {
public:
	DLLIterator& operator++();
	DLLIterator operator++(int);
	T& operator*() const;
	void add(const T& element);
	void delete();
protected:
	/**
	* new and delete are protected so heap allocation is disallowed. Must be 
	* allocated on the stack, for RAII.
	*/
	explicit DLLIterator(DLLNode<T> node) : _node(node) {}
private:
	DLLNode<T> *_node;
};

/**
* @class DoublyDoublyLinkedList
* DoublyLinkedList, specialized as a Queue to be used for keeping track of order 
* in LRU CacheManager.
*/
template <typename T>
class DoublyLinkedList {
public:
	/**
	 * Default constructor.
	 */
	DoublyLinkedList() : _head(nullptr), _tail(nullptr), _count(0) {}

	/** 
	 * Destructor.
	 */
	~DoublyLinkedList() { clear(); }

	/**
	 * Copy constructor.
	 */
	DoublyLinkedList(const DoublyLinkedList<T>& other);

	/*
	 * Move constructor.
	 */
	DoublyLinkedList(DoublyLinkedList<T>&& other) noexcept;

	/**
	 * Assignment operator.
	 */
	DoublyLinkedList<T>& operator=(const DoublyLinkedList<T>& rhs);

	/**
	 * Move assignment operator.
	 */
	DoublyLinkedList<T>& operator=(DoublyLinkedList<T>&& rhs) noexcept;

	friend class DLLIterator<T>;

	/**
	 * Overloaded ostream operator, '<<'.
	 */
	friend ostream& operator<<(ostream& out, const DoublyLinkedList& dll) const;

 	/**
	 * Overloaded istream operator, '>>'.
	 */
	friend istream& operator>>(istream& in, DoublyLinkedList& dll) const;

	/**
	 * Returns the first element of DoublyLinkedList.
	 *
	 * @return T element The first element.
	 */
	T* front() const;

	/**
	 * Returns the last element of DoublyLinkedList.
	 *
	 * @return T element The last element.
	 */
	T* back() const;

	/**
	 * Returns the first element of DoublyLinkedList and deletes it from the 
	 * list.
	 *
	 * @return T element The first element.
	 */
	T* pop_front();

	/**
	 * Adds a new node at the beginning of the list.
	 *
	 * @param int element
	 * The element to be inserted.
	 */
	void push_front(const T& element);

	/**
	 * Returns and removes the element at the back of the list. Throws an 
	 * exception if the list is empty.
	 */
	T* pop_back();

	/**
	 * Inserts an element after the DLLNode.
	 *
	 * @param T element The element to be inserted.
	 * @param DLLNode<T> node The node to insert after.
	 */
	void insert(const T& element, DLLNode<T>* node);	

	/**
	 * Searches for a node with a specific elementue and deletes it from the list.
	 *
	 * @param int element
	 * The element to be deleted.
	 *
	 * @return TRUE if deleted, FALSE if not deleted
	 */
	bool remove(const T& element);

	/**
	 * Checks if DoublyLinkedList contains an element.
	 *
	 * @param T element The element to check for.
	 *
	 * @return TRUE if the list contains the element; FALSE if the list does not
	 * contain the element.
	 */
	bool contains(const T& element) const;

	/**
	 * Finds an element and returns an Iterator to it, or nullptr if the element 
	 * was not found.
	 *
	 * @param T element The element to find.
	 *
	 * @return STTIterator<T> iterator An iterator pointing to the element, or 
	 * nullptr if not found.
	 */
	DLLIterator<T> find(const T& element);
 	
	/**
	* Returns the size of DoublyLinkedList.
	*
	* @return std::size_t The size.
	*/
	std::size_t size() const;

	/**
	* Check whether DoublyLinkedList is empty or not.
	*
	* @return TRUE if empty; FALSE if not empty.
	*/
	bool empty() const;

	/** 
	 * Returns an Iterator pointing to the beginning (first element) of 
	 * DoublyLinkedList.
	 *
	 * @return DLLIterator iterator An Iterator pointing to begin.
	 */
	DLLIterator<T> begin() const;

	/** 
	 * Returns an Iterator pointing to the end (last element) of 
	 * DoublyLinkedList.
	 *
	 * @return DLLIterator iterator An Iterator pointing to end.
	 */
	DLLIterator<T> end() const;
private:
	void copy_calling_list_empty(const DoublyLinkedList<T>& other);
	void copy_lists_same_length(const DoublyLinkedList<T>& other);
	void copy_calling_list_longer(const DoublyLinkedList<T>& other);
	void copy_calling_list_shorter(const DoublyLinkedList<T>& other);

	/**
	* Searches for an element and returns the Node before it. The element's node
	* can be accessed by get_next().
	*/
	DLLNode<T>* search(const T& element);

	/**
	* Clears all SinglyLinkedList's Nodes and deallocates their memory.
	*/
	void clear();

	DLLNode<T>* _head;
	DLLNode<T>* _tail;
	std::size_t _size;
};
}
#include "doubly-linked-list.tpp"
