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
 * @class ListIterator<T, DerivedNode>
 * IConcurrentList Iterator.
 * 
 * ListIterators are invalidated on any (read|write) operation of
 * ConcurrentList. Thread safety is only guaranteed if the entire ConcurrentList
 * is locked before iteration.
 *
 */
template <typename T, typename DerivedNode>
class ListIterator {
public:
	using ListNodeT = DerivedNode;

	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = T;
	using difference_type = std::ptrdiff_t;
	using pointer = T*;
	using reference = T&;
	using const_reference = const T&;

	explicit ListIterator(ListNodeT *node) : _node(node) {}

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
	ListNodeT *_node;
};

// Forward declaration for overloaded insertion operator with template class.
template <typename T, typename DerivedNode>
class IConcurrentList;
template <typename T, typename DerivedNode>
std::ostream& operator<<(std::ostream&, const IConcurrentList<T, DerivedNode>&);

/**
* @class ConcurrentList<T>
* ConcurrentList, specialized as a Queue to be used for keeping track of order
* in LRU CacheManager.
*/
template <typename T, typename DerivedNode>
class IConcurrentList {
public:
	using ListNodeT = DerivedNode;

	/**
	 * Default constructor.
	 */
	IConcurrentList() : _head(nullptr), _tail(nullptr), _size(0) {}

	/**
	 * Copy constructor.
	 */
	IConcurrentList(const IConcurrentList<T, DerivedNode>& other);

	/*
	 * Move constructor.
	 */
	IConcurrentList(IConcurrentList<T, DerivedNode>&& other) noexcept;

	/**
	 * Assignment operator.
	 */
	IConcurrentList<T, DerivedNode>& operator=(const IConcurrentList<T, DerivedNode>& rhs);

	/**
	 * Move assignment operator.
	 */
	IConcurrentList<T, DerivedNode>& operator=(IConcurrentList<T, DerivedNode>&& rhs) noexcept;

	/**
	 * FriunsafeEnd declaration of ListIterator.
	 */
	friend class ListIterator<T, DerivedNode>;

	/**
	 * Overloaded insertion operator<<.
	 */
	friend std::ostream& operator<< <>(std::ostream& out,
		const IConcurrentList<T, DerivedNode>& list);

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
	virtual const ListNodeT* pushFront(const T& element) = 0;

	/**
	 * Inserts an element at the unsafeEnd of ConcurrentList.
	 *
	 * @param T element The element to be inserted.
	 *
	 * @return const ListNode<T> *ptr A const pointer to the inserted element.
	 */
	virtual const ListNodeT* pushBack(const T& element) = 0;

	/**
	 * Gets the element contained in the ListNode.
	 *
	 * @param ListNode<T> *node The node to get the element from.
	 *
	 * @return std::optional<T> element The element if the node wasn't nullptr,
	 * or no element for a nullptr node.
	 */
	virtual std::optional<T> get(const ListNodeT *node) = 0;

	/**
	 * Gets the ListNode that contains the element.
	 *
	 * @param const T& element The element to get the node for.
	 *
	 * @return const ListNode<T> *node The node that contains the element, or
	 * nullptr if the element was not in the list.
	 */
	virtual const ListNodeT* get(const T& element) = 0;

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
	virtual bool remove(const ListNodeT *node) = 0;

	/**
	 * Removes a ListNode and pushes it to the front of ConcurrentList.
	 * Specialized function for LRU cache policy, so that only pointers are
	 * rearranged.
	 *
	 * @param ListNode<T> *node The node to remove and push front.
	 *
	 * @return TRUE for success; FALSE, the node was not in the list.
	 */
	virtual bool removeAndPushFront(const ListNodeT *node) = 0;

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
	virtual bool contains(const ListNodeT *node) const = 0;

	/**
	 * Returns an Iterator pointing to the unsafeBeginning (first element) of
	 * ConcurrentList.
	 *
	 * @return ListIterator iterator An Iterator pointing to unsafeBegin.
	 */
	ListIterator<T, DerivedNode> unsafeBegin() const;

	/**
	 * Returns an Iterator pointing PAST the unsafeEnd (last element) of
	 * ConcurrentList.
	 *
	 * @return ListIterator iterator An Iterator pointing past the unsafeEnd.
	 */
	ListIterator<T, DerivedNode> unsafeEnd() const;

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
	size_t unsafeSize() const;

	/**
	* Check whether ConcurrentList is empty or not.
	*
	* @return TRUE if empty; FALSE if not empty.
	*/
	bool isEmpty() const;
	
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
	void copyCallingListEmpty(const IConcurrentList<T, DerivedNode>& other);

