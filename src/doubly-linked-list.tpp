/**
 * @file doubly-linked-list.tpp
 * @class DoublyLinkedList
 *
 * @author Tyler Baxter
 * @version 1.0
 * @since 2024-08-30
 *
 * DoublyLinkedList implementation.
 */

#include "linked-list.h"

#include <iostream>
#include <exception>

using namespace csc;

template <typename T>
DoublyLinkedList<T>::DoublyLinkedList(const DoublyLinkedList<T>& para)
{
    // Check if list to be copied has any nodes.
    if (!para.empty()) {
		copy_calling_list_empty(para);
	}
}

template <typename T>
DoublyLinkedList<T>::DoublyLinkedList(DoublyLinkedList<T>&& para) noexcept :
	// Steal the r-value list's resources.
	_head(para._head), _tail(para._tail), _count(para._count)
{
	// NULL the r-value list.
	para._head = nullptr;
	para._tail = nullptr;
	para._count = 0;

	// xxx microsoft(r) recommendation
	//*this = std::move(para);
}

template <typename T>
DoublyLinkedList<T>::~DoublyLinkedList()
{
	clear();
}

template <typename T>
DoublyLinkedList<T>& DoublyLinkedList<T>::operator=(const DoublyLinkedList<T>& rhs)
{
	// Check if both lists have the same address; then they're the same.
    if (&rhs == this) {
		std::cerr << "Attempted assignment to self.";
		// Return out.
		return *this;
    }

	// We've proceeded, the lists have different addresses and are distinct.
    // We don't need to check if _count is the same; not random access!
    if (rhs.empty()) {
        this->clear();
    }
    else if (this->empty()) {
        this->copy_calling_list_empty(rhs);
    }
    else if (this->_count == rhs._count) {
        this->copy_lists_same_length(rhs);
    }
    else if (this->_count > rhs._count) {
        this->copy_calling_list_longer(rhs);
    }
    else if (this->_count < rhs._count) {
        this->copy_calling_list_longer(rhs);
    }

    // Return calling list.
    return *this;
}

template <typename T>
DoublyLinkedList<T>& DoublyLinkedList<T>::operator=(DoublyLinkedList<T>&& rhs) noexcept
{
	// Check for self-assignment.
	if (this != &rhs) {
		// Release the l-value list's resources.
		clear();
		// Steal the r-value list's resources.
		_head = rhs._head;
		_tail = rhs._tail;
		_count = rhs._count;
		// NULL the r-value list.
		rhs._head = nullptr;
		rhs._tail = nullptr;
		rhs._count = 0;
	}
	// Return l-value list.
	return *this;
}

template <typename T>
void DoublyLinkedList<T>::copy_calling_list_empty(const DoublyLinkedList<T>& para) {
   // It's assumed calling object is empty, so we don't need to check.
   // Assign caller _count as parameter _count.
   _count = para._count;
   // Create _head for caller.
   _head = new DLLNode<T>(para._head->getElement());
   // curr at _head, para_curr at para _head
   DLLNode<T>* curr = _head;
   DLLNode<T>* para_curr = para._head;
   // Loop through all parameter list nodes and create for caller list.
   for (int i = 1; i < _count; ++i) {
       para_curr = para_curr->getNext();
       curr->setNext(new DLLNode<T>(para_curr->getElement(), nullptr, curr));
       curr = curr->getNext();
   }
   _tail = curr;
   // Cleanup dangling pointers.
   curr = para_curr = nullptr;
}

template <typename T>
void DoublyLinkedList<T>::copy_lists_same_length(const DoublyLinkedList<T>& para) {
    DLLNode<T>* curr = _head;
    DLLNode<T>* para_curr = para._head;
    while (curr != nullptr) {
        curr->setElement(para_curr->getElement());
        curr = curr->getNext();
        para_curr = para_curr->getNext();
    }
   // Cleanup dangling pointers.
   curr = para_curr = nullptr;
}

