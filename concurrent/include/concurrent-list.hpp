/**
 * @file concurrent-list.hpp
 *
 * @author agge3
 * @version 1.0
 * @since 2024-09-21
 *
 * ConcurrentList interface and concrete classes.
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
* IConcurrentList Node.
*
* @see Forward declaration - see concrete classes for definition.
*/
template <typename T>
struct ListNode<T>;

/**
 * @class ListIterator<T>
 * IConcurrentList Iterator.
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
	struct ListNode<T>;
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
class IConcurrentList {
public:
	/**
	 * Default constructor.
	 */
	IConcurrentList() : _head(nullptr), _tail(nullptr), _size(0) {}

	/**
	 * Copy constructor.
	 */
	IConcurrentList(const IConcurrentList<T>& other);

	/*
	 * Move constructor.
	 */
	IConcurrentList(IConcurrentList<T>&& other) noexcept;

	/**
	 * Assignment operator.
	 */
	IConcurrentList<T>& operator=(const IConcurrentList<T>& rhs);

	/**
	 * Move assignment operator.
	 */
	IConcurrentList<T>& operator=(IConcurrentList<T>&& rhs) noexcept;

	/**
	 * FriunsafeEnd declaration of ListIterator.
	 */
	friend class ListIterator<T>;

	/**
	 * Overloaded insertion operator<<.
	 */
	friend std::ostream& operator<< <>(std::ostream& out,
		const IConcurrentList<T>& list);

	/**
	 * Returns the first element of ConcurrentList.
	 *
	 * @return T element The first element.
	 */
	std::optional<T> front() const = 0;

	/**
	 * Returns the last element of ConcurrentList.
	 *
	 * @return T element The last element.
	 */
	std::optional<T> back() const = 0;

	/**
	 * Returns the first element of ConcurrentList and removes it from the
	 * list.
	 *
	 * @return T element The first element.
	 */
	virtual std::optional<T> popFront() = 0;

	/**
	 * Returns the last element of ConcurrentList and removes it from the
	 * list.
	 *
	 * @return T element The last element.
	 */
	virtual std::optional<T> popBack() = 0;

	/**
	 * Inserts an element at the unsafeBeginning of ConcurrentList.
	 *
	 * @param T element The element to be inserted.
	 *
	 * @return const ListNode<T> *ptr A const pointer to the inserted element.
	 */
	virtual ListNode<T>* pushFront(const T& element) = 0;

	/**
	 * Inserts an element at the unsafeEnd of ConcurrentList.
	 *
	 * @param T element The element to be inserted.
	 *
	 * @return const ListNode<T> *ptr A const pointer to the inserted element.
	 */
	virtual ListNode<T>* pushBack(const T& element) = 0;

	/**
	 * Gets the element contained in the ListNode.
	 *
	 * @param ListNode<T> *node The node to get the element from.
	 *
	 * @return std::optional<T> element The element if the node wasn't nullptr,
	 * or no element for a nullptr node.
	 */
	virtual std::optional<T> get(const ListNode<T> *node) = 0;

	/**
	 * Gets the ListNode that contains the element.
	 *
	 * @param const T& element The element to get the node for.
	 *
	 * @return const ListNode<T> *node The node that contains the element, or
	 * nullptr if the element was not in the list.
	 */
	virtual const ListNode<T>* get(const T& element) = 0;

	/**
	 * Removes an element from ConcurrentList.
	 *
	 * @param T element The element to be removed.
	 *
	 * @return TRUE, the element was removed; FALSE, the element was not in the
	 * list.
	 */
	virtual bool remove(const T& element) = 0;

	/**
	 * Removes an a ListNode from ConcurrentList.
	 *
	 * @param const ListNode<T> *node The node to be removed.
	 *
	 * @return TRUE, the nide was removed; FALSE, the node was not in the
	 * list.
	 *
	 * @post The node will be freed and invalid after this operation (if it
	 * wasn't already before).
	 */
	virtual bool remove(const ListNode<T> *node) = 0;

	/**
	 * Removes a ListNode and pushes it to the front of ConcurrentList.
	 * Specialized function for LRU cache policy, so that only pointers are
	 * rearranged.
	 *
	 * @param ListNode<T> *node The node to remove and push front.
	 *
	 * @return TRUE for success; FALSE, the node was not in the list.
	 */
	virtual bool removeAndPushFront(const ListNode<T> *node) = 0;

	/**
	 * Checks if ConcurrentList contains an element.
	 *
	 * @param T element The element to check for.
	 *
	 * @return TRUE, the list contains the element; FALSE, the list does not
	 * contain the element.
	 */
	virtual bool contains(const T& element) const = 0;

	/**
	 * Checks if ConcurrentList contains a ListNode.
	 *
	 * @param const ListNode<T> *node The node to check for.
	 *
	 * @return TRUE, the list contains the node; FALSE, the list does not
	 * contain the node.
	 */
	virtual bool contains(const ListNode<T> *node) const = 0;

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
	virtual size_t size() const = 0;

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
	bool isEmpty() const = 0;
	
	virtual void clear() = 0;

	/**
	* Clears all ConcurrentList's ListNodes and deallocates their memory.
	*/
	void unsafeClear();
	
	/**
	 * Destructor.
	 */
	~IConcurrentList() { unsafeClear(); }
