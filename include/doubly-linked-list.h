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
#include <iterator>
#include <optional>

/**
* @namespace csc
* Namespace for CacheManager-specific packages.
*/
namespace csc {

/**
* @class DLLNode<T>
* DoublyLinkedList Node.
*/
template <typename T>
class DLLNode {
public:
	DLLNode(const T& element) : 
		_element(element), _next(nullptr), _prev(nullptr) {}
	DLLNode(const T& element, DLLNode *next, DLLNode *prev) : 
		_element(element), _next(next), _prev(prev) {}
	~DLLNode() {}

	const T& getElement() const { return _element; }
	DLLNode* getNext() const { return _next; }
	DLLNode* getPrev() const { return _prev; }

	void setElement(const T& element) { _element = element; }
	void setNext(DLLNode *next) { _next = next; }
	void setPrev(DLLNode *prev) { _prev = prev; }
private:
	T _element;
	DLLNode *_next;
	DLLNode *_prev;
};

/**
 * @class DLLIterator<T>
 * DoublyLinkedList Iterator.
 */
template <typename T>
class DLLIterator {
public:
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = T;
	using difference_type = std::ptrdiff_t;
	using pointer = T*;
	using reference = T&;
	using const_reference = const T&;

	explicit DLLIterator(DLLNode<T> *node) : _node(node) {}

	const_reference operator*();
	pointer operator->();
	DLLIterator& operator++();
	DLLIterator operator++(int);
    DLLIterator& operator--();
    DLLIterator operator--(int);
    bool operator==(const DLLIterator& other) const;
    bool operator!=(const DLLIterator& other) const;
protected:
private:
	DLLNode<T> *_node;
};

/**
* @class DoublyLinkedList<T>
* DoublyLinkedList, specialized as a Queue to be used for keeping track of order 
* in LRU CacheManager.
*/
template <typename T>
class DoublyLinkedList {
public:
	/**
	 * Default constructor.
	 */
	DoublyLinkedList() : _head(nullptr), _tail(nullptr), _size(0) {}

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

	/**
	 * Friend declaration of DLLIterator.
	 */
	friend class DLLIterator<T>;

	/**
	 * Overloaded ostream operator, '<<'.
	 */
	//friend ostream& operator<<(ostream& out, const DoublyLinkedList& dll) const;

 	/**
	 * Overloaded istream operator, '>>'.
	 */
	//friend istream& operator>>(istream& in, DoublyLinkedList& dll) const;

	/**
	 * Returns the first element of DoublyLinkedList.
	 *
	 * @return T element The first element.
	 */
	std::optional<T> front() const;

	/**
	 * Returns the last element of DoublyLinkedList.
	 *
	 * @return T element The last element.
	 */
	std::optional<T> back() const;

	/**
	 * Returns the first element of DoublyLinkedList and removes it from the 
	 * list.
	 *
	 * @return T element The first element.
	 */
	std::optional<T> popFront();

	/**
	 * Returns the last element of DoublyLinkedList and removes it from the 
	 * list.
	 *
	 * @return T element The last element.
	 */
	std::optional<T> popBack();

	/**
	 * Inserts an element at the beginning of DoublyLinkedList.
	 *
	 * @param T element The element to be inserted.
	 */
	const DLLNode<T>* pushFront(const T& element);

	/**
	 * Inserts an element at the end of DoublyLinkedList.
	 *
	 * @param T element The element to be inserted.
	 */
	const DLLNode<T>* pushBack(const T& element);


	/**
	 * Inserts an element after the DLLNode.
	 *
	 * @param T element The element to be inserted.
	 * @param DLLNode<T> *node The node to insert after.
	 */
	//void insert(const T& element, DLLNode<T> *node);	

	/**
	 * Gets the element contained in the DLLNode.
	 *
	 * @param DLLNode<T> *node The node to get the element from.
	 *
	 * @return std::optional<T> element The element if the node wasn't nullptr, 
	 * or no element for a nullptr node.
	 */
	std::optional<T> get(const DLLNode<T> *ptr);
	
	const DLLNode<T>* get(const T& element);

	/**
	 * Removes an element from DoublyLinkedList.
	 *
	 * @param T element The element to be removed.
	 *
	 * @return TRUE, the element was removed; FALSE, the element was not in the
	 * list.
	 */
	bool remove(const T& element);

	/**
	 * Removes a DLLNode from DoublyLinkedList.
	 *
	 * @param DLLNode<T> *node The node to be removed.
	 *
	 * @return TRUE, the node was removed; FALSE, the node was not in the list.
	 */
	bool removeAndPushFront(const DLLNode<T> *ptr);

	/**
	 * Checks if DoublyLinkedList contains an element.
	 *
	 * @param T element The element to check for.
	 *
	 * @return TRUE, the list contains the element; FALSE, the list does not
	 * contain the element.
	 */
	bool contains(const T& element) const;

	/**
	 * Checks if DoublyLinkedList contains a DLLNode.
	 *
	 * @param T node The node to check for.
	 *
	 * @return TRUE, the list contains the node; FALSE, the list does not 
	 * contain the node.
	 */
	bool contains(const DLLNode<T> *ptr) const;

	/**
	 * Finds an element and returns an Iterator to it, or nullptr if the element 
	 * was not found.
	 *
	 * @param T element The element to find.
	 *
	 * @return STTIterator<T> iterator An iterator pointing to the element, or 
	 * nullptr if not found.
	 */
	//DLLIterator<T> find(const T& element);
 	
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
	* Clears all DoublyLinkedList's DLLNodes and deallocates their memory.
	*/
	void clear();

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
	void copyCallingListEmpty(const DoublyLinkedList<T>& other);
	void copyListsSameLength(const DoublyLinkedList<T>& other);
	void copyCallingListLonger(const DoublyLinkedList<T>& other);
	void copyCallingListShorter(const DoublyLinkedList<T>& other);

	/**
	* Searches for an element and returns the Node before it. The element's node
	* can be accessed by getNext().
	*/
	const DLLNode<T>* search(const T& element) const;

	DLLNode<T> *_head;
	DLLNode<T> *_tail;
	std::size_t _size;
};
}
#include "doubly-linked-list.cpp"
