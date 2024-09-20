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

#include "iterator.h"

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

	T get_element() const { return _element; }
	SLLNode* get_next() const { return _next; }

	void set_element(const T& element) { _element = element; }
	void set_next(SLLNode* next) { _next = next; }
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
	~SinglyLinkedList();

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
	friend std::ostream& operator<<(std::ostream& out, const SinglyLinkedList& sll);

 	/**
	 * Overloaded istream operator, '>>'.
	 */
	friend std::istream& operator>>(std::istream& in, SinglyLinkedList& sll);

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
	T* pop_front();

	/**
	 * Adds a new element at the beginning of SinglyLinkedList.
	 *
	 * @param T element The element to be added.
	 */
	void insert(const T& element);

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
	 * Finds an element and returns an Iterator to it, or nullptr if the element
	 * was not found.
	 *
	 * @param T element The element to find.
	 *
	 * @return STTIterator<T> iterator An iterator pointing to the element, or
	 * nullptr if not found.
	 */
	T* find(const T& element);

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
	* Searches for an element and returns the Node before it. The element's node
	* can be accessed by get_next().
	*/
	SLLNode<T>* search(const T& element);

	/**
	* Clears all SinglyLinkedList's Nodes and deallocates their memory.
	*/
	void clear();

	SLLNode<T>* _head;
	std::size_t _size;
};
}
#include "singly-linked-list.tpp"