	/**
	 * Copy constructor helper for same length ConcurrentLists.
	 */
	void copyListsSameLength(const IConcurrentList<T, DerivedNode>& other);

	/**
	 * Copy constructor helper for longer calling ConcurrentList.
	 */
	void copyCallingListLonger(const IConcurrentList<T, DerivedNode>& other);

	/**
	 * Copy constructor helper for shorter calling ConcurrentList.
	 */
	void copyCallingListShorter(const IConcurrentList<T, DerivedNode>& other);

	virtual bool unlink(const ListNodeT *node) = 0;

	/**
	* Searches for an element and returns the node that contains it.
	*/
	virtual const ListNodeT* search(const T& element) const = 0;

	ListNodeT *_head;
	ListNodeT *_tail;
	size_t _size;
	// XXX Two options: mutable mutex since it is used in const functions,
	// Or discard the const on the methods.
	mutable std::shared_mutex _mutex;
};

/**
* @class ListNode<T>
* IConcurrentList Node.
*
* CoarseConcurrentList Node - relies on global list lock.
*/
template <typename T>
struct CoarseListNode {
	T ele;
	CoarseListNode *next;
	CoarseListNode *prev;
	
	CoarseListNode(const T& ele) :
	   ele(ele), next(nullptr), prev(nullptr) {}
	CoarseListNode(const T& ele, CoarseListNode<T> *next, CoarseListNode *prev) :
		ele(ele), next(next), prev(prev) {}
	~CoarseListNode() = default;	
};

/**
* @class ConcurrentList<T>
* CoarseConcurrentList, specialized as a Queue to be used for keeping track of order
* in LRU CacheManager.
*/
template <typename T>
class CoarseConcurrentList : public IConcurrentList<T, CoarseListNode<T>> {
public:
	using ListNodeT = CoarseListNode<T>;
	CoarseConcurrentList() : IConcurrentList<T, CoarseListNode<T>>() {}
	std::optional<T> popFront() override;
	std::optional<T> popBack() override;
	const CoarseListNode<T>* pushFront(const T& element) override;
	const CoarseListNode<T>* pushBack(const T& element) override;
	std::optional<T> get(const ListNodeT *node) override;
	const ListNodeT* get(const T& element) override;
	bool remove(const T& element) override;
	bool remove(const ListNodeT *node) override;
	bool removeAndPushFront(const ListNodeT *node) override;
	bool contains(const T& element) const override;
	bool contains(const ListNodeT *node) const override;
	size_t size() const override;
	void clear() override;
protected:
	bool unlink(const ListNodeT *node) override;
	const ListNodeT* search(const T& element) const override;
};

/**
* @class ListNode<T>
*
* FineConcurrentList Node - hand-over-hand per node lock.
*/
template <typename T>
struct FineListNode {
	T ele;
	FineListNode *next;
	FineListNode *prev;
	mutable std::shared_mutex mtx;
	
	FineListNode(const T& ele) :
	   ele(ele), next(nullptr), prev(nullptr) {}
	FineListNode(const T& ele, FineListNode<T> *next, FineListNode *prev) :
		ele(ele), next(next), prev(prev) {}
	~FineListNode() = default;	
};

/**
* @class ConcurrentList<T>
* CoarseConcurrentList, specialized as a Queue to be used for keeping track of order
* in LRU CacheManager.
*/
template <typename T>
class FineConcurrentList : public IConcurrentList<T, FineListNode<T>> {
public:
	using ListNodeT = FineListNode<T>;
	FineConcurrentList() : IConcurrentList<T, FineListNode<T>>() {}
	std::optional<T> popFront() override;
	std::optional<T> popBack() override;
	const FineListNode<T>* pushFront(const T& element) override;
	const FineListNode<T>* pushBack(const T& element) override;
	std::optional<T> get(const ListNodeT *node) override;
	const FineListNode<T>* get(const T& element) override;
	bool remove(const T& element) override;
	bool remove(const ListNodeT *node) override;
	bool removeAndPushFront(const ListNodeT *node) override;
	bool contains(const T& element) const override;
	bool contains(const ListNodeT *node) const override;
	size_t size() const override;
	void clear() override;
protected:
	bool unlink(const ListNodeT *node) override;
	const ListNodeT* search(const T& element) const override;
};
}
#include "concurrent-list-impl.hpp"
#include "coarse-concurrent-list-impl.hpp"
#include "fine-concurrent-list-impl.hpp"
