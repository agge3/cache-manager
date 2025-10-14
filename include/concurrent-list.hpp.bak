/*
 * @file concurrent-list-base.hpp
 *
 * @author agge3, kpowkitty
 * @version 2.0
 * @since 2024-09-21
 *
 * IConcurrentList base class and helpers.
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
* IConcurrentList Node.
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
 * IConcurrentList Iterator.
 * 
 * ListIterators are invalidated on any (read|write) operation of
 * IConcurrentList. Thread safety is only guaranteed if the entire IConcurrentList
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
class IConcurrentList;
template <typename T>
std::ostream& operator<<(std::ostream&, const IConcurrentList<T>&);

/**
* @class IConcurrentList<T>
* Abstract base class for concurrent list implementations with different locking strategies.
* Specialized as a Queue to be used for keeping track of order in LRU CacheManager.
* 
* Derived classes override the public virtual methods (insert, remove, contains, etc.)
* to implement their specific locking strategies. The base class provides concrete
* helper methods for the actual structural mutations (linking, unlinking, searching).
*/
template <typename T>
class IConcurrentList {
public:
	/**
	 * Default constructor.
	 */
	IConcurrentList() : _head(nullptr), _tail(nullptr), _size(0) {}

	/**
	 * Virtual destructor.
	 */
	virtual ~IConcurrentList() { unsafeClear(); }

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
	 * Friend declaration of ListIterator.
	 */
	friend class ListIterator<T>;

	/**
	 * Overloaded insertion operator<<.
	 */
	friend std::ostream& operator<< <>(std::ostream& out,
		const IConcurrentList<T>& list);

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
	void copyCallingListEmpty(const IConcurrentList<T>& other);

	/**
	 * Copy constructor helper for same length lists.
	 */
	void copyListsSameLength(const IConcurrentList<T>& other);

	/**
	 * Copy constructor helper for longer calling list.
	 */
	void copyCallingListLonger(const IConcurrentList<T>& other);

	/**
	 * Copy constructor helper for shorter calling list.
	 */
	void copyCallingListShorter(const IConcurrentList<T>& other);

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

/**
 * @class CoarseGrainedList<T>
 *
 * CoarseGrainedList - Concurrent list with coarse-grained locking.
 * Acquires a single lock for the entire list on any mutation operation.
 * 
 * Concurrent list implementation using coarse-grained locking.
 * All operations that mutate or read the list acquire the single list-wide mutex.
 * Simple, but potentially lower concurrency than fine-grained strategies.
 */
template <typename T>
class CoarseGrainedList : public IConcurrentList<T> {
public:
	/**
	 * Default constructor.
	 */
	CoarseGrainedList() : IConcurrentList<T>() {}

	/**
	 * Destructor.
	 */
	virtual ~CoarseGrainedList() {}

	// ==================== Virtual Method Implementations ====================

	/**
	 * Inserts an element into the list.
	 * Acquires exclusive lock for entire operation.
	 *
	 * @param const T& element The element to insert.
	 *
	 * @return bool TRUE if inserted successfully; FALSE if element already exists.
	 */
	bool insert(const T& element) override {
		std::unique_lock<std::shared_mutex> lock(this->_mutex);
		
		// Check if element already exists
		if (this->search(element) != nullptr) {
			return false;
		}
		
		// Insert at back
		this->insertHelper(element);
		return true;
	}

	/**
	 * Removes an element from the list.
	 * Acquires exclusive lock for entire operation.
	 *
	 * @param const T& element The element to remove.
	 *
	 * @return bool TRUE if removed successfully; FALSE if element not found.
	 */
	bool remove(const T& element) override {
		std::unique_lock<std::shared_mutex> lock(this->_mutex);
		return this->removeHelper(element);
	}

	/**
	 * Checks if the list contains an element.
	 * Acquires shared lock for read operation.
	 *
	 * @param const T& element The element to search for.
	 *
	 * @return bool TRUE if element is in list; FALSE otherwise.
	 */
	bool contains(const T& element) const override {
		std::shared_lock<std::shared_mutex> lock(this->_mutex);
		return this->search(element) != nullptr;
	}

