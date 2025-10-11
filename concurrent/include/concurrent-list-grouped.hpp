/*
 * @file groupedfinegrained-list.hpp
 *
 * @author agge3
 * @version 1.0
 * @since 2024-09-21
 *
 * GroupedFineGrainedList - Concurrent list with grouped fine-grained locking.
 * Acquires locks on prev, curr, and next nodes during mutations.
 */

#pragma once

#include "concurrent-list-base.hpp"
#include <shared_mutex>
#include <memory>

namespace cm {

/**
 * @class GroupedFineGrainedList<T>
 * 
 * Concurrent list implementation using grouped fine-grained locking.
 * Locks a small window (prev, curr, next) around the mutation point.
 * Better concurrency than coarse-grained, but simpler than hand-over-hand.
 */
template <typename T>
class GroupedFineGrainedList : public ConcurrentList<T> {
public:
	/**
	 * Default constructor.
	 */
	GroupedFineGrainedList() : ConcurrentList<T>() {}

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
