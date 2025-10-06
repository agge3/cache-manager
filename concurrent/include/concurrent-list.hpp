/**
 * @file concurrent-list.hpp
 *
 * @author agge3
 * @version 1.0
 * @since 2024-09-21
 *
 * ConcurrentList and ConcurrentList helpers.
 */

#pragma once

#include <cstddef>
#include <iostream>
#include <iterator>
#include <optional>
#include <shared_mutex>

/**
* @namespace cm
* Namespace for CacheManager-specific packages.
*/
namespace cm {

/**
* @class ListNode<T>
* ConcurrentList Node.
*/
template <typename T>
struct ListNode {
	T _ele;
	ListNode *_next;
	ListNode *_prev;
	std::shared_mutex _mtx;

	ListNode(const T& ele) :
		_ele(ele), _next(nullptr), _prev(nullptr) {}
	ListNode(const T& ele, ListNode *next, ListNode *prev) :
		_ele(ele), _next(next), _prev(prev) {}
	~ListNode() {}
};

/**
 * @class ListIterator<T>
 * ConcurrentList Iterator.
 * 
 * ListIterators are invalidated on any (read|write) operation of
 * ConcurrentList. Thread safety is only guaranteed if the entire ConcurrentList
 * is locked before iteration.
 *
 */
template <typename T>
class ListIterator {
public:
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = T;
	using difference_type = std::ptrdiff_t;
	using pointer = T*;
	using reference = T&;
	using const_reference = const T&;

	explicit ListIterator(ListNode<T> *node) : _node(node) {}

	const_reference operator*();
	pointer operator->();
	ListIterator& operator++();
	ListIterator operator++(int);
    ListIterator& operator--();
    ListIterator operator--(int);
    bool operator==(const ListIterator& other) const;
    bool operator!=(const ListIterator& other) const;
protected:
private:
	ListNode<T> *_node;
};

// Forward declaration for overloaded insertion operator with template class.
template <typename T>
class ConcurrentList;
template <typename T>
std::ostream& operator<<(std::ostream&, const ConcurrentList<T>&);

/**
* @class ConcurrentList<T>
* ConcurrentList, specialized as a Queue to be used for keeping track of order
* in LRU CacheManager.
*/
template <typename T>
class ConcurrentList {
public:
	/**
	 * Default constructor.
	 */
	ConcurrentList() : _head(nullptr), _tail(nullptr), _size(0) {}

	/**
	 * Destructor.
	 */
	~ConcurrentList() { unsafeClear(); }

	/**
	 * Copy constructor.
	 */
	ConcurrentList(const ConcurrentList<T>& other);

	/*
	 * Move constructor.
	 */
	ConcurrentList(ConcurrentList<T>&& other) noexcept;

	/**
	 * Assignment operator.
	 */
	ConcurrentList<T>& operator=(const ConcurrentList<T>& rhs);

	/**
	 * Move assignment operator.
	 */
	ConcurrentList<T>& operator=(ConcurrentList<T>&& rhs) noexcept;

	/**
	 * FriunsafeEnd declaration of ListIterator.
	 */
	friend class ListIterator<T>;

	/**
	 * Overloaded insertion operator<<.
	 */
	friend std::ostream& operator<< <>(std::ostream& out,
		const ConcurrentList<T>& list);

	/**
	 * Returns the first element of ConcurrentList.
	 *
	 * @return T element The first element.
	 */
	std::optional<T> front() const;

	/**
	 * Returns the last element of ConcurrentList.
	 *
	 * @return T element The last element.
	 */
	std::optional<T> back() const;

	/**
	 * Returns the first element of ConcurrentList and removes it from the
	 * list.
	 *
	 * @return T element The first element.
	 */
	std::optional<T> popFront();

	/**
	 * Returns the last element of ConcurrentList and removes it from the
	 * list.
	 *
	 * @return T element The last element.
	 */
	std::optional<T> popBack();

	/**
	 * Inserts an element at the unsafeBeginning of ConcurrentList.
	 *
	 * @param T element The element to be inserted.
	 *
	 * @return const ListNode<T> *ptr A const pointer to the inserted element.
	 */
	const ListNode<T>* pushFront(const T& element);

