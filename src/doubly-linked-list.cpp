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

#include "doubly-linked-list.h"

#include <iostream>

using namespace csc;

template <typename T>
DoublyLinkedList<T>::DoublyLinkedList(const DoublyLinkedList<T>& other)
{
    // Check if list to be copied has any nodes.
    if (!other.empty()) {
		copyCallingListEmpty(other);
	}
}

template <typename T>
DoublyLinkedList<T>::DoublyLinkedList(DoublyLinkedList<T>&& other) noexcept :
	// Steal the r-value list's resources.
	_head(other._head), _tail(other._tail), _size(other._size)
{
	// NULL the r-value list.
	other._head = nullptr;
	other._tail = nullptr;
	other._size = 0;

	// xxx microsoft(r) recommendation
	//*this = std::move(other);
}

template <typename T>
DoublyLinkedList<T>& DoublyLinkedList<T>::operator=(
	const DoublyLinkedList<T>& rhs)
{
	// Check if both lists have the same address; then they're the same.
    if (&rhs == this) {
		std::cerr << "Attempted assignment to self.";
		// Return out.
		return *this;
    }

	// We've proceeded, the lists have different addresses and are distinct.
    // We don't need to check if _size is the same; not random access!
    if (rhs.empty()) {
        this->clear();
    }
    else if (this->empty()) {
        this->copyCallingListEmpty(rhs);
    }
    else if (this->_size == rhs._size) {
        this->copyListsSameLength(rhs);
    }
    else if (this->_size > rhs._size) {
        this->copyCallingListLonger(rhs);
    }
    else if (this->_size < rhs._size) {
        this->copyCallingListLonger(rhs);
    }

    // Return calling list.
    return *this;
}

template <typename T>
DoublyLinkedList<T>& DoublyLinkedList<T>::operator=(
	DoublyLinkedList<T>&& rhs) noexcept
{
	// Check for self-assignment.
	if (this != &rhs) {
		// Release the l-value list's resources.
		clear();
		// Steal the r-value list's resources.
		_head = rhs._head;
		_tail = rhs._tail;
		_size = rhs._size;
		// NULL the r-value list.
		rhs._head = nullptr;
		rhs._tail = nullptr;
		rhs._size = 0;
	}
	// Return l-value list.
	return *this;
}

template <typename T>
void DoublyLinkedList<T>::copyCallingListEmpty(
	const DoublyLinkedList<T>& other) {
	// It's assumed calling object is empty, so we don't need to check.
	// Assign caller _size as other _size.
	_size = other._size;
	// Create _head for caller.
	_head = new DLLNode<T>(other._head->getElement());
	// curr at _head, otherCurr at other _head
	DLLNode<T>* curr = _head;
	DLLNode<T>* otherCurr = other._head;
	// Loop through all other list nodes and create for caller list.
	for (int i = 1; i < _size; ++i) {
	    otherCurr = otherCurr->getNext();
	    curr->setNext(new DLLNode<T>(otherCurr->getElement(), nullptr, curr));
	    curr = curr->getNext();
	}
	_tail = curr;
	// Cleanup dangling pointers.
	curr = otherCurr = nullptr;
}

template <typename T>
void DoublyLinkedList<T>::copyListsSameLength(
	const DoublyLinkedList<T>& other) 
{
	DLLNode<T> *curr = _head;
	DLLNode<T> *otherCurr = other._head;
	while (curr != nullptr) {
	    curr->setElement(otherCurr->getElement());
	    curr = curr->getNext();
	    otherCurr = otherCurr->getNext();
	}
	// Cleanup dangling pointers.
	curr = otherCurr = nullptr;
}

template <typename T>
void DoublyLinkedList<T>::copyCallingListLonger(
	const DoublyLinkedList<T>& other) 
{
    // Create curr for caller and other _head.
    DLLNode<T>* curr = _head;
    DLLNode<T>* otherCurr = other._head;
    // Iterate through, stopping at _tail node of other.
    while (otherCurr != nullptr) {
        curr->setElement(otherCurr->getElement());
        if (otherCurr->getNext() == nullptr) {
           _tail = curr;
           curr->setNext(nullptr);
        }
        curr = curr->getNext();
        otherCurr = otherCurr->getNext();
    }
    // curr at new _tail for caller.
    // Delete everything after...
    while (curr != nullptr) {
        delete curr;
        curr = curr->getNext();
    }
    // Cleanup: _size is equal, assign _tail, and delete dangling pointers.
    _size = other._size;
    curr = otherCurr = nullptr;
}