template <typename T>
void DoublyLinkedList<T>::copy_calling_list_longer(const DoublyLinkedList<T>& para) {
    // Create curr for caller and parameter _head.
    DLLNode<T>* curr = _head;
    DLLNode<T>* para_curr = para._head;
    // Iterate through, stopping at _tail node of parameter.
    while (para_curr != nullptr) {
        curr->setElement(para_curr->getElement());
        if (para_curr->getNext() == nullptr) {
           _tail = curr;
           curr->setNext(nullptr);
        }
        curr = curr->getNext();
        para_curr = para_curr->getNext();
    }
    // curr at new _tail for caller.
    // Delete everything after...
    while (curr != nullptr) {
        delete curr;
        curr = curr->getNext();
    }
    // Cleanup: _count is equal, assign _tail, and delete dangling pointers.
    _count = para._count;
    curr = para_curr = nullptr;
}

template <typename T>
void DoublyLinkedList<T>::copy_calling_list_shorter(const DoublyLinkedList<T>& para) {
    DLLNode<T>* curr = _head;
    DLLNode<T>* para_curr = para._head;
    while (curr != nullptr) {
        curr->setElement(para_curr->getElement());
        curr = curr->getNext();
        para_curr = para_curr->getNext();
    }
    // Reset current to be at _tail.
    curr = _tail;
    // Second loop to create new nodes for remaining nodes of caller.
    while (para_curr != nullptr) {
        curr->setNext(new DLLNode<T>(para_curr->getElement(), nullptr, curr));
        curr = curr->getNext();
        para_curr = para_curr->getNext();
    }
    // New _tail is next of current node.
    _tail = curr;
    _tail->setNext(nullptr);
    _count = para._count;
    // Cleanup dangling pointers.
    curr = para_curr = nullptr;
}

template <typename T>
const DLLNode<T>* DoublyLinkedList<T>::begin() const
{
	// Return a const_cast pointer to the head node for read-only access.
	return _head;
}

template <typename T>
const DLLNode<T>* DoublyLinkedList<T>::end() const
{
	// Return a const_cast pointer to the tail node for read-only access.
	return _tail;
}

template <typename T>
bool DoublyLinkedList<T>::empty() const
{
	return _head == nullptr && _tail == nullptr && _count == 0;
}

template <typename T>
std::size_t DoublyLinkedList<T>::size() const
{
	return _count;
}

template <typename T>
T DoublyLinkedList<T>::front() const
{
	if (empty()) {
    	throw std::out_of_range(
			"Attempted to access the front element of an empty list.");
	}
	return _head->getElement();
}

template <typename T>
T DoublyLinkedList<T>::back() const
{
	if (empty()) {
    	throw std::out_of_range(
			"Attempted to access the back element of an empty list.");
	}
	return _tail->getElement();
}

template <typename T>
void DoublyLinkedList<T>::pushFront(T element)
{
	if (empty()) {
		_head = new DLLNode<T>(element);
		_tail = _head;
	} else {
		DLLNode<T>* ptr = new DLLNode<T>(element, _head, nullptr);
		_head->setPrev(ptr);
		_head = ptr;
		ptr = nullptr;
	}
	// Increment count, node has been added.
    ++_count;	
}

template <typename T>
T DoublyLinkedList<T>::popFront()
{
	if (empty()) {
		throw std::out_of_range("Attempted to pop an empty list.");
	}
	T ele = front();
	if (_head == _tail) {
		clear();
	} else {
		DLLNode<T>* ptr = _head->getNext();
		delete _head;
		_head = ptr;
		_head->setPrev(nullptr);
		--_count;
		ptr = nullptr;
	}
	return ele;
}

template <typename T>
void DoublyLinkedList<T>::push_back(T element)
{
	if (empty()) {
		_head = new DLLNode<T>(element);
		_tail = _head;
	} else {
		_tail->setNext(new DLLNode<T>(element, nullptr, _tail));
		_tail = _tail->getNext();
	}
    // Increment _count, node has been added.
    ++_count;
}

template <typename T>
T DoublyLinkedList<T>::popBack()
{
	if (empty()) {
		throw std::out_of_range("Attempted to pop an empty list.");
	}
	T ele = back();
	if (_head == _tail) {
		clear();
	} else {
	DLLNode<T>* ptr = _tail->getPrev();
	delete _tail;
	_tail = ptr;
	_tail->setNext(nullptr);
	--_count;
	ptr = nullptr;
	}
	return ele;
}

