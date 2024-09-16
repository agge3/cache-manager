/**
 * @file linked-list.tpp
 * @class LinkedLinkedList<T>
 *
 * @author Tyler Baxter
 * @version 1.0
 * @since 2024-08-30
 *
 * LinkedLinkedList<T> implementation.
 */

#include "linked-list.h"

#include <iostream>
#include <exception>

using namespace csc;

template <typename T>
LinkedList<T>::LinkedList(const LinkedList<T>& para)
{
    // Check if list to be copied has any nodes.
    if (!para.empty()) {
		copy_calling_list_empty(para);
	}
}

template <typename T>
LinkedList<T>::LinkedList(LinkedList<T>&& para) noexcept :
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
LinkedList<T>::~LinkedList()
{
	clear();
}

template <typename T>
LinkedList<T>& LinkedList<T>::operator=(const LinkedList<T>& rhs)
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
LinkedList<T>& LinkedList<T>::operator=(LinkedList<T>&& rhs) noexcept
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
void LinkedList<T>::copy_calling_list_empty(const LinkedList<T>& para) {
   // It's assumed calling object is empty, so we don't need to check.
   // Assign caller _count as parameter _count.
   _count = para._count;
   // Create _head for caller.
   _head = new Node<T>(para._head->get_element());
   // curr at _head, para_curr at para _head
   Node<T>* curr = _head;
   Node<T>* para_curr = para._head;
   // Loop through all parameter list nodes and create for caller list.
   for (int i = 1; i < _count; ++i) {
       para_curr = para_curr->get_next();
       curr->set_next(new Node<T>(para_curr->get_element(), nullptr, curr));
       curr = curr->get_next();
   }
   _tail = curr;
   // Cleanup dangling pointers.
   curr = para_curr = nullptr;
}

template <typename T>
void LinkedList<T>::copy_lists_same_length(const LinkedList<T>& para) {
    Node<T>* curr = _head;
    Node<T>* para_curr = para._head;
    while (curr != nullptr) {
        curr->set_element(para_curr->get_element());
        curr = curr->get_next();
        para_curr = para_curr->get_next();
    }
   // Cleanup dangling pointers.
   curr = para_curr = nullptr;
}

template <typename T>
void LinkedList<T>::copy_calling_list_longer(const LinkedList<T>& para) {
    // Create curr for caller and parameter _head.
    Node<T>* curr = _head;
    Node<T>* para_curr = para._head;
    // Iterate through, stopping at _tail node of parameter.
    while (para_curr != nullptr) {
        curr->set_element(para_curr->get_element());
        if (para_curr->get_next() == nullptr) {
           _tail = curr;
           curr->set_next(nullptr);
        }
        curr = curr->get_next();
        para_curr = para_curr->get_next();
    }
    // curr at new _tail for caller.
    // Delete everything after...
    while (curr != nullptr) {
        delete curr;
        curr = curr->get_next();
    }
    // Cleanup: _count is equal, assign _tail, and delete dangling pointers.
    _count = para._count;
    curr = para_curr = nullptr;
}

template <typename T>
void LinkedList<T>::copy_calling_list_shorter(const LinkedList<T>& para) {
    Node<T>* curr = _head;
    Node<T>* para_curr = para._head;
    while (curr != nullptr) {
        curr->set_element(para_curr->get_element());
        curr = curr->get_next();
        para_curr = para_curr->get_next();
    }
    // Reset current to be at _tail.
    curr = _tail;
    // Second loop to create new nodes for remaining nodes of caller.
    while (para_curr != nullptr) {
        curr->set_next(new Node<T>(para_curr->get_element(), nullptr, curr));
        curr = curr->get_next();
        para_curr = para_curr->get_next();
    }
    // New _tail is next of current node.
    _tail = curr;
    _tail->set_next(nullptr);
    _count = para._count;
    // Cleanup dangling pointers.
    curr = para_curr = nullptr;
}

template <typename T>
const Node<T>* LinkedList<T>::begin() const
{
	// Return a const_cast pointer to the head node for read-only access.
	return _head;
}

template <typename T>
const Node<T>* LinkedList<T>::end() const
{
	// Return a const_cast pointer to the tail node for read-only access.
	return _tail;
}

template <typename T>
bool LinkedList<T>::empty() const
{
	return _head == nullptr && _tail == nullptr && _count == 0;
}

template <typename T>
std::size_t LinkedList<T>::size() const
{
	return _count;
}

template <typename T>
T LinkedList<T>::front() const
{
	if (empty()) {
    	throw std::out_of_range(
			"Attempted to access the front element of an empty list.");
	}
	return _head->get_element();
}

template <typename T>
T LinkedList<T>::back() const
{
	if (empty()) {
    	throw std::out_of_range(
			"Attempted to access the back element of an empty list.");
	}
	return _tail->get_element();
}

template <typename T>
void LinkedList<T>::push_front(T element)
{
	if (empty()) {
		_head = new Node<T>(element);
		_tail = _head;
	} else {
		Node<T>* ptr = new Node<T>(element, _head, nullptr);
		_head->set_prev(ptr);
		_head = ptr;
		ptr = nullptr;
	}
	// Increment count, node has been added.
    ++_count;
}

