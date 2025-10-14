/*
 * @file concurrent_list.cppm
 *
 * @author agge3, kpowkitty
 * @version 3.0
 * @since 2024-09-21
 *
 * IConcurrentList base class.
 * C++23 Module implementation for concurrent list with multiple locking strategies.
 */
export module concurrent_list;

// Standard library includes
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>

/**
* @namespace cm
* Namespace for CacheManager-specific packages.
*/
export namespace cm {

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

// Forward declarations
template <typename T>
class IConcurrentList;
template <typename T>
std::ostream& operator<<(std::ostream&, const IConcurrentList<T>&);

/**
* @class IConcurrentList<T>
* Abstract base class for concurrent list implementations.
* Template method pattern: defines algorithm structure, strategies define lock behavior.
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

	// Copy/move operations
	IConcurrentList(const IConcurrentList<T>& other);
	IConcurrentList(IConcurrentList<T>&& other) noexcept;
	IConcurrentList<T>& operator=(const IConcurrentList<T>& rhs);
	IConcurrentList<T>& operator=(IConcurrentList<T>&& rhs) noexcept;

	friend class ListIterator<T>;
	friend std::ostream& operator<< <>(std::ostream& out, const IConcurrentList<T>& list);

	// ==================== Public Interface Methods (Virtual) ====================

	/**
	 * Inserts an element into the list.
	 */
	virtual bool insert(const T& element) = 0;

	/**
	 * Removes an element from the list.
	 */
	virtual bool remove(const T& element) = 0;

	/**
	 * Removes a ListNode from the list.
	 *
	 * @param const ListNode<T>* ptr The node to remove.
	 *
	 * @return bool TRUE if removed successfully; FALSE if node not found or invalid.
	 */
	virtual bool remove(const ListNode<T>* ptr) = 0;

	/**
	 * Checks if the list contains an element.
	 */
	virtual bool contains(const T& element) const = 0;

	/**
	 * Checks if the list contains a ListNode.
	 */
	virtual bool contains(const ListNode<T> *ptr) const = 0;

	/**
	 * Removes a ListNode and pushes it to the front of the list.
	 * Specialized for LRU cache policy - only rearranges pointers.
	 */
	virtual bool removeAndPushFront(const ListNode<T> *ptr) = 0;

	/**
	 * Returns the size of the list.
	 */
	virtual std::size_t size() const = 0;

	/**
	 * Checks if the list is empty.
	 */
	virtual bool isEmpty() const = 0;

	/**
	 * Unlinks a node from the list.
	 */
	virtual bool unlink(const ListNode<T> *node) = 0;

	/**
	 * Validates list integrity after threaded manipulation.
	 */
	virtual bool isConsistent() const = 0;

	/**
	 * Clears all elements from the list.
	 */
	virtual void clear() = 0;

	// ==================== Public Convenience Methods (Non-Virtual) ====================

	std::optional<T> front() const;
	std::optional<T> back() const;
	std::optional<T> popFront();
	std::optional<T> popBack();
	const ListNode<T>* pushFront(const T& element);
	const ListNode<T>* pushBack(const T& element);
	std::optional<T> get(const ListNode<T> *ptr);
	const ListNode<T>* get(const T& element);
	ListIterator<T> unsafeBegin() const;
	ListIterator<T> unsafeEnd() const;
	std::size_t unsafeSize() const;

protected:
	// ==================== Protected Helper Methods ====================

	/**
	 * Searches for an element and returns the node containing it.
	 * Unsafe - no locking.
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
	 */
	const ListNode<T>* insertHelper(const T& element) {
		ListNode<T> *newNode = new ListNode<T>(element);
		link(newNode);
		return newNode;
	}

	/**
	 * Helper for remove() - performs the actual removal.
	 * Unsafe - no locking, assumes caller has acquired necessary locks.
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
	 * Defers physical deallocation until no threads hold references to it.
	 */
	void retireNode(ListNode<T>* node) {
    	std::lock_guard<std::mutex> g(_retireMutex);
    	_retired.push_back(node);
    	
		if (_retired.size() > 1024) {
    	    for (auto* n : _retired) {
				delete n;
			}
    	    _retired.clear();
    	}
	}

