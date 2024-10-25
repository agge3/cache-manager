/**
 * @file singly-linked-list.h
 * @class SinglyLinkedList<T>
 *
 * @author Kat Powell
 * @version 1.0
 * @since 2024-09-19
 *
 * SinglyLinkedList and SinglyLinkedList helpers.
 */

#pragma once

#include <cstddef>
#include <iterator>
#include <iostream>
#include <optional>

/**
* @namespace csc
* Namespace for CSC-specific packages.
*/
namespace csc {

/**
* @class SLLNode
* SinglyLinkedList Node.
*/
template <typename T>
class SLLNode {
public:
	SLLNode(const T& element) : _element(element), _next(nullptr) {}
	SLLNode(const T& element, SLLNode* next) : _element(element), _next(next) {}

	const T& getElement() const { return _element; }
	SLLNode* getNext() const { return _next; }

	void setElement(const T& element) { _element = element; }
	void setNext(SLLNode* next) { _next = next; }
private:
	T _element;
	SLLNode* _next;
};

/**
 * @class SLLIterator<T>
 * SinglyLinkedList Iterator.
 */
template <typename T>
class SLLIterator {
public:
	using iterator_category = std::forward_iterator_tag;
	using value_type = T;
	using difference_type = std::ptrdiff_t;
	using pointer = T*;
	using reference = T&;
	using const_reference = const T&;

    explicit SLLIterator(SLLNode<T>* node) : _node(node) {}

    const_reference operator*();
	pointer operator->();
    SLLIterator& operator++();
	SLLIterator operator++(int);
	bool operator==(const SLLIterator& other) const;
    bool operator!=(const SLLIterator& other) const;
private:
    SLLNode<T>* _node;
};

// Forward declaration for overloaded insertion operator with template class.
template <typename T>
class SinglyLinkedList;
template <typename T>
std::ostream& operator<<(std::ostream&, const SinglyLinkedList<T>&);
template <typename T>
constexpr bool operator==(const SinglyLinkedList<T>&,
						  const SinglyLinkedList<T>&);
template <typename T>
constexpr bool operator!=(const SinglyLinkedList<T>&,
						  const SinglyLinkedList<T>&);

/**
* @class SinglyLinkedList
* SinglyLinkedList.
*/
template <typename T>
class SinglyLinkedList {
public:
	/**
	 * Default constructor.
	 */
	SinglyLinkedList() : _head(nullptr), _size(0) {}

	/**
	 * Destructor.
	 */
	~SinglyLinkedList() { clear(); }

	/**
	 * Copy constructor.
	 */
	SinglyLinkedList(const SinglyLinkedList<T>& other);

	/*
	 * Move constructor.
	 */
	SinglyLinkedList(SinglyLinkedList<T>&& other) noexcept;

	/**
	 * Assignment operator.
	 */
	SinglyLinkedList<T>& operator=(const SinglyLinkedList<T>& rhs);

	/**
	 * Move assignment operator.
	 */
	SinglyLinkedList<T>& operator=(SinglyLinkedList<T>&& rhs) noexcept;

	/**
	 * Friend declaration of SLLIterator.
	 */	
	friend class SLLIterator<T>;

	/**
	 * Overloaded insertion operator<<.
	 */
	friend std::ostream& operator<< <>(std::ostream& out,
		const SinglyLinkedList<T>& list);
	
	/**
	 * Equality operator.
	 */
	friend constexpr bool operator== <>(const SinglyLinkedList<T>& lhs,
		const SinglyLinkedList<T>& rhs);

	/**
	 * Inequality operator.
	 */
	friend constexpr bool operator!= <>(const SinglyLinkedList<T>& lhs,
		const SinglyLinkedList<T>& rhs);

	/**
	 * Returns the first element of SinglyLinkedList.
	 *
	 * @return T element The first element.
	 */
	std::optional<T> front() const;

	/**
	 * Returns a ptr to the first node of SinglyLinkedList.
	 *
	 * @return T* the head of the list.
	 */
	SLLNode<T>* front_ptr() const;

	/**
	 * Returns the first element of SinglyLinkedList and deletes it from the
	 * list.
	 *
	 * @return T element The first element.
	 */
	std::optional<T> popFront();

	/**
	 * Adds a new node at the beginning of the list.
	 *
	 * @param T element The element to be inserted.
	 */
	void pushFront(const T& element);

	/**
	 * Adds a new node at the end of the list.
	 *
	 * @param T element The element to be inserted.
	 */
	void pushBack(const T& element);

	/**
	 * Prints the list.
	 */
	void print() const;

	/**
	 * Adds a new element at the beginning of SinglyLinkedList.
	 *
	 * @param T element The element to be added.
	 * @param Node, the node the element will get inserted after
	 */
	void insert(const T& element, SLLNode<T>* node);

	/**
	 * Searches for a node with the specified element and deletes it from the
	 * list.
	 *
	 * @param T element The element to be deleted.
	 *
	 * @return TRUE if deleted; FALSE if not deleted.
	 */
	bool remove(const T& element);

	/**
	 * Checks if SinglyLinkedList contains an element.
	 *
	 * @param T element The element to check for.
	 *
	 * @return TRUE if the list contains the element; FALSE if the list does not
	 * contain the element.
	 */
	bool contains(const T& element) const;

	/**
	* Returns the size of SinglyLinkedList.
	*
	* @return std::size_t The size.
	*/
	std::size_t size() const;

	/**
	* Check whether SinglyLinkedList is empty or not.
	*
	* @return TRUE if empty; FALSE if not empty.
	*/
	bool isEmpty() const;

	std::optional<T> find(const T& element) const;
	
	/** 
	 * Returns an Iterator pointing to the beginning (first element) of 
	 * SinglyLinkedList.
	 *
	 * @return SLLIterator iterator An Iterator pointing to begin.
	 */
    SLLIterator<T> begin() const { return SLLIterator<T>(_head); }

	/** 
	 * Returns an Iterator pointing to the end (last element) of 
	 * SinglyLinkedList.
	 *
	 * @return SLLIterator iterator An Iterator pointing to end.
	 */
 	SLLIterator<T> end() const { return SLLIterator<T>(nullptr); }

	/**
	* Clears all SinglyLinkedList's Nodes and deallocates their memory.
	*/
	void clear();
private:
	/**
	* Searches for an element and returns the Node before it. The element's node
	* can be accessed by getNext().
	*/
	SLLNode<T>* search(const T& element) const;

	SLLNode<T>* _head;
	std::size_t _size;
};
}
#include "singly-linked-list.cpp"