template <typename T>
void DoublyLinkedList<T>::copyCallingListShorter(
	const DoublyLinkedList<T>& other) 
{
    DLLNode<T>* curr = _head;
    DLLNode<T>* otherCurr = other._head;
    while (curr != nullptr) {
        curr->setElement(otherCurr->getElement());
        curr = curr->getNext();
        otherCurr = otherCurr->getNext();
    }
    // Reset current to be at _tail.
    curr = _tail;
    // Second loop to create new nodes for remaining nodes of caller.
    while (otherCurr != nullptr) {
        curr->setNext(new DLLNode<T>(otherCurr->getElement(), nullptr, curr));
        curr = curr->getNext();
        otherCurr = otherCurr->getNext();
    }
    // New _tail is next of current node.
    _tail = curr;
    _tail->setNext(nullptr);
    _size = other._size;
    // Cleanup dangling pointers.
    curr = otherCurr = nullptr;
}

//template <typename T>
//DLLIterator<T> DoublyLinkedList<T>::begin() const
//{
//	return DLLIterator<T>(_head);
//}
//
//template <typename T>
//DLLIterator<T> DoublyLinkedList<T>::end() const
//{
//	return DLLIterator<T>(nullptr);
//}

template <typename T>
bool DoublyLinkedList<T>::empty() const
{
	return _head == nullptr && _tail == nullptr && _size == 0;
}

template <typename T>
std::size_t DoublyLinkedList<T>::size() const
{
	return _size;
}

template <typename T>
std::optional<T> DoublyLinkedList<T>::front() const
{
	return !empty() ? std::optional<T>(_head->getElement()) : std::nullopt;
}

template <typename T>
std::optional<T> DoublyLinkedList<T>::back() const
{
	return !empty() ? std::optional<T>(_tail->getElement()) : std::nullopt;
}

template <typename T>
const DLLNode<T>* DoublyLinkedList<T>::pushFront(const T& element)
{
	if (empty()) {
		_head = new DLLNode<T>(element);
		_tail = _head;
	} else {
		DLLNode<T> *ptr = new DLLNode<T>(element, _head, nullptr);
		_head->setPrev(ptr);
		_head = ptr;
		ptr = nullptr;
	}
	// Increment size, node has been added.
    ++_size;	
	return _head;
}

template <typename T>
std::optional<T> DoublyLinkedList<T>::popFront()
{
	if (empty()) {
		return std::nullopt;
	}
	std::optional<T> ele = front();
	if (_head == _tail) {
		clear();
	} else {
		DLLNode<T> *ptr = _head->getNext();
		delete _head;
		_head = ptr;
		_head->setPrev(nullptr);
		--_size;
		ptr = nullptr;
	}
	return ele;
}

template <typename T>
const DLLNode<T>* DoublyLinkedList<T>::pushBack(const T& element)
{
	if (empty()) {
		_head = new DLLNode<T>(element);
		_tail = _head;
	} else {
		_tail->setNext(new DLLNode<T>(element, nullptr, _tail));
		_tail = _tail->getNext();
	}
    ++_size;	// Increment _size, node has been added.
	return _tail;
}

template <typename T>
std::optional<T> DoublyLinkedList<T>::popBack()
{
	if (empty()) {
		return std::nullopt;
	}
	std::optional<T> ele = back();
	if (_head == _tail) {
		clear();
	} else {
	DLLNode<T> *ptr = _tail->getPrev();
	delete _tail;
	_tail = ptr;
	_tail->setNext(nullptr);
	--_size;
	ptr = nullptr;
	}
	return ele;
}