template <typename T>
bool DoublyLinkedList<T>::insert(T element, int index)
{
	// xxx rework the bool
	index_out_of_range(index);

	// Insert at _head.
	if (index == 0) {
		pushFront(element);
		return true;
	}

	// Insert at _tail.
	if (index == _count) {
		push_back(element);
		return true;
	}

	DLLNode<T>* curr = _head;
	int i = 0;
	// We want to stop one before the index we want to insert, so we can 
	// use setNext() to insert at that index.
	while (++i != index) {
		curr->getNext();
	}
	DLLNode<T>* curr_next = curr->getNext();
	curr->setNext(new DLLNode<T>(element, curr_next, curr));
	// Make sure curr_next isn't nullptr before trying to set prev for it.
	if (curr_next != nullptr) {
		curr_next->setPrev(curr->getNext());
	}
	++_count;
	curr = curr_next = nullptr;
	return true;
}

template <typename T>
void DoublyLinkedList<T>::index_out_of_range(int index) const
{
	if (index < 0) {
		throw std::out_of_range("Index cannot be negative");
	}
	if (index > _count) {
		throw std::out_of_range("Index out of range");
	}
}

template <typename T>
bool DoublyLinkedList<T>::remove(T element)
{
	// Only delete if list has nodes.
	if (empty()) {
		throw std::out_of_range("Attempted to remove from an empty list.");
	}
	// If node to delete is the head, don't loop.
	if (_head->getElement() == element) {
		DLLNode<T>* tmp = _head;
		_head = tmp->getNext();
		delete tmp;
		tmp = nullptr;
		--_count;
	} else {
		DLLNode<T>* curr = _head;
		while (curr->getNext() != nullptr && 
			curr->getNext()->getElement() != element) {
			curr = curr->getNext();
		}
		if (curr->getNext() == nullptr) {
			// At tail and element not found.
			return false;
		}
		DLLNode<T>* tmp = curr->getNext();
		curr->setNext(tmp->getNext());
		delete tmp;
		tmp = nullptr;
		--_count;
	}
	return true;
}

template <typename T>
void DoublyLinkedList<T>::print() const
{
	if (empty()) {
		std::cout << "Empty list\n";
	// Guard loop if there's only one element.
	} else if (_head == _tail) {
		std::cout << _head->getElement();
	} else {
		DLLNode<T>* curr = _head;
		while (curr != nullptr) {
			std::cout << curr->getElement() << ", ";
			curr = curr->getNext();
		}
		curr = nullptr;
	}
}

template <typename T>
T DoublyLinkedList<T>::get(int index) const
{
	index_out_of_range(index);
	// Check _head.
	if (index == 0) {
		return _head->getElement();
	}
	// Check _tail.
	if (index == _count) {
		return _tail->getElement();
	}
	// _head already checked, _tail already checked.
	DLLNode<T>* curr = _head->getNext();
	int i = 1;
	while (i++ != index) {
		curr = curr->getNext();
	}
	return curr->getElement();
}

template <typename T>
const DLLNode<T>* DoublyLinkedList<T>::find(T element) const
{
    // Guard if list is empty.
	if (empty()) {
		throw std::out_of_range("Attempted to get from an empty list.");
	}
	const DLLNode<T>* curr = _head;
	while (curr != nullptr) {
		if (curr->getElement() == element) {
			return curr;
   		}
		curr = curr->getNext();
	}
    return nullptr;
}

template <typename T>
bool DoublyLinkedList<T>::contains(T element) const
{
	return find(element) != nullptr;
}

template <typename T>
void DoublyLinkedList<T>::clear()
{
	if (!empty()) {
		DLLNode<T>* curr = _head;
		DLLNode<T>* curr_next;
		while (curr != nullptr) {
			curr_next = curr->getNext();
			delete curr;
			curr = curr_next;
		}
        _count = 0; // Set count = known number after while loop so 
                   // less calculations.
		_head = _tail = curr = curr_next = nullptr;
	}
}