	/**
	 * Checks if the list contains a ListNode.
	 * Acquires shared lock for read operation.
	 *
	 * @param const ListNode<T>* node The node to check for.
	 *
	 * @return bool TRUE if node is in list; FALSE otherwise.
	 */
	bool contains(const ListNode<T> *ptr) const override {
		std::shared_lock<std::shared_mutex> lock(this->_mutex);
		
		// Traverse list to find node
		for (ListNode<T> *curr = this->_head; curr != nullptr; curr = curr->_next) {
			if (curr == ptr) {
				return true;
			}
		}
		return false;
	}

	/**
	 * Removes a ListNode and pushes it to the front of the list.
	 * Acquires exclusive lock for entire operation.
	 *
	 * @param const ListNode<T>* node The node to remove and push front.
	 *
	 * @return bool TRUE if successful; FALSE if node not in list.
	 */
	bool removeAndPushFront(const ListNode<T> *ptr) override {
		std::unique_lock<std::shared_mutex> lock(this->_mutex);
		
		// Cast away const for manipulation
		ListNode<T> *node = const_cast<ListNode<T>*>(ptr);
		
		// Verify node is in list
		ListNode<T> *curr = this->_head;
		bool found = false;
		while (curr != nullptr) {
			if (curr == node) {
				found = true;
				break;
			}
			curr = curr->_next;
		}
		
		if (!found) {
			return false;
		}
		
		// Unlink from current position
		if (!this->unlinkImpl(node)) {
			return false;
		}
		
		// Link at front
		node->_next = this->_head;
		node->_prev = nullptr;
		
		if (this->_head != nullptr) {
			this->_head->_prev = node;
		} else {
			this->_tail = node;
		}
		
		this->_head = node;
		return true;
	}

	/**
	 * Returns the size of the list.
	 * Acquires shared lock for read operation.
	 *
	 * @return std::size_t The number of elements in the list.
	 */
	std::size_t size() const override {
		std::shared_lock<std::shared_mutex> lock(this->_mutex);
		return this->_size;
	}

	/**
	 * Checks if the list is empty.
	 * Acquires shared lock for read operation.
	 *
	 * @return bool TRUE if empty; FALSE otherwise.
	 */
	bool isEmpty() const override {
		std::shared_lock<std::shared_mutex> lock(this->_mutex);
		return this->_size == 0;
	}

	/**
	 * Unlinks a node from the list.
	 * Acquires exclusive lock for entire operation.
	 *
	 * @param const ListNode<T>* node The node to unlink.
	 *
	 * @return bool TRUE if successful; FALSE if node not in list.
	 */
	bool unlink(const ListNode<T> *node) override {
		std::unique_lock<std::shared_mutex> lock(this->_mutex);
		return this->unlinkImpl(const_cast<ListNode<T>*>(node));
	}

	/**
	 * Validates list integrity after threaded manipulation.
	 * Acquires shared lock for read operation.
	 *
	 * @return bool TRUE if consistent; FALSE if corrupted.
	 */
	bool isConsistent() const override {
		std::shared_lock<std::shared_mutex> lock(this->_mutex);
		
		// Check head/tail consistency
		if ((this->_head == nullptr) != (this->_tail == nullptr)) {
			return false;
		}
		
		if (this->_size == 0 && (this->_head != nullptr || this->_tail != nullptr)) {
			return false;
		}
		
		// Traverse forward and count
		std::size_t count = 0;
		ListNode<T> *curr = this->_head;
		ListNode<T> *prev = nullptr;
		
		while (curr != nullptr) {
			count++;
			
			// Check backward link
			if (curr->_prev != prev) {
				return false;
			}
			
			prev = curr;
			curr = curr->_next;
		}
		
		// Check tail
		if (prev != this->_tail) {
			return false;
		}
		
		// Check size matches count
		if (count != this->_size) {
			return false;
		}
		
		return true;
	}

