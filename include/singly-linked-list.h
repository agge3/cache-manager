/**
 * @file singly-linked-list.h
 * @class SinglyLinkedList
 *
 * @author Tyler Baxter, Kat Powell
 * @version 1.0
 * @since 2024-08-30
 *
 * SinglyLinkedList and SinglyLinkedList helpers.
 */

#pragma once

//#include "iterator.h"

#include <cstddef>
#include <iostream>

/**
* @namespace csc
* Namespace for CacheManager-specific packages.
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

	T getElement() const { return _element; }
	SLLNode* getNext() const { return _next; }

	void setElement(const T& element) { _element = element; }
	void setNext(SLLNode* next) { _next = next; }
private:
	T _element;
	SLLNode* _next;
};

//template <typename T>
//class SLLIterator : public Iterator<T> {
//public:
//	SLLIterator& operator++() override;
//	SLLIterator operator++(int) override;
//	T& operator*() override;
//	void add(const T& element) override;
//	void deleteIt() override; // XXX TEMP NAME
//protected:
//	/**
//	* new and delete are protected so heap allocation is disallowed. Must be
//	* allocated on the stack, for RAII.
//	*/
//	explicit SLLIterator(SLLNode<T> node) : _node(node) {}
//private:
//	SLLNode<T> *_node;
//};


// would we rather declare these inline in definition file, or have them
// pre-declared here and keep them in the header?
template <typename T> class SinglyLinkedList;

template <typename T>
std::ostream& operator<<(std::ostream& out, const SinglyLinkedList<T>& sll);

template <typename T>
std::istream& operator>>(std::istream& in, SinglyLinkedList<T>& sll);


/**
* @class SinglyLinkedList
* SinglyLinkedList, specialized to be used as buckets for HashMap.
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

	//friend class SLLIterator<T>;

	/**
	 * Overloaded ostream operator, '<<'.
	 */
	friend std::ostream& operator<< <>(std::ostream& out, const SinglyLinkedList& sll);

 	/**
	 * Overloaded istream operator, '>>'.
	 */
	friend std::istream& operator>> <>(std::istream& in, SinglyLinkedList& sll);

	/**
	 * Returns the first element of SinglyLinkedList.
	 *
	 * @return T element The first element.
	 */
	T front() const;

	/**
	 * Returns the first element of SinglyLinkedList and deletes it from the
	 * list.
	 *
	 * @return T element The first element.
	 */
	T popFront();

	/**
	 * Inserts an element at the beginning of SinglyLinkedList.
	 *
	 * @param T element The element to be inserted.
	 */
	void pushFront(const T& element);

	/**
	 * Inserts a new element after the node.
	 *
	 * @param T element The element to be inserted.
	 * @param SLLNode *node The node the element will be inserted after.
	 */
	void insert(const T& element, SLLNode<T> *node);

	/**
	 * Removes an element from SinglyLinkedList.
	 *
	 * @param T element The element to be removed.
	 *
	 * @return TRUE, the element was removed; FALSE, the element was not in the
	 * list.
	 */
	bool remove(const T& element);

	/**
	 * Removes a SLLNode from SinglyLinkedList.
	 *
	 * @param DLLNode<T> *node The node to be removed.
	 *
	 * @return TRUE, the node was removed; FALSE, the node was not in the list.
	 */
	bool remove(SLLNode<T> *node);

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
	 * Checks if SinglyLinkedList contains a SLLNode.
	 *
	 * @param T node The node to check for.
	 *
	 * @return TRUE, the list contains the node; FALSE, the list does not 
	 * contain the node.
	 */	
	bool contains(SLLNode<T> *node) const;

	/**
	 * Finds an element and returns an Iterator to it, or nullptr if the element
	 * was not found.
	 *
	 * @param T element The element to find.
	 *
	 * @return STTIterator<T> iterator An iterator pointing to the element, or
	 * nullptr if not found.
	 */
	//T* find(const T& element);

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
	bool empty() const;

	/**
	 * Returns an Iterator pointing to the beginning (first element) of
	 * SinglyLinkedList.
	 *
	 * @return SLLIterator iterator An Iterator pointing to begin.
	 */
	//SLLIterator<T> begin() const;

	/**
	 * Returns an Iterator pointing PAST the end (nullptr) of SinglyLinkedList.
	 *
	 * @return SLLIterator iterator An Iterator pointing to end.
	 */
	//SLLIterator<T> end() const;
private:
	/**
	* Searches for an element and returns the SLLNode before it. The element's 
	* node can be accessed by getNext().
	*/
	SLLNode<T>* search(const T& element) const;

	/**
	* Clears all SinglyLinkedList's SLLNodes and deallocates their memory.
	*/
	void clear();

	SLLNode<T>* _head;
	std::size_t _size;
};
}
#include "singly-linked-list.cpp"
