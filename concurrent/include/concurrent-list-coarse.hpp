/*
 * @file concurrent-list-coarse.hpp
 *
 * @author agge3, kpowkitty
 * @version 1.0
 * @since 2024-09-21
 *
 * CoarseGrainedList - Concurrent list with coarse-grained locking.
 * Acquires a single lock for the entire list on any mutation operation.
 */

#pragma once

#include "concurrent-list-base.hpp"
#include <shared_mutex>

namespace cm {

/**
 * @class CoarseGrainedList<T>
 * 
 * Concurrent list implementation using coarse-grained locking.
 * All operations that mutate or read the list acquire the single list-wide mutex.
 * Simple, but potentially lower concurrency than fine-grained strategies.
 */
template <typename T>
class CoarseGrainedList : public ConcurrentList<T> {
public:
	/**
	 * Default constructor.
	 */
	CoarseGrainedList() : ConcurrentList<T>() {}

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

} // namespace cm