	/**
	 * Clears all elements from the list.
	 * Acquires exclusive lock for entire operation.
	 */
	void clear() override {
		std::unique_lock<std::shared_mutex> lock(this->_mutex);
		this->unsafeClear();
	}
};

/**
 * @class GroupedFineGrainedList<T>
 *
 * GroupedFineGrainedList - Concurrent list with grouped fine-grained locking.
 * Acquires locks on prev, curr, and next nodes during mutations.
 * 
 * Concurrent list implementation using grouped fine-grained locking.
 * Locks a small window (prev, curr, next) around the mutation point.
 * Better concurrency than coarse-grained, but simpler than hand-over-hand.
 */
template <typename T>
class GroupedFineGrainedList : public IConcurrentList<T> {
public:
	/**
	 * Default constructor.
	 */
	GroupedFineGrainedList() : IConcurrentList<T>() {}

	/**
	 * Destructor.
	 */
	virtual ~GroupedFineGrainedList() {}

	// ==================== Virtual Method Implementations ====================

	/**
	 * Inserts an element into the list.
	 * Locks the tail and its neighbors during insertion.
	 *
	 * @param const T& element The element to insert.
	 *
	 * @return bool TRUE if inserted successfully; FALSE if element already exists.
	 */
	bool insert(const T& element) override {
		// First, search without holding node locks (use list mutex for consistency)
		std::shared_lock<std::shared_mutex> searchLock(this->_mutex);
		if (this->search(element) != nullptr) {
			searchLock.unlock();
			return false;
		}
		searchLock.unlock();
		
		// Now lock the insertion point (tail and its neighbors)
		std::unique_lock<std::shared_mutex> listLock(this->_mutex);
		
		// Lock tail node and its prev (prev may be null)
		ListNode<T> *tail = this->_tail;
		ListNode<T> *tailPrev = (tail != nullptr) ? tail->_prev : nullptr;
		
		std::unique_ptr<std::unique_lock<std::shared_mutex>> tailLock;
		std::unique_ptr<std::unique_lock<std::shared_mutex>> prevLock;
		
		if (tail != nullptr) {
			tailLock = std::make_unique<std::unique_lock<std::shared_mutex>>(tail->_mtx);
		}
		if (tailPrev != nullptr) {
			prevLock = std::make_unique<std::unique_lock<std::shared_mutex>>(tailPrev->_mtx);
		}
		
		listLock.unlock();
		
		// Double-check element still doesn't exist (while holding node locks)
		if (this->search(element) != nullptr) {
			return false;
		}
		
		// Perform insertion
		this->insertHelper(element);
		return true;
	}

	/**
	 * Removes an element from the list.
	 * Locks prev, curr, and next nodes during removal.
	 *
	 * @param const T& element The element to remove.
	 *
	 * @return bool TRUE if removed successfully; FALSE if element not found.
	 */
	bool remove(const T& element) override {
		std::shared_lock<std::shared_mutex> listLock(this->_mutex);
		
		// Find the node to remove
		ListNode<T> *curr = this->_head;
		while (curr != nullptr && curr->_ele != element) {
			curr = curr->_next;
		}
		
		if (curr == nullptr) {
			return false;
		}
		
		// Now lock the window: prev, curr, next
		ListNode<T> *prev = curr->_prev;
		ListNode<T> *next = curr->_next;
		
		std::unique_ptr<std::unique_lock<std::shared_mutex>> prevLock;
		std::unique_ptr<std::unique_lock<std::shared_mutex>> currLock;
		std::unique_ptr<std::unique_lock<std::shared_mutex>> nextLock;
		
		if (prev != nullptr) {
			prevLock = std::make_unique<std::unique_lock<std::shared_mutex>>(prev->_mtx);
		}
		currLock = std::make_unique<std::unique_lock<std::shared_mutex>>(curr->_mtx);
		if (next != nullptr) {
			nextLock = std::make_unique<std::unique_lock<std::shared_mutex>>(next->_mtx);
		}
		
		listLock.unlock();
		
		// Verify element is still there (could have been removed by another thread)
		if (curr->_ele != element) {
			return false;
		}
		
		// Perform removal
		return this->removeHelper(element);
	}

