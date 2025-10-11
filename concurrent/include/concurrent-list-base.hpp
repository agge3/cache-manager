/*
 * @file concurrent-list.hpp
 *
 * @author agge3
 * @version 2.0
 * @since 2024-09-21
 *
 * ConcurrentList base class and helpers.
 */

#pragma once

#include <cstddef>
#include <iostream>
#include <iterator>
#include <optional>
#include <shared_mutex>
#include <memory>

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
	mutable std::shared_mutex _mtx;

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

	const_reference operator*() const { return _node->_ele; }
	pointer operator->() { return &_node->_ele; }
	
	ListIterator& operator++() {
		_node = _node->_next;
		return *this;
	}
	
	ListIterator operator++(int) {
		ListIterator tmp = *this;
		++(*this);
		return tmp;
	}
	
	ListIterator& operator--() {
		_node = _node->_prev;
		return *this;
	}
	
	ListIterator operator--(int) {
		ListIterator tmp = *this;
		--(*this);
		return tmp;
	}
	
	bool operator==(const ListIterator& other) const {
		return _node == other._node;
	}
	
	bool operator!=(const ListIterator& other) const {
		return _node != other._node;
	}
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
* Abstract base class for concurrent list implementations with different locking strategies.
* Specialized as a Queue to be used for keeping track of order in LRU CacheManager.
* 
* Derived classes override the public virtual methods (insert, remove, contains, etc.)
* to implement their specific locking strategies. The base class provides concrete
* helper methods for the actual structural mutations (linking, unlinking, searching).
*/
template <typename T>
class ConcurrentList {
public:
	/**
	 * Default constructor.
	 */
	ConcurrentList() : _head(nullptr), _tail(nullptr), _size(0) {}

	/**
	 * Virtual destructor.
	 */
	virtual ~ConcurrentList() { unsafeClear(); }

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
	 * Friend declaration of ListIterator.
	 */
	friend class ListIterator<T>;

	/**
	 * Overloaded insertion operator<<.
	 */
	friend std::ostream& operator<< <>(std::ostream& out,
		const ConcurrentList<T>& list);

	// ==================== Public Interface Methods (Virtual) ====================
	// Derived classes override these to implement their locking strategies.

	/**
	 * Inserts an element into the list.
	 * Derived classes override to add their locking strategy, then call
	 * the base class helper method insertHelper().
	 *
	 * @param const T& element The element to insert.
	 *
	 * @return bool TRUE if inserted successfully; FALSE if element already exists.
	 */
	virtual bool insert(const T& element) = 0;

	/**
	 * Removes an element from the list.
	 * Derived classes override to add their locking strategy, then call
	 * the base class helper method removeHelper().
	 *
	 * @param const T& element The element to remove.
	 *
	 * @return bool TRUE if removed successfully; FALSE if element not found.
	 */
	virtual bool remove(const T& element) = 0;

	/**
	 * Checks if the list contains an element.
	 * Derived classes override to add their locking strategy, then call
	 * the base class helper method search().
	 *
	 * @param const T& element The element to search for.
	 *
	 * @return bool TRUE if element is in list; FALSE otherwise.
	 */
	virtual bool contains(const T& element) const = 0;

	/**
	 * Checks if the list contains a ListNode.
	 * Derived classes override to add their locking strategy.
	 *
	 * @param const ListNode<T>* node The node to check for.
	 *
	 * @return bool TRUE if node is in list; FALSE otherwise.
	 */
	virtual bool contains(const ListNode<T> *ptr) const = 0;

	/**
	 * Removes a ListNode and pushes it to the front of the list.
	 * Specialized for LRU cache policy - only rearranges pointers.
	 * Derived classes override to add their locking strategy.
	 *
	 * @param const ListNode<T>* node The node to remove and push front.
	 *
	 * @return bool TRUE if successful; FALSE if node not in list.
	 */
	virtual bool removeAndPushFront(const ListNode<T> *ptr) = 0;

	/**
	 * Returns the size of the list.
	 * Derived classes override to add their locking strategy.
	 *
	 * @return std::size_t The number of elements in the list.
	 */
	virtual std::size_t size() const = 0;

