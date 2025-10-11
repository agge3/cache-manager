/*
 * @file concurrent-list-handoff.hpp
 *
 * @author agge3, kpowkitty
 * @version 1.0
 * @since 2024-09-21
 *
 * HandOverHandList - Concurrent list with hand-over-hand locking.
 * Acquires lock on current node, then next node, then releases current before moving.
 */

#pragma once

#include "concurrent-list-base.hpp"
#include <shared_mutex>
#include <memory>

namespace cm {

/**
 * @class HandOverHandList<T>
 * 
 * Concurrent list implementation using hand-over-hand (lock-coupling) locking.
 * During traversal, locks the current node, then locks the next node, then
 * releases the current node. This ensures no gaps in coverage and prevents
 * other threads from modifying the list structure during traversal.
 * Maximizes concurrency by holding minimal locks at any time.
 */
template <typename T>
class HandOverHandList : public ConcurrentList<T> {
public:
	/**
	 * Default constructor.
	 */
	HandOverHandList() : ConcurrentList<T>() {}

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