	/**
	 * Checks if the list contains an element.
	 * Locks nodes along the traversal path.
	 *
	 * @param const T& element The element to search for.
	 *
	 * @return bool TRUE if element is in list; FALSE otherwise.
	 */
	bool contains(const T& element) const override {
		std::shared_lock<std::shared_mutex> listLock(this->_mutex);
		
		ListNode<T> *curr = this->_head;
		while (curr != nullptr) {
			std::shared_lock<std::shared_mutex> currLock(curr->_mtx);
			
			if (curr->_ele == element) {
				return true;
			}
			
			curr = curr->_next;
		}
		
		return false;
	}

	/**
	 * Checks if the list contains a ListNode.
	 * Locks nodes along the traversal path.
	 *
	 * @param const ListNode<T>* node The node to check for.
	 *
	 * @return bool TRUE if node is in list; FALSE otherwise.
	 */
	bool contains(const ListNode<T> *ptr) const override {
		std::shared_lock<std::shared_mutex> listLock(this->_mutex);
		
		ListNode<T> *curr = this->_head;
		while (curr != nullptr) {
			std::shared_lock<std::shared_mutex> currLock(curr->_mtx);
			
			if (curr == ptr) {
				return true;
			}
			
			curr = curr->_next;
		}
		
		return false;
	}

	/**
	 * Removes a ListNode and pushes it to the front of the list.
	 * Locks prev, curr, next and the new head during operation.
	 *
	 * @param const ListNode<T>* node The node to remove and push front.
	 *
	 * @return bool TRUE if successful; FALSE if node not in list.
	 */
	bool removeAndPushFront(const ListNode<T> *ptr) override {
		std::unique_lock<std::shared_mutex> listLock(this->_mutex);
		
		ListNode<T> *node = const_cast<ListNode<T>*>(ptr);
		
		// Verify node is in list
		ListNode<T> *curr = this->_head;
		bool found = false;
		while (curr != nullptr) {
			if (curr == node) {
				found = true;
				break;
			}
			curr = curr->_next;
		}
		
		if (!found) {
			return false;
		}
		
		// Lock the window around current node
		ListNode<T> *prev = node->_prev;
		ListNode<T> *next = node->_next;
		ListNode<T> *oldHead = this->_head;
		
		std::unique_ptr<std::unique_lock<std::shared_mutex>> prevLock;
		std::unique_ptr<std::unique_lock<std::shared_mutex>> nodeLock;
		std::unique_ptr<std::unique_lock<std::shared_mutex>> nextLock;
		std::unique_ptr<std::unique_lock<std::shared_mutex>> headLock;
		
		if (prev != nullptr) {
			prevLock = std::make_unique<std::unique_lock<std::shared_mutex>>(prev->_mtx);
		}
		nodeLock = std::make_unique<std::unique_lock<std::shared_mutex>>(node->_mtx);
		if (next != nullptr) {
			nextLock = std::make_unique<std::unique_lock<std::shared_mutex>>(next->_mtx);
		}
		if (oldHead != nullptr) {
			headLock = std::make_unique<std::unique_lock<std::shared_mutex>>(oldHead->_mtx);
		}
		
		listLock.unlock();
		
		// Unlink from current position
		if (!this->unlinkImpl(node)) {
			return false;
		}
		
		// Link at front
		node->_next = oldHead;
		node->_prev = nullptr;
		if (oldHead != nullptr) {
			oldHead->_prev = node;
		}
		this->_head = node;
		
		return true;
	}

	/**
	 * Returns the size of the list.
	 * Acquires list lock for consistency.
	 *
	 * @return std::size_t The number of elements in the list.
	 */
	std::size_t size() const override {
		std::shared_lock<std::shared_mutex> listLock(this->_mutex);
		return this->_size;
	}

	/**
	 * Checks if the list is empty.
	 * Acquires list lock for consistency.
	 *
	 * @return bool TRUE if empty; FALSE otherwise.
	 */
	bool isEmpty() const override {
		std::shared_lock<std::shared_mutex> listLock(this->_mutex);
		return this->_size == 0;
	}