protected:
	/**
	 * Copy constructor helper for empty calling ConcurrentList.
	 */
	void copyCallingListEmpty(const ConcurrentList<T>& other) = 0;

	/**
	 * Copy constructor helper for same length ConcurrentLists.
	 */
	void copyListsSameLength(const ConcurrentList<T>& other) = 0;

	/**
	 * Copy constructor helper for longer calling ConcurrentList.
	 */
	void copyCallingListLonger(const ConcurrentList<T>& other) = 0;

	/**
	 * Copy constructor helper for shorter calling ConcurrentList.
	 */
	void copyCallingListShorter(const ConcurrentList<T>& other) = 0;

	virtual bool unlink(ListNode<T> *node) = 0;

	/**
	* Searches for an element and returns the node that contains it.
	*/
	virtual const ListNode<T>* search(const T& element) const = 0;

	ListNode<T> *_head;
	ListNode<T> *_tail;
	std::size_t _size;
	// XXX Two options: mutable mutex since it is used in const functions,
	// Or discard the const on the methods.
	std::shared_mutex _mutex;
};

/**
* @class ConcurrentList<T>
* CoarseConcurrentList, specialized as a Queue to be used for keeping track of order
* in LRU CacheManager.
*/
template <typename T>
class CoarseConcurrentList : public IConcurrentList<T> {
public:
	/**
	* @class ListNode<T>
	* CoarseConcurrentList Node - relies on global list lock.
	*/
	struct ListNode {
		T ele;
		ListNode *next;
		ListNode *prev;
	
		ListNode(const T& ele) :
			ele(ele), next(nullptr), prev(nullptr) {}
		ListNode(const T& ele, ListNode *next, ListNode *prev) :
			ele(ele), next(next), prev(prev) {}
		~ListNode() {}
	};

	CoarseConcurrentList() : IConcurrentList<T>() {}
	std::optional<T> popFront() = 0;
	std::optional<T> popBack() = 0;
	ListNode<T>* pushFront(const T& element) = 0;
	ListNode<T>* pushBack(const T& element) = 0;
	std::optional<T> get(const ListNode<T> *node) = 0;
	const ListNode<T>* get(const T& element) = 0;
	bool remove(const T& element) = 0;
	bool remove(const ListNode<T> *node) = 0;
	bool removeAndPushFront(const ListNode<T> *node) = 0;
	bool contains(const T& element) const = 0;
	bool contains(const ListNode<T> *node) const = 0;
	size_t size() const = 0;
	void clear() = 0;
protected:
	bool unlink(ListNode<T> *node) = 0;
	const ListNode<T>* search(const T& element) const = 0;
};

/**
* @class ConcurrentList<T>
* CoarseConcurrentList, specialized as a Queue to be used for keeping track of order
* in LRU CacheManager.
*/
template <typename T>
class FineConcurrentList : public IConcurrentList<T> {
public:
	/**
	* @class ListNode<T>
	* FineConcurrentList Node - hand-over-hand locking.
	*/
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

	FineConcurrentList() : IConcurrentList<T>() {}
	std::optional<T> popFront() override;
	std::optional<T> popBack() override;
	ListNode<T>* pushFront(const T& element) override;
	ListNode<T>* pushBack(const T& element) override;
	std::optional<T> get(const ListNode<T> *node) override;
	const ListNode<T>* get(const T& element) override;
	bool remove(const T& element) override;
	bool remove(const ListNode<T> *node) override;
	bool removeAndPushFront(const ListNode<T> *node) override;
	bool contains(const T& element) const override;
	bool contains(const ListNode<T> *node) const override;
	size_t size() const override;
	void clear() = override;
protected:
	bool unlink(ListNode<T> *node) = override;
	const ListNode<T>* search(const T& element) const override;
};
}
#include "concurrent-list-impl.hpp"
#include "coarse-concurrent-list-impl.hpp"
#include "fine-concurrent-list-impl.hpp"