	/**
	 * Unlinks a node from the list.
	 * Unsafe - no locking, assumes caller has acquired necessary locks.
	 */
	bool unlinkImpl(ListNode<T>* node) {
	    if (!node) return false;
	
	    if (node->_prev) node->_prev->_next = node->_next;
	    else this->_head = node->_next;
	
	    if (node->_next) node->_next->_prev = node->_prev;
	    else this->_tail = node->_prev;
	
	    this->_size--;
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
	 * Validates forward traversal and backward links.
	 */
	bool validateStructure() const {
		if ((this->_head == nullptr) != (this->_tail == nullptr)) {
			return false;
		}
		if (this->_size == 0 && (this->_head != nullptr || this->_tail != nullptr)) {
			return false;
		}

		std::size_t count = 0;
		ListNode<T> *curr = this->_head;
		ListNode<T> *prev = nullptr;
		
		while (curr != nullptr) {
			count++;
			if (curr->_prev != prev) {
				return false;
			}
			prev = curr;
			curr = curr->_next;
		}
		
		if (prev != this->_tail || count != this->_size) {
			return false;
		}
		return true;
	}

	void copyCallingListEmpty(const IConcurrentList<T>& other);
	void copyListsSameLength(const IConcurrentList<T>& other);
	void copyCallingListLonger(const IConcurrentList<T>& other);
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
 * Concurrent list with coarse-grained locking.
 * Acquires a single lock for the entire list on any operation.
 */
export template <typename T>
class CoarseGrainedList : public cm::IConcurrentList<T> {
public:
	CoarseGrainedList() : cm::IConcurrentList<T>() {}
	virtual ~CoarseGrainedList() {}

	bool insert(const T& element) override {
		std::unique_lock<std::shared_mutex> lock(this->_mutex);
		if (this->search(element) != nullptr) {
			return false;
		}
		this->insertHelper(element);
		return true;
	}

	bool remove(const T& element) override {
		std::unique_lock<std::shared_mutex> lock(this->_mutex);
		return this->removeHelper(element);
	}

	bool remove(const ListNode<T>* ptr) override {
		std::unique_lock<std::shared_mutex> lock(this->_mutex);
		ListNode<T>* node = const_cast<ListNode<T>*>(ptr);
		
		// Verify node is actually in the list
		ListNode<T>* curr = this->_head;
		while (curr && curr != node) {
			curr = curr->_next;
		}
		
		if (!curr) return false;  // Node not found
		
		return this->unlinkImpl(node);
	}

	bool contains(const T& element) const override {
		std::shared_lock<std::shared_mutex> lock(this->_mutex);
		return this->search(element) != nullptr;
	}

	bool contains(const ListNode<T> *ptr) const override {
		std::shared_lock<std::shared_mutex> lock(this->_mutex);
		for (ListNode<T> *curr = this->_head; curr != nullptr; curr = curr->_next) {
			if (curr == ptr) return true;
		}
		return false;
	}

	bool removeAndPushFront(const ListNode<T> *ptr) override {
		std::unique_lock<std::shared_mutex> lock(this->_mutex);
		ListNode<T> *node = const_cast<ListNode<T>*>(ptr);
		
		// Verify in list
		ListNode<T> *curr = this->_head;
		while (curr && curr != node) curr = curr->_next;
		if (!curr) return false;
		
		// Unlink and relink at front
		if (!this->unlinkImpl(node)) return false;
		node->_next = this->_head;
		node->_prev = nullptr;
		if (this->_head) this->_head->_prev = node;
		else this->_tail = node;
		this->_head = node;
		return true;
	}

	std::size_t size() const override {
		std::shared_lock<std::shared_mutex> lock(this->_mutex);
		return this->_size;
	}

	bool isEmpty() const override {
		std::shared_lock<std::shared_mutex> lock(this->_mutex);
		return this->_size == 0;
	}

	bool unlink(const ListNode<T> *node) override {
		std::unique_lock<std::shared_mutex> lock(this->_mutex);
		return this->unlinkImpl(const_cast<ListNode<T>*>(node));
	}

	bool isConsistent() const override {
		std::shared_lock<std::shared_mutex> lock(this->_mutex);
		return this->validateStructure();
	}

	void clear() override {
		std::unique_lock<std::shared_mutex> lock(this->_mutex);
		this->unsafeClear();
	}
};

/**
 * @class GroupedFineGrainedList<T>
 * Concurrent list with grouped fine-grained locking.
 * Locks a small window (prev, curr, next) around mutation points.
 */
export template <typename T>
class GroupedFineGrainedList : public cm::IConcurrentList<T> {
public:
	GroupedFineGrainedList() : cm::IConcurrentList<T>() {}
	virtual ~GroupedFineGrainedList() {}

	bool insert(const T& element) override {
		{
			std::shared_lock<std::shared_mutex> lock(this->_mutex);
			if (this->search(element) != nullptr) return false;
		}
		
		std::unique_lock<std::shared_mutex> lock(this->_mutex);
		ListNode<T> *tail = this->_tail;
		
		std::unique_ptr<std::unique_lock<std::shared_mutex>> tailLock;
		std::unique_ptr<std::unique_lock<std::shared_mutex>> prevLock;
		
		if (tail) tailLock = std::make_unique<std::unique_lock<std::shared_mutex>>(tail->_mtx);
		if (tail && tail->_prev) prevLock = std::make_unique<std::unique_lock<std::shared_mutex>>(tail->_prev->_mtx);
		
		lock.unlock();
		
		if (this->search(element) != nullptr) return false;
		this->insertHelper(element);
		return true;
	}

	bool remove(const T& element) override {
		std::shared_lock<std::shared_mutex> lock(this->_mutex);
		ListNode<T> *curr = this->_head;
		while (curr && curr->_ele != element) curr = curr->_next;
		if (!curr) return false;
		
		ListNode<T> *prev = curr->_prev;
		ListNode<T> *next = curr->_next;
		
		std::unique_ptr<std::unique_lock<std::shared_mutex>> prevLock;
		std::unique_ptr<std::unique_lock<std::shared_mutex>> currLock;
		std::unique_ptr<std::unique_lock<std::shared_mutex>> nextLock;
		
		if (prev) prevLock = std::make_unique<std::unique_lock<std::shared_mutex>>(prev->_mtx);
		currLock = std::make_unique<std::unique_lock<std::shared_mutex>>(curr->_mtx);
		if (next) nextLock = std::make_unique<std::unique_lock<std::shared_mutex>>(next->_mtx);
		
		lock.unlock();
		
		if (curr->_ele != element) return false;
		return this->removeHelper(element);
	}

	bool remove(const ListNode<T>* ptr) override {
		std::unique_lock<std::shared_mutex> lock(this->_mutex);
		ListNode<T>* node = const_cast<ListNode<T>*>(ptr);
		
		// Verify node is in list
		ListNode<T>* curr = this->_head;
		while (curr && curr != node) curr = curr->_next;
		if (!curr) return false;
		
		ListNode<T> *prev = node->_prev;
		ListNode<T> *next = node->_next;
		
		std::unique_ptr<std::unique_lock<std::shared_mutex>> prevLock;
		std::unique_ptr<std::unique_lock<std::shared_mutex>> currLock;
		std::unique_ptr<std::unique_lock<std::shared_mutex>> nextLock;
		
		if (prev) prevLock = std::make_unique<std::unique_lock<std::shared_mutex>>(prev->_mtx);
		currLock = std::make_unique<std::unique_lock<std::shared_mutex>>(node->_mtx);
		if (next) nextLock = std::make_unique<std::unique_lock<std::shared_mutex>>(next->_mtx);
		
		lock.unlock();
		
		return this->unlinkImpl(node);
	}
	
	bool remove(const ListNode<T>* ptr) override {
		if (!this->_head) return false;
		
		ListNode<T>* node = const_cast<ListNode<T>*>(ptr);
		auto currLock = std::make_unique<std::unique_lock<std::shared_mutex>>(this->_head->_mtx);
		ListNode<T>* curr = this->_head;
		ListNode<T>* prev = nullptr;
		
		// Hand-over-hand search to find the node
		while (curr) {
			if (curr == node) break;
			
			ListNode<T>* next = curr->_next;
			if (!next) return false;
			
			auto nextLock = std::make_unique<std::unique_lock<std::shared_mutex>>(next->_mtx);
			currLock.reset();
			prev = curr;
			currLock = std::move(nextLock);
			curr = next;
		}
		
		if (!curr) return false;
		
		// Lock neighbors
		std::unique_ptr<std::unique_lock<std::shared_mutex>> prevLock;
		std::unique_ptr<std::unique_lock<std::shared_mutex>> nextLock;
		
		ListNode<T>* next = curr->_next;
		if (prev) prevLock = std::make_unique<std::unique_lock<std::shared_mutex>>(prev->_mtx);
		if (next) nextLock = std::make_unique<std::unique_lock<std::shared_mutex>>(next->_mtx);
		
		bool success = this->unlinkImpl(curr);
		currLock.reset();
		if (success) this->retireNode(curr);
		return success;
	}
	
	bool contains(const T& element) const override {
		std::shared_lock<std::shared_mutex> lock(this->_mutex);
		ListNode<T> *curr = this->_head;
		while (curr) {
			std::shared_lock<std::shared_mutex> currLock(curr->_mtx);
			if (curr->_ele == element) return true;
			curr = curr->_next;
		}
		return false;
	}

	bool contains(const ListNode<T> *ptr) const override {
		std::shared_lock<std::shared_mutex> lock(this->_mutex);
		ListNode<T> *curr = this->_head;
		while (curr) {
			std::shared_lock<std::shared_mutex> currLock(curr->_mtx);
			if (curr == ptr) return true;
			curr = curr->_next;
		}
		return false;
	}

	bool removeAndPushFront(const ListNode<T> *ptr) override {
		std::unique_lock<std::shared_mutex> lock(this->_mutex);
		ListNode<T> *node = const_cast<ListNode<T>*>(ptr);
		
		ListNode<T> *curr = this->_head;
		while (curr && curr != node) curr = curr->_next;
		if (!curr) return false;
		
		ListNode<T> *prev = node->_prev;
		ListNode<T> *next = node->_next;
		ListNode<T> *oldHead = this->_head;
		
		std::unique_ptr<std::unique_lock<std::shared_mutex>> prevLock;
		std::unique_ptr<std::unique_lock<std::shared_mutex>> nodeLock;
		std::unique_ptr<std::unique_lock<std::shared_mutex>> nextLock;
		std::unique_ptr<std::unique_lock<std::shared_mutex>> headLock;
		
		if (prev) prevLock = std::make_unique<std::unique_lock<std::shared_mutex>>(prev->_mtx);
		nodeLock = std::make_unique<std::unique_lock<std::shared_mutex>>(node->_mtx);
		if (next) nextLock = std::make_unique<std::unique_lock<std::shared_mutex>>(next->_mtx);
		if (oldHead) headLock = std::make_unique<std::unique_lock<std::shared_mutex>>(oldHead->_mtx);
		
		lock.unlock();
		
		if (!this->unlinkImpl(node)) return false;
		node->_next = oldHead;
		node->_prev = nullptr;
		if (oldHead) oldHead->_prev = node;
		this->_head = node;
		return true;
	}

	std::size_t size() const override {
		std::shared_lock<std::shared_mutex> lock(this->_mutex);
		return this->_size;
	}

	bool isEmpty() const override {
		std::shared_lock<std::shared_mutex> lock(this->_mutex);
		return this->_size == 0;
	}

	bool unlink(const ListNode<T> *node) override {
		std::unique_lock<std::shared_mutex> lock(this->_mutex);
		ListNode<T> *curr = const_cast<ListNode<T>*>(node);
		
		ListNode<T> *traverser = this->_head;
		while (traverser && traverser != curr) traverser = traverser->_next;
		if (!traverser) return false;
		
		ListNode<T> *prev = curr->_prev;
		ListNode<T> *next = curr->_next;
		
		std::unique_ptr<std::unique_lock<std::shared_mutex>> prevLock;
		std::unique_ptr<std::unique_lock<std::shared_mutex>> currLock;
		std::unique_ptr<std::unique_lock<std::shared_mutex>> nextLock;
		
		if (prev) prevLock = std::make_unique<std::unique_lock<std::shared_mutex>>(prev->_mtx);
		currLock = std::make_unique<std::unique_lock<std::shared_mutex>>(curr->_mtx);
		if (next) nextLock = std::make_unique<std::unique_lock<std::shared_mutex>>(next->_mtx);
		
		lock.unlock();
		return this->unlinkImpl(curr);
	}

	bool isConsistent() const override {
		std::unique_lock<std::shared_mutex> lock(this->_mutex);
		if ((this->_head == nullptr) != (this->_tail == nullptr)) return false;
		if (this->_size == 0 && (this->_head || this->_tail)) return false;
		
		std::size_t count = 0;
		ListNode<T> *curr = this->_head;
		ListNode<T> *prev = nullptr;
		
		while (curr) {
			std::shared_lock<std::shared_mutex> currLock(curr->_mtx);
			count++;
			if (curr->_prev != prev) return false;
			prev = curr;
			curr = curr->_next;
		}
		
		return prev == this->_tail && count == this->_size;
	}

	void clear() override {
		std::unique_lock<std::shared_mutex> lock(this->_mutex);
		this->unsafeClear();
	}
};

/**
 * @class HandOverHandList<T>
 * Concurrent list with hand-over-hand (lock-coupling) locking.
 * Holds minimal locks during traversal by locking next before releasing current.
 */
export template <typename T>
class HandOverHandList : public cm::IConcurrentList<T> {
public:
	HandOverHandList() : cm::IConcurrentList<T>() {}
	virtual ~HandOverHandList() {}

	bool insert(const T& element) override {
		if (!this->_head) {
			std::unique_lock<std::shared_mutex> lock(this->_mutex);
			if (!this->_head) {
				this->insertHelper(element);
				return true;
			}
		}
		
		auto currLock = std::make_unique<std::unique_lock<std::shared_mutex>>(this->_head->_mtx);
		ListNode<T> *curr = this->_head;
		
		while (curr && curr->_ele < element) {
			ListNode<T> *next = curr->_next;
			if (!next) break;
			
			auto nextLock = std::make_unique<std::unique_lock<std::shared_mutex>>(next->_mtx);
			currLock.reset();
			currLock = std::move(nextLock);
			curr = next;
		}
		
		if (curr && curr->_ele == element) return false;
		this->insertHelper(element);
		return true;
	}

	bool remove(const T& element) override {
		if (!this->_head) return false;
		
		auto currLock = std::make_unique<std::unique_lock<std::shared_mutex>>(this->_head->_mtx);
		ListNode<T>* curr = this->_head;
		ListNode<T>* prev = nullptr;
		
		while (curr) {
			if (curr->_ele == element) {
				std::unique_ptr<std::unique_lock<std::shared_mutex>> prevLock;
				std::unique_ptr<std::unique_lock<std::shared_mutex>> nextLock;
				
				ListNode<T>* next = curr->_next;
				if (prev) prevLock = std::make_unique<std::unique_lock<std::shared_mutex>>(prev->_mtx);
				if (next) nextLock = std::make_unique<std::unique_lock<std::shared_mutex>>(next->_mtx);
				
				bool success = this->unlinkImpl(curr);
				currLock.reset();
				if (success) this->retireNode(curr);
				return success;
			}
			
			ListNode<T>* next = curr->_next;
			if (!next) return false;
			
			auto nextLock = std::make_unique<std::unique_lock<std::shared_mutex>>(next->_mtx);
			currLock.reset();
			prev = curr;
			currLock = std::move(nextLock);
			curr = next;
		}
		return false;
	}

	bool contains(const T& element) const override {
		if (!this->_head) return false;
		auto currLock = std::make_unique<std::shared_lock<std::shared_mutex>>(this->_head->_mtx);
		ListNode<T> *curr = this->_head;
		
		while (curr) {
			if (curr->_ele == element) return true;
			ListNode<T> *next = curr->_next;
			if (!next) return false;
			
			auto nextLock = std::make_unique<std::shared_lock<std::shared_mutex>>(next->_mtx);
			currLock.reset();
			currLock = std::move(nextLock);
			curr = next;
		}
		return false;
	}

	bool contains(const ListNode<T> *ptr) const override {
		if (!this->_head) return false;
		auto currLock = std::make_unique<std::shared_lock<std::shared_mutex>>(this->_head->_mtx);
		ListNode<T> *curr = this->_head;
		
		while (curr) {
			if (curr == ptr) return true;
			ListNode<T> *next = curr->_next;
			if (!next) return false;
			
			auto nextLock = std::make_unique<std::shared_lock<std::shared_mutex>>(next->_mtx);
			currLock.reset();
			currLock = std::move(nextLock);
			curr = next;
		}
		return false;
	}

	bool removeAndPushFront(const ListNode<T> *ptr) override {
		if (!this->_head) return false;
		ListNode<T> *node = const_cast<ListNode<T>*>(ptr);
		
		auto currLock = std::make_unique<std::unique_lock<std::shared_mutex>>(this->_head->_mtx);
		ListNode<T> *curr = this->_head;
		
		while (curr) {
			if (curr == node) break;
			ListNode<T> *next = curr->_next;
			if (!next) return false;
			
			auto nextLock = std::make_unique<std::unique_lock<std::shared_mutex>>(next->_mtx);
			currLock.reset();
			currLock = std::move(nextLock);
			curr = next;
		}
		
		if (!curr) return false;
		
		ListNode<T> *prev = node->_prev;
		ListNode<T> *next = node->_next;
		
		std::unique_ptr<std::unique_lock<std::shared_mutex>> prevLock;
		std::unique_ptr<std::unique_lock<std::shared_mutex>> nextLock;
		if (prev) prevLock = std::make_unique<std::unique_lock<std::shared_mutex>>(prev->_mtx);
		if (next) nextLock = std::make_unique<std::unique_lock<std::shared_mutex>>(next->_mtx);
		
		if (!this->unlinkImpl(node)) return false;
		
		auto oldHeadLock = std::make_unique<std::unique_lock<std::shared_mutex>>(this->_head->_mtx);
		node->_next = this->_head;
		node->_prev = nullptr;
		this->_head->_prev = node;
		this->_head = node;
		return true;
	}

	std::size_t size() const override {
		if (!this->_head) return 0;
		std::size_t count = 0;
		auto currLock = std::make_unique<std::shared_lock<std::shared_mutex>>(this->_head->_mtx);
		ListNode<T> *curr = this->_head;
		
		while (curr) {
			count++;
			ListNode<T> *next = curr->_next;
			if (!next) break;
			
			auto nextLock = std::make_unique<std::shared_lock<std::shared_mutex>>(next->_mtx);
			currLock.reset();
			currLock = std::move(nextLock);
			curr = next;
		}
		return count;
	}

	bool isEmpty() const override {
		return !this->_head;
	}

	bool unlink(const ListNode<T> *node) override {
		if (!this->_head) return false;
		ListNode<T> *curr = const_cast<ListNode<T>*>(node);
		
		auto currLock = std::make_unique<std::unique_lock<std::shared_mutex>>(this->_head->_mtx);
		ListNode<T> *traverser = this->_head;
		
		while (traverser) {
			if (traverser == curr) break;
			ListNode<T> *next = traverser->_next;
			if (!next) return false;
			
			auto nextLock = std::make_unique<std::unique_lock<std::shared_mutex>>(next->_mtx);
			currLock.reset();
			currLock = std::move(nextLock);
			traverser = next;
		}
		
		if (!traverser) return false;
		
		ListNode<T> *prev = curr->_prev;
		ListNode<T> *next = curr->_next;
		
		std::unique_ptr<std::unique_lock<std::shared_mutex>> prevLock;
		std::unique_ptr<std::unique_lock<std::shared_mutex>> nextLock;
		
		if (prev) prevLock = std::make_unique<std::unique_lock<std::shared_mutex>>(prev->_mtx);
		if (next) nextLock = std::make_unique<std::unique_lock<std::shared_mutex>>(next->_mtx);
		
		bool success = this->unlinkImpl(curr);
		if (success) this->retireNode(curr);
		return success;
	}

	bool isConsistent() const override {
		if (!this->_head) {
			return !this->_tail && this->_size == 0;
		}
		
		std::size_t count = 0;
		ListNode<T> *prev = nullptr;
		auto currLock = std::make_unique<std::shared_lock<std::shared_mutex>>(this->_head->_mtx);
		ListNode<T> *curr = this->_head;
		
		while (curr) {
			count++;
			if (curr->_prev != prev) return false;
			
			ListNode<T> *next = curr->_next;
			if (!next) {
				if (curr != this->_tail) return false;
				break;
			}
			
			auto nextLock = std::make_unique<std::shared_lock<std::shared_mutex>>(next->_mtx);
			currLock.reset();
			currLock = std::move(nextLock);
			prev = curr;
			curr = next;
		}
		
		return count == this->_size;
	}

	void clear() override {
		if (!this->_head) return;
		
		auto currLock = std::make_unique<std::unique_lock<std::shared_mutex>>(this->_head->_mtx);
		ListNode<T> *curr = this->_head;
		
		while (curr) {
			ListNode<T> *next = curr->_next;
			
			if (next) {
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