	/**
	 * Unlinks a node from the list.
	 * Locks prev, curr, and next nodes during unlink.
	 *
	 * @param const ListNode<T>* node The node to unlink.
	 *
	 * @return bool TRUE if successful; FALSE if node not in list.
	 */
	bool unlink(const ListNode<T> *node) override {
		std::unique_lock<std::shared_mutex> listLock(this->_mutex);
		
		ListNode<T> *curr = const_cast<ListNode<T>*>(node);
		
		// Verify node is in list
		ListNode<T> *traverser = this->_head;
		bool found = false;
		while (traverser != nullptr) {
			if (traverser == curr) {
				found = true;
				break;
			}
			traverser = traverser->_next;
		}
		
		if (!found) {
			return false;
		}
		
		// Lock the window: prev, curr, next
		ListNode<T> *prev = curr->_prev;
		ListNode<T> *next = curr->_next;
		
		std::unique_ptr<std::unique_lock<std::shared_mutex>> prevLock;
		std::unique_ptr<std::unique_lock<std::shared_mutex>> currLock;
		std::unique_ptr<std::unique_lock<std::shared_mutex>> nextLock;
		
		if (prev != nullptr) {
			prevLock = std::make_unique<std::unique_lock<std::shared_mutex>>(prev->_mtx);
		}
		currLock = std::make_unique<std::unique_lock<std::shared_mutex>>(curr->_mtx);
		if (next != nullptr) {
			nextLock = std::make_unique<std::unique_lock<std::shared_mutex>>(next->_mtx);
		}
		
		listLock.unlock();
		
		return this->unlinkImpl(curr);
	}

	/**
	 * Validates list integrity after threaded manipulation.
	 * Locks list and all nodes during validation.
	 *
	 * @return bool TRUE if consistent; FALSE if corrupted.
	 */
	bool isConsistent() const override {
		std::unique_lock<std::shared_mutex> listLock(this->_mutex);
		
		// Check head/tail consistency
		if ((this->_head == nullptr) != (this->_tail == nullptr)) {
			return false;
		}
		
		if (this->_size == 0 && (this->_head != nullptr || this->_tail != nullptr)) {
			return false;
		}
		
		// Lock and traverse all nodes
		std::size_t count = 0;
		ListNode<T> *curr = this->_head;
		ListNode<T> *prev = nullptr;
		
		while (curr != nullptr) {
			std::shared_lock<std::shared_mutex> currLock(curr->_mtx);
			count++;
			
			// Check backward link
			if (curr->_prev != prev) {
				return false;
			}
			
			prev = curr;
			curr = curr->_next;
		}
		
		// Check tail
		if (prev != this->_tail) {
			return false;
		}
		
		// Check size matches count
		if (count != this->_size) {
			return false;
		}
		
		return true;
	}

	/**
	 * Clears all elements from the list.
	 * Acquires list lock for entire operation.
	 */
	void clear() override {
		std::unique_lock<std::shared_mutex> listLock(this->_mutex);
		this->unsafeClear();
	}
};

} // namespace cm

/**
 * @class HandOverHandList<T>
 *
 * HandOverHandList - Concurrent list with hand-over-hand locking.
 * Acquires lock on current node, then next node, then releases current before moving.
 * 
 * Concurrent list implementation using hand-over-hand (lock-coupling) locking.
 * During traversal, locks the current node, then locks the next node, then
 * releases the current node. This ensures no gaps in coverage and prevents
 * other threads from modifying the list structure during traversal.
 * Maximizes concurrency by holding minimal locks at any time.
 */
template <typename T>
class HandOverHandList : public IConcurrentList<T> {
public:
	/**
	 * Default constructor.
	 */
	HandOverHandList() : IConcurrentList<T>() {}

	/**
	 * Destructor.
	 */
	virtual ~HandOverHandList() {}

	// ==================== Virtual Method Implementations ====================