	/**
	 * Inserts an element at the unsafeEnd of ConcurrentList.
	 *
	 * @param T element The element to be inserted.
	 *
	 * @return const ListNode<T> *ptr A const pointer to the inserted element.
	 */
	const ListNode<T>* pushBack(const T& element);

	/**
	 * Gets the element contained in the ListNode.
	 *
	 * @param ListNode<T> *node The node to get the element from.
	 *
	 * @return std::optional<T> element The element if the node wasn't nullptr,
	 * or no element for a nullptr node.
	 */
	std::optional<T> get(const ListNode<T> *ptr);

	/**
	 * Gets the ListNode that contains the element.
	 *
	 * @param const T& element The element to get the node for.
	 *
	 * @return const ListNode<T> *node The node that contains the element, or
	 * nullptr if the element was not in the list.
	 */
	const ListNode<T>* get(const T& element);

	/**
	 * Removes an element from ConcurrentList.
	 *
	 * @param T element The element to be removed.
	 *
	 * @return TRUE, the element was removed; FALSE, the element was not in the
	 * list.
	 */
	bool remove(const T& element);

	/**
	 * Removes a ListNode and pushes it to the front of ConcurrentList.
	 * Specialized function for LRU cache policy, so that only pointers are
	 * rearranged.
	 *
	 * @param ListNode<T> *node The node to remove and push front.
	 *
	 * @return TRUE for success; FALSE, the node was not in the list.
	 */
	bool removeAndPushFront(const ListNode<T> *ptr);

	/**
	 * Checks if ConcurrentList contains an element.
	 *
	 * @param T element The element to check for.
	 *
	 * @return TRUE, the list contains the element; FALSE, the list does not
	 * contain the element.
	 */
	bool contains(const T& element) const;

	/**
	 * Checks if ConcurrentList contains a ListNode.
	 *
	 * @param const ListNode<T> *node The node to check for.
	 *
	 * @return TRUE, the list contains the node; FALSE, the list does not
	 * contain the node.
	 */
	bool contains(const ListNode<T> *ptr) const;

	/**
	 * Returns an Iterator pointing to the unsafeBeginning (first element) of
	 * ConcurrentList.
	 *
	 * @return ListIterator iterator An Iterator pointing to unsafeBegin.
	 */
	ListIterator<T> unsafeBegin() const;

	/**
	 * Returns an Iterator pointing PAST the unsafeEnd (last element) of
	 * ConcurrentList.
	 *
	 * @return ListIterator iterator An Iterator pointing past the unsafeEnd.
	 */
	ListIterator<T> unsafeEnd() const;

	/**
	* Returns the size of ConcurrentList.
	*
	* @return std::size_t The size.
	*/
	std::size_t size() const;

	/**
	 * Returns the size of ConcurrentList without locking.
	 *
	 * @return std::size_t The size.
	 */
	std::size_t unsafeSize() const;

	/**
	* Check whether ConcurrentList is empty or not.
	*
	* @return TRUE if empty; FALSE if not empty.
	*/
	bool empty() const;

	void link(ListNode<T> *node);

	void unlink(ListNode<T> *node);

	/**
	* Clears all ConcurrentList's ListNodes and deallocates their memory.
	*/
	void unsafeClear();
private:
	/**
	 * Copy constructor helper for empty calling ConcurrentList.
	 */
	void copyCallingListEmpty(const ConcurrentList<T>& other);

	/**
	 * Copy constructor helper for same length ConcurrentLists.
	 */
	void copyListsSameLength(const ConcurrentList<T>& other);

	/**
	 * Copy constructor helper for longer calling ConcurrentList.
	 */
	void copyCallingListLonger(const ConcurrentList<T>& other);

	/**
	 * Copy constructor helper for shorter calling ConcurrentList.
	 */
	void copyCallingListShorter(const ConcurrentList<T>& other);

	/**
	* Searches for an element and returns the node that contains it.
	*/
	const ListNode<T>* search(const T& element) const;

	ListNode<T> *_head;
	ListNode<T> *_tail;
	std::size_t _size;
	std::shared_mutex _mutex;
};
}
#include "concurrent-list-impl.hpp"