//template <typename T>
//bool DoublyLinkedList<T>::insert(T element, int index)
//{
//	// xxx rework the bool
//	index_out_of_range(index);
//
//	// Insert at _head.
//	if (index == 0) {
//		pushFront(element);
//		return true;
//	}
//
//	// Insert at _tail.
//	if (index == _size) {
//		push_back(element);
//		return true;
//	}
//
//	DLLNode<T>* curr = _head;
//	int i = 0;
//	// We want to stop one before the index we want to insert, so we can 
//	// use setNext() to insert at that index.
//	while (++i != index) {
//		curr->getNext();
//	}
//	DLLNode<T>* currNext = curr->getNext();
//	curr->setNext(new DLLNode<T>(element, currNext, curr));
//	// Make sure currNext isn't nullptr before trying to set prev for it.
//	if (currNext != nullptr) {
//		currNext->setPrev(curr->getNext());
//	}
//	++_size;
//	curr = currNext = nullptr;
//	return true;
//}

template <typename T>
std::optional<T> DoublyLinkedList<T>::get(DLLNode<T> *node)
{
	return node ? std::optional<T>(node->getElement()) : std::nullopt;
}

template <typename T>
bool DoublyLinkedList<T>::remove(const T& element)
{
	// Only delete if list has nodes.
	if (empty()) {
		return false;
	}
	// If node to delete is the head, don't loop.
	if (_head->getElement() == element) {
		DLLNode<T>* tmp = _head;
		_head = tmp->getNext();
		delete tmp;
		tmp = nullptr;
		--_size;
	} else {
		DLLNode<T>* curr = _head;
		// xxx avoid function calls in a loop parameter
		while (curr->getNext() && curr->getNext()->getElement() != element) {
			curr = curr->getNext();
		}
		if (!curr->getNext()) {
			// At tail and element not found.
			return false;
		}
		DLLNode<T>* tmp = curr->getNext();
		curr->setNext(tmp->getNext());
		delete tmp;
		tmp = nullptr;
		--_size;
	}
	return true;
}

template <typename T>
bool DoublyLinkedList<T>::removeAndPushFront(const DLLNode<T> *ptr)
{
	// Empty list or nullptr node.
	if (!ptr || empty()) {
		return false;
	}

	// Cast away the client's const, we're in our owned instance.
	DLLNode<T> *node = const_cast<DLLNode<T>*>(ptr);

	// Handle head and tail.
	if (node == _head) {
		// Do nothing, but return TRUE for success.
		return true;
	} else if (node == _tail) {
		// `else if` will trap us in a condition where `size() > 1`.
		_tail->getPrev()->setNext(nullptr);
		_tail = _tail->getPrev();
		_head->setPrev(node);
		node->setPrev(nullptr);
		node->setNext(_head);
		_head = node;
		return true;
	}

	// General case:
	node->getPrev()->setNext(node->getNext());
	node->getNext()->setPrev(node->getPrev());
	_head->setPrev(node);
	node->setNext(_head);
	_head = node;
	return true;
}

//template <typename T>
//void DoublyLinkedList<T>::print() const
//{
//	if (empty()) {
//		std::cout << "Empty list\n";
//	// Guard loop if there's only one element.
//	} else if (_head == _tail) {
//		std::cout << _head->getElement();
//	} else {
//		DLLNode<T>* curr = _head;
//		while (curr != nullptr) {
//			std::cout << curr->getElement() << ", ";
//			curr = curr->getNext();
//		}
//		curr = nullptr;
//	}
//}

template <typename T>
const DLLNode<T>* DoublyLinkedList<T>::search(const T& element) const
{
    // Guard if list is empty.
	if (empty()) {
		return nullptr;
	}
	const DLLNode<T> *curr = _head;
	while (curr) {
		if (curr->getElement() == element) {
			return curr;
   		}
		curr = curr->getNext();
	}
    return nullptr;
}

template <typename T>
bool DoublyLinkedList<T>::contains(const T& element) const
{
	return search(element);
}

template <typename T>
bool DoublyLinkedList<T>::contains(DLLNode<T> *node) const
{
	return node;
}

template <typename T>
void DoublyLinkedList<T>::clear()
{
	if (!empty()) {
		DLLNode<T>* curr = _head;
		DLLNode<T>* currNext;
		while (curr != nullptr) {
			currNext = curr->getNext();
			delete curr;
			curr = currNext;
		}
        _size = 0; // Set size = known number after while loop so 
                   // less calculations.
		_head = _tail = curr = currNext = nullptr;
	}
}