	/**
	 * Inserts an element into the list.
	 * Hand-over-hand locks during search, then locks insertion point.
	 *
	 * @param const T& element The element to insert.
	 *
	 * @return bool TRUE if inserted successfully; FALSE if element already exists.
	 */
	bool insert(const T& element) override {
		// Handle empty list case
		if (this->_head == nullptr) {
			std::unique_lock<std::shared_mutex> headLock(this->_mutex);
			if (this->_head == nullptr) {
				this->insertHelper(element);
				return true;
			}
		}
		
		// Lock head
		auto currLock = std::make_unique<std::unique_lock<std::shared_mutex>>(this->_head->_mtx);
		ListNode<T> *curr = this->_head;
		
		while (curr != nullptr && curr->_ele < element) {
			ListNode<T> *next = curr->_next;
			
			if (next == nullptr) {
				break;
			}
			
			// Lock next before releasing current (hand-over-hand)
			auto nextLock = std::make_unique<std::unique_lock<std::shared_mutex>>(next->_mtx);
			currLock.reset();
			
			currLock = std::move(nextLock);
			curr = next;
		}
		
		// Check if element already exists
		if (curr != nullptr && curr->_ele == element) {
			return false;
		}
		
		// Insert at current position
		this->insertHelper(element);
		return true;
	}

	/**
	 * Removes an element from the list.
	 * Hand-over-hand locks during search, locks prev/curr/next during removal.
	 *
	 * @param const T& element The element to remove.
	 *
	 * @return bool TRUE if removed successfully; FALSE if element not found.
	 */
	bool remove(const T& element) override {
	    if (this->_head == nullptr) {
	        return false;
	    }
	
	    auto currLock = std::make_unique<std::unique_lock<std::shared_mutex>>(this->_head->_mtx);
	    ListNode<T>* curr = this->_head;
	    ListNode<T>* prev = nullptr;
	
	    while (curr != nullptr) {
	        if (curr->_ele == element) {
	            // Lock neighbors
	            std::unique_ptr<std::unique_lock<std::shared_mutex>> prevLock;
	            std::unique_ptr<std::unique_lock<std::shared_mutex>> nextLock;
	
	            ListNode<T>* next = curr->_next;
	
	            if (prev != nullptr) {
	                prevLock = std::make_unique<std::unique_lock<std::shared_mutex>>(prev->_mtx);
	            }
	            if (next != nullptr) {
	                nextLock = std::make_unique<std::unique_lock<std::shared_mutex>>(next->_mtx);
	            }
	
	            bool success = this->unlinkImpl(curr);
	            currLock.reset(); // Release node lock before retiring
	
	            if (success) {
	                this->retireNode(curr);
	            }
	
	            return success;
	        }
	
	        ListNode<T>* next = curr->_next;
	        if (next == nullptr) {
	            return false;
	        }
	
	        // Hand-over-hand locking
	        auto nextLock = std::make_unique<std::unique_lock<std::shared_mutex>>(next->_mtx);
	        currLock.reset();
	
	        prev = curr;
	        currLock = std::move(nextLock);
	        curr = next;
	    }
	
	    return false;
	}

	/**
	 * Checks if the list contains an element.
	 * Hand-over-hand locks during search using shared locks.
	 *
	 * @param const T& element The element to search for.
	 *
	 * @return bool TRUE if element is in list; FALSE otherwise.
	 */
	bool contains(const T& element) const override {
		if (this->_head == nullptr) {
			return false;
		}
		
		// Lock head with shared lock
		auto currLock = std::make_unique<std::shared_lock<std::shared_mutex>>(this->_head->_mtx);
		ListNode<T> *curr = this->_head;
		
		// Hand-over-hand search
		while (curr != nullptr) {
			if (curr->_ele == element) {
				return true;
			}
			
			ListNode<T> *next = curr->_next;
			
			if (next == nullptr) {
				return false;
			}
			
			// Lock next before releasing current (hand-over-hand)
			auto nextLock = std::make_unique<std::shared_lock<std::shared_mutex>>(next->_mtx);
			currLock.reset();
			
			currLock = std::move(nextLock);
			curr = next;
		}
		
		return false;
	}