	/**
	 * Checks if the list is empty.
	 * Derived classes override to add their locking strategy.
	 *
	 * @return bool TRUE if empty; FALSE otherwise.
	 */
	virtual bool isEmpty() const = 0;

	/**
	 * Unlinks a node from the list.
	 * Derived classes override to add their locking strategy, then call
	 * the base class helper method unlinkImpl().
	 *
	 * @param const ListNode<T>* node The node to unlink.
	 *
	 * @return bool TRUE if successful; FALSE if node not in list.
	 */
	virtual bool unlink(const ListNode<T> *node) = 0;

	/**
	 * Validates list integrity after threaded manipulation.
	 * Derived classes override as needed.
	 *
	 * @return bool TRUE if consistent; FALSE if corrupted.
	 */
	virtual bool isConsistent() const = 0;

	/**
	 * Clears all elements from the list.
	 * Derived classes override to add their locking strategy, then call
	 * the base class helper method unsafeClear().
	 */
	virtual void clear() = 0;

	// ==================== Public Convenience Methods (Non-Virtual) ====================

	/**
	 * Returns the first element of the list.
	 *
	 * @return std::optional<T> The first element, or empty if list is empty.
	 */
	std::optional<T> front() const;

	/**
	 * Returns the last element of the list.
	 *
	 * @return std::optional<T> The last element, or empty if list is empty.
	 */
	std::optional<T> back() const;

	/**
	 * Returns the first element and removes it from the list.
	 *
	 * @return std::optional<T> The first element, or empty if list is empty.
	 */
	std::optional<T> popFront();

	/**
	 * Returns the last element and removes it from the list.
	 *
	 * @return std::optional<T> The last element, or empty if list is empty.
	 */
	std::optional<T> popBack();

	/**
	 * Inserts an element at the beginning of the list.
	 *
	 * @param const T& element The element to insert.
	 *
	 * @return const ListNode<T>* A const pointer to the inserted node.
	 */
	const ListNode<T>* pushFront(const T& element);

	/**
	 * Inserts an element at the end of the list.
	 *
	 * @param const T& element The element to insert.
	 *
	 * @return const ListNode<T>* A const pointer to the inserted node.
	 */
	const ListNode<T>* pushBack(const T& element);

	/**
	 * Gets the element contained in a ListNode.
	 *
	 * @param const ListNode<T>* node The node to get the element from.
	 *
	 * @return std::optional<T> The element, or empty if node is invalid.
	 */
	std::optional<T> get(const ListNode<T> *ptr);

	/**
	 * Gets the ListNode that contains an element.
	 *
	 * @param const T& element The element to find.
	 *
	 * @return const ListNode<T>* The node containing element, or nullptr if not found.
	 */
	const ListNode<T>* get(const T& element);

	/**
	 * Returns an iterator pointing to the beginning of the list.
	 * Unsafe - no locking.
	 *
	 * @return ListIterator<T> Iterator to first element.
	 */
	ListIterator<T> unsafeBegin() const;

	/**
	 * Returns an iterator pointing past the end of the list.
	 * Unsafe - no locking.
	 *
	 * @return ListIterator<T> Iterator past last element.
	 */
	ListIterator<T> unsafeEnd() const;

	/**
	 * Returns the size without locking.
	 * Unsafe - no locking.
	 *
	 * @return std::size_t The size.
	 */
	std::size_t unsafeSize() const;

protected:
	// ==================== Protected Helper Methods (Non-Virtual) ====================
	// These handle the actual structural mutations. Derived classes call these
	// from within their virtual method overrides after acquiring locks.

	/**
	 * Searches for an element and returns the node containing it.
	 * Unsafe - no locking.
	 *
	 * @param const T& element The element to search for.
	 *
	 * @return const ListNode<T>* The node, or nullptr if not found.
	 */
	const ListNode<T>* search(const T& element) const {
		ListNode<T> *curr = this->_head;
		while (curr != nullptr) {
			if (curr->_ele == element) {
				return curr;
			}
			curr = curr->_next;
		}
		return nullptr;
	}