template <typename T>
T LinkedList<T>::pop_front()
{
	if (empty()) {
		throw std::out_of_range("Attempted to pop an empty list.");
	}
	T ele = front();
	if (_head == _tail) {
		clear();
	} else {
		Node<T>* ptr = _head->get_next();
		delete _head;
		_head = ptr;
		_head->set_prev(nullptr);
		--_count;
		ptr = nullptr;
	}
	return ele;
}

template <typename T>
void LinkedList<T>::push_back(T element)
{
	if (empty()) {
		_head = new Node<T>(element);
		_tail = _head;
	} else {
		_tail->set_next(new Node<T>(element, nullptr, _tail));
		_tail = _tail->get_next();
	}
    // Increment _count, node has been added.
    ++_count;
}

template <typename T>
T LinkedList<T>::pop_back()
{
	if (empty()) {
		throw std::out_of_range("Attempted to pop an empty list.");
	}
	T ele = back();
	if (_head == _tail) {
		clear();
	} else {
	Node<T>* ptr = _tail->get_prev();
	delete _tail;
	_tail = ptr;
	_tail->set_next(nullptr);
	--_count;
	ptr = nullptr;
	}
	return ele;
}

template <typename T>
bool LinkedList<T>::insert(T element, int index)
{
	// xxx rework the bool
	index_out_of_range(index);

	// Insert at _head.
	if (index == 0) {
		push_front(element);
		return true;
	}

	// Insert at _tail.
	if (index == _count) {
		push_back(element);
		return true;
	}

	Node<T>* curr = _head;
	int i = 0;
	// We want to stop one before the index we want to insert, so we can
	// use set_next() to insert at that index.
	while (++i != index) {
		curr->get_next();
	}
	Node<T>* curr_next = curr->get_next();
	curr->set_next(new Node<T>(element, curr_next, curr));
	// Make sure curr_next isn't nullptr before trying to set prev for it.
	if (curr_next != nullptr) {
		curr_next->set_prev(curr->get_next());
	}
	++_count;
	curr = curr_next = nullptr;
	return true;
}

template <typename T>
void LinkedList<T>::index_out_of_range(int index) const
{
	if (index < 0) {
		throw std::out_of_range("Index cannot be negative");
	}
	if (index > _count) {
		throw std::out_of_range("Index out of range");
	}
}

template <typename T>
bool LinkedList<T>::remove(T element)
{
	// Only delete if list has nodes.
	if (empty()) {
		throw std::out_of_range("Attempted to remove from an empty list.");
	}
	// If node to delete is the head, don't loop.
	if (_head->get_element() == element) {
		Node<T>* tmp = _head;
		_head = tmp->get_next();
		delete tmp;
		tmp = nullptr;
		--_count;
	} else {
		Node<T>* curr = _head;
		while (curr->get_next() != nullptr &&
			curr->get_next()->get_element() != element) {
			curr = curr->get_next();
		}
		if (curr->get_next() == nullptr) {
			// At tail and element not found.
			return false;
		}
		Node<T>* tmp = curr->get_next();
		curr->set_next(tmp->get_next());
		delete tmp;
		tmp = nullptr;
		--_count;
	}
	return true;
}

template <typename T>
void LinkedList<T>::print() const
{
	if (empty()) {
		std::cout << "Empty list\n";
	// Guard loop if there's only one element.
	} else if (_head == _tail) {
		std::cout << _head->get_element();
	} else {
		Node<T>* curr = _head;
		while (curr != nullptr) {
			std::cout << curr->get_element() << ", ";
			curr = curr->get_next();
		}
		curr = nullptr;
	}
}

template <typename T>
T LinkedList<T>::get(int index) const
{
	index_out_of_range(index);
	// Check _head.
	if (index == 0) {
		return _head->get_element();
	}
	// Check _tail.
	if (index == _count) {
		return _tail->get_element();
	}
	// _head already checked, _tail already checked.
	Node<T>* curr = _head->get_next();
	int i = 1;
	while (i++ != index) {
		curr = curr->get_next();
	}
	return curr->get_element();
}

template <typename T>
const Node<T>* LinkedList<T>::find(T element) const
{
    // Guard if list is empty.
	if (empty()) {
		throw std::out_of_range("Attempted to get from an empty list.");
	}
	const Node<T>* curr = _head;
	while (curr != nullptr) {
		if (curr->get_element() == element) {
			return curr;
   		}
		curr = curr->get_next();
	}
    return nullptr;
}

template <typename T>
bool LinkedList<T>::contains(T element) const
{
	return find(element) != nullptr;
}

template <typename T>
void LinkedList<T>::clear()
{
	if (!empty()) {
		Node<T>* curr = _head;
		Node<T>* curr_next;
		while (curr != nullptr) {
			curr_next = curr->get_next();
			delete curr;
			curr = curr_next;
		}
        _count = 0; // Set count = known number after while loop so
                   // less calculations.
		_head = _tail = curr = curr_next = nullptr;
	}
}