	/**
	 * Checks if the list contains a ListNode.
	 * Hand-over-hand locks during search.
	 *
	 * @param const ListNode<T>* node The node to check for.
	 *
	 * @return bool TRUE if node is in list; FALSE otherwise.
	 */
	bool contains(const ListNode<T> *ptr) const override {
		if (this->_head == nullptr) {
			return false;
		}
		
		// Lock head with shared lock
		auto currLock = std::make_unique<std::shared_lock<std::shared_mutex>>(this->_head->_mtx);
		ListNode<T> *curr = this->_head;
		
		// Hand-over-hand search
		while (curr != nullptr) {
			if (curr == ptr) {
				return true;
			}
			
			ListNode<T> *next = curr->_next;
			
			if (next == nullptr) {
				return false;
			}
			
			// Lock next before releasing current (hand-over-hand)
			auto nextLock = std::make_unique<std::shared_lock<std::shared_mutex>>(next->_mtx);
			currLock.reset();
			
			currLock = std::move(nextLock);
			curr = next;
		}
		
		return false;
	}

	/**
	 * Removes a ListNode and pushes it to the front of the list.
	 * Hand-over-hand locks to find node, then locks head for reinsertion.
	 *
	 * @param const ListNode<T>* node The node to remove and push front.
	 *
	 * @return bool TRUE if successful; FALSE if node not in list.
	 */
	bool removeAndPushFront(const ListNode<T> *ptr) override {
		if (this->_head == nullptr) {
			return false;
		}
		
		ListNode<T> *node = const_cast<ListNode<T>*>(ptr);
		
		// Hand-over-hand search to find node
		auto currLock = std::make_unique<std::unique_lock<std::shared_mutex>>(this->_head->_mtx);
		ListNode<T> *curr = this->_head;
		bool found = false;
		
		while (curr != nullptr) {
			if (curr == node) {
				found = true;
				break;
			}
			
			ListNode<T> *next = curr->_next;
			
			if (next == nullptr) {
				return false;
			}
			
			// Lock next before releasing current (hand-over-hand)
			auto nextLock = std::make_unique<std::unique_lock<std::shared_mutex>>(next->_mtx);
			currLock.reset();
			
			currLock = std::move(nextLock);
			curr = next;
		}
		
		if (!found) {
			return false;
		}
		
		// Now we hold lock on node, need to lock prev and next
		ListNode<T> *prev = node->_prev;
		ListNode<T> *next = node->_next;
		
		std::unique_ptr<std::unique_lock<std::shared_mutex>> prevLock;
		std::unique_ptr<std::unique_lock<std::shared_mutex>> nextLock;
		
		if (prev != nullptr) {
			prevLock = std::make_unique<std::unique_lock<std::shared_mutex>>(prev->_mtx);
		}
		if (next != nullptr) {
			nextLock = std::make_unique<std::unique_lock<std::shared_mutex>>(next->_mtx);
		}
		
		// Unlink from current position
		if (!this->unlinkImpl(node)) {
			return false;
		}
		
		// Lock old head
		auto oldHeadLock = std::make_unique<std::unique_lock<std::shared_mutex>>(this->_head->_mtx);
		
		// Link at front
		node->_next = this->_head;
		node->_prev = nullptr;
		this->_head->_prev = node;
		this->_head = node;
		
		return true;
	}

	/**
	 * Returns the size of the list.
	 * Hand-over-hand traverses to count elements.
	 *
	 * @return std::size_t The number of elements in the list.
	 */
	std::size_t size() const override {
		if (this->_head == nullptr) {
			return 0;
		}
		
		std::size_t count = 0;
		
		// Hand-over-hand traverse
		auto currLock = std::make_unique<std::shared_lock<std::shared_mutex>>(this->_head->_mtx);
		ListNode<T> *curr = this->_head;
		
		while (curr != nullptr) {
			count++;
			
			ListNode<T> *next = curr->_next;
			
			if (next == nullptr) {
				break;
			}
			
			// Lock next before releasing current (hand-over-hand)
			auto nextLock = std::make_unique<std::shared_lock<std::shared_mutex>>(next->_mtx);
			currLock.reset();
			
			currLock = std::move(nextLock);
			curr = next;
		}
		
		return count;
	}

	/**
	 * Checks if the list is empty.
	 * Locks head to check if it's null.
	 *
	 * @return bool TRUE if empty; FALSE otherwise.
	 */
	bool isEmpty() const override {
		if (this->_head == nullptr) {
			return true;
		}
		
		std::shared_lock<std::shared_mutex> headLock(this->_head->_mtx);
		return this->_head == nullptr;
	}