	/**
	 * Helper for insert() - performs the actual insertion at the back.
	 * Unsafe - no locking, assumes caller has acquired necessary locks.
	 *
	 * @param const T& element The element to insert.
	 *
	 * @return const ListNode<T>* The newly inserted node.
	 */
	const ListNode<T>* insertHelper(const T& element) {
		ListNode<T> *newNode = new ListNode<T>(element);
		link(newNode);
		return newNode;
	}

	/**
	 * Helper for remove() - performs the actual removal.
	 * Unsafe - no locking, assumes caller has acquired necessary locks.
	 *
	 * @param const T& element The element to remove.
	 *
	 * @return bool TRUE if removed; FALSE if not found.
	 */
	bool removeHelper(const T& element) {
		ListNode<T> *node = const_cast<ListNode<T>*>(search(element));
		if (node == nullptr) {
			return false;
		}
		return unlinkImpl(node);
	}

	/**
	 * Links a node into the list structure.
	 * Unsafe - no locking.
	 *
	 * @param ListNode<T>* node The node to link.
	 */
	void link(ListNode<T> *node) {
		node->_prev = this->_tail;
		node->_next = nullptr;
		
		if (this->_tail != nullptr) {
			this->_tail->_next = node;
		} else {
			this->_head = node;
		}
		
		this->_tail = node;
		this->_size++;
	}

	/**
	 * @brief Retires a node from active use and schedules it for safe reclamation.
	 *
	 * This method marks the given node as logically removed from the data structure
	 * and defers its physical deallocation until it is guaranteed that no other
	 * threads hold references to it. The actual reclamation mechanism depends on
	 * the concurrency strategy (e.g., hazard pointers or epoch-based reclamation).
	 *
	 * @param node Pointer to the node being retired. Must already be unlinked.
	 *
	 * @note This function should only be called by derived classes after a
	 * successful unlink operation. It is intentionally protected to prevent misuse.
	 */
	void retireNode(ListNode<T>* node) {
    	std::lock_guard<std::mutex> g(_retireMutex);
    	_retired.push_back(node);
    	
		if (_retired.size() > 1024) {  // arbitrary threshold
    	    for (auto* n : _retired) {
				delete n;
			}
    	    _retired.clear();
    	}
	}

	/**
	 * Unlinks a node from the list.
	 * Unsafe - no locking, assumes caller has acquired necessary locks.
	 *
	 * @param ListNode<T>* node The node to unlink.
	 *
	 * @return bool TRUE if successful; FALSE if node not in list.
	 */
	bool unlinkImpl(ListNode<T>* node) {
	    if (!node) return false;
	
	    if (node->_prev) node->_prev->_next = node->_next;
	    else this->_head = node->_next;
	
	    if (node->_next) node->_next->_prev = node->_prev;
	    else this->_tail = node->_prev;
	
	    this->_size--;
	
	    // Instead of deleting immediately, enqueue for later destruction
	    retireNode(node);
	    return true;
	}

	/**
	 * Clears all elements and deallocates memory.
	 * Unsafe - no locking.
	 */
	void unsafeClear() {
		ListNode<T> *curr = this->_head;
		while (curr != nullptr) {
			ListNode<T> *next = curr->_next;
			delete curr;
			curr = next;
		}
		this->_head = nullptr;
		this->_tail = nullptr;
		this->_size = 0;
	}

	/**
	 * Copy constructor helper for empty calling list.
	 */
	void copyCallingListEmpty(const ConcurrentList<T>& other);

	/**
	 * Copy constructor helper for same length lists.
	 */
	void copyListsSameLength(const ConcurrentList<T>& other);

	/**
	 * Copy constructor helper for longer calling list.
	 */
	void copyCallingListLonger(const ConcurrentList<T>& other);

	/**
	 * Copy constructor helper for shorter calling list.
	 */
	void copyCallingListShorter(const ConcurrentList<T>& other);

	/**
	 * Protected data members accessible to derived classes.
	 */
	ListNode<T> *_head;
	ListNode<T> *_tail;
	std::size_t _size;
	mutable std::shared_mutex _mutex;
    mutable std::mutex _retireMutex;
    std::vector<ListNode<T>*> _retired;
};

} // namespace cm