	/**
	 * Unlinks a node from the list.
	 * Hand-over-hand locks to find and verify node, then unlinks.
	 *
	 * @param const ListNode<T>* node The node to unlink.
	 *
	 * @return bool TRUE if successful; FALSE if node not in list.
	 */
	bool unlink(const ListNode<T> *node) override {
		if (this->_head == nullptr) {
			return false;
		}
		
		ListNode<T> *curr = const_cast<ListNode<T>*>(node);
		
		// Hand-over-hand search to find node
		auto currLock = std::make_unique<std::unique_lock<std::shared_mutex>>(this->_head->_mtx);
		ListNode<T> *traverser = this->_head;
		bool found = false;
		
		while (traverser != nullptr) {
			if (traverser == curr) {
				found = true;
				break;
			}
			
			ListNode<T> *next = traverser->_next;
			
			if (next == nullptr) {
				return false;
			}
			
			// Lock next before releasing current (hand-over-hand)
			auto nextLock = std::make_unique<std::unique_lock<std::shared_mutex>>(next->_mtx);
			currLock.reset();
			
			currLock = std::move(nextLock);
			traverser = next;
		}
		
		if (!found) {
			return false;
		}
		
		// Lock prev and next
		ListNode<T> *prev = curr->_prev;
		ListNode<T> *next = curr->_next;
		
		std::unique_ptr<std::unique_lock<std::shared_mutex>> prevLock;
		std::unique_ptr<std::unique_lock<std::shared_mutex>> nextLock;
		
		if (prev != nullptr) {
			prevLock = std::make_unique<std::unique_lock<std::shared_mutex>>(prev->_mtx);
		}
		if (next != nullptr) {
			nextLock = std::make_unique<std::unique_lock<std::shared_mutex>>(next->_mtx);
		}
		
		bool success = this->unlinkImpl(curr);
		if (success) {
		    this->retireNode(curr);
		}
		return success;
	}

	/**
	 * Validates list integrity after threaded manipulation.
	 * Hand-over-hand traverses to validate structure.
	 *
	 * @return bool TRUE if consistent; FALSE if corrupted.
	 */
	bool isConsistent() const override {
		if (this->_head == nullptr) {
			return this->_tail == nullptr && this->_size == 0;
		}
		
		std::size_t count = 0;
		ListNode<T> *prev = nullptr;
		
		// Hand-over-hand traverse
		auto currLock = std::make_unique<std::shared_lock<std::shared_mutex>>(this->_head->_mtx);
		ListNode<T> *curr = this->_head;
		
		while (curr != nullptr) {
			count++;
			
			// Check backward link
			if (curr->_prev != prev) {
				return false;
			}
			
			ListNode<T> *next = curr->_next;
			
			if (next == nullptr) {
				// At tail - verify it matches
				if (curr != this->_tail) {
					return false;
				}
				break;
			}
			
			// Lock next before releasing current (hand-over-hand)
			auto nextLock = std::make_unique<std::shared_lock<std::shared_mutex>>(next->_mtx);
			currLock.reset();
			
			currLock = std::move(nextLock);
			prev = curr;
			curr = next;
		}
		
		// Check size matches count
		if (count != this->_size) {
			return false;
		}
		
		return true;
	}

	/**
	 * Clears all elements from the list.
	 * Hand-over-hand locks during traversal to deallocate nodes.
	 */
	void clear() override {
		if (this->_head == nullptr) {
			return;
		}
		
		// Hand-over-hand delete
		auto currLock = std::make_unique<std::unique_lock<std::shared_mutex>>(this->_head->_mtx);
		ListNode<T> *curr = this->_head;
		
		while (curr != nullptr) {
			ListNode<T> *next = curr->_next;
			
			if (next != nullptr) {
				auto nextLock = std::make_unique<std::unique_lock<std::shared_mutex>>(next->_mtx);
				currLock.reset();
				currLock = std::move(nextLock);
			} else {
				currLock.reset();
			}
			
			delete curr;
			curr = next;
		}
		
		this->_head = nullptr;
		this->_tail = nullptr;
		this->_size = 0;
	}
};

} // namespace cm
