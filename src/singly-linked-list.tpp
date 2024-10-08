/**
 * @file singly-linked-list.cpp
 * @class SinglyLinkedList<T>
 *
 * @author Tyler Baxter
 * @version 1.0
 * @since 2024-08-30
 *
 * SinglyLinkedList<T> implementation.
 */

#include "singly-linked-list.h"

#include <exception>

using namespace csc;

SLLIterator<T>& SLLIterator<T>::operator++()
{
	if (_node) {
		_node = _node->get_next();
	}
	return *this;
}

SLLIterator<T> SLLIterator::operator++(int)
{
	SLLIterator tmp = *this;
	++(*this);
	return tmp;
}

T& operator*() const
{
	if (!_node) {
		throw std::out_of_range("Attempt to dereference nullptr iterator");
	}
	return _node->get_element();
}

void SLLIterator::add(const T& element)
{
	_node->set_next(new SLLNode<T>(element, _node->get_next()));
}

void SLLIterator::delete()
{
	// xxx don't think this syntax is going to work. likely going to need to (1)
	// create a copy of the list and get one before our position, or (2) pass a
	// reference to the list as the parameter. (2) is not a solution, that gives
	// the client a ton of bookkeeping. Hopefully other solutions? We need our
	// binded list instance
	SLLNode<T> *node = SinglyLinkedList::search(element);
	node->set_next() = _node->get_next();
	delete _node;
	_node = node->get_next();
	node = nullptr;
}

T* SinglyLinkedList<T>::front() const
{
	if (empty()) {
		return nullptr;
	}
	return _head->get_element();
}

void SinglyLinkedList<T>::insert(const T& element)
{
	if (empty()) {
		_head = new SLLNode<T>(element);
		++_size;
	}
	_head = new SLLNode<T>(element, _head);
	++_size;
}

bool SinglyLinkedList<T>::remove(const T& element)
{
	SLLNode<T> *curr = search();
	if (curr == nullptr) {
		return false;
	}
	curr->set_next() = curr->get_next()->get_next();
	delete curr->get_next();
	curr = nullptr;
	--_size;
	return true;
}

SLLNode<T>* SinglyLinkedList<T>::search(const T& element)
{
	// List is empty, return nullptr.
	if (empty()) {
		return nullptr;
	}
	// Search element is head, return a pointer to head.
	if (_head->get_element() == element) {
		return _head;
	}
	// Already checked head.
	SLLNode<T> *curr = _head->get_next();
	if (curr == nullptr) {
		return nullptr;
	}
	// General case:
	SLLNode<T> *curr_next = _curr->get_next();
	while (curr_next != nullptr) {
		if (curr_next->get_element() == element) {
			return curr;
		}
		curr = curr->get_next();
		curr_next = curr_next->get_next();
	}
	return nullptr;
}

bool SinglyLinkedList<T>::contains(const T& element)
{
	return search() != nullptr;
}

T* SinglyLinkedList<T>::find(const T& element)
{
	return search()->get_next();
}

T* SinglyLinkedList<T>::pop_front()
{
	// Guard if the list is empty.
	if (empty()) {
		return nullptr;
	}
	// Guard if the list only has one element.
	if (_head->get_next() == nullptr) {
		delete _head;
		_head = nullptr;
		return _head;
	}
	// General case:
	SLLNode<T> *curr = _head;
	_head = _head->get_next();
	T *ptr = curr->get_element();
	delete curr;
	curr = nullptr;
	--_size;
	return ptr;
}

SLLIterator<T> SinglyLinkedList<T>::begin() const
{
	return SLLIterator<T>(_head);
}

SLLIterator<T> SinglyLinkedList<T>::end() const
{
	return SLLIterator<T>(nullptr);
}

// xxx
ostream& SinglyLinkedList<T>::operator<<(ostream& out, 
	const SinglyLinkedList<T>& sll) const
{
	if (_head == nullptr) {
		std::cout << "Empty list\n";
	} else if (_head->get_next() == nullptr) {
	// Guard loop if there's only one element.
		std::cout << _head->get_element() << "\n";
	} else {
		SLLNode<T>* curr = _head;
		while (curr->get_next() != nullptr) {
			std::cout << curr->get_element() << ", ";
			curr = curr->get_next();
		}
		std::cout << curr->get_element() << "\n";
		curr = nullptr;
	}
}

// xxx
istream& SinglyLinkedList<T>::operator>>(istream& in, 
	const SinglyLinkedList<T>& sll) const
{
	if (_head == nullptr) {
		std::cout << "Empty list\n";
	} else if (_head->get_next() == nullptr) {
	// Guard loop if there's only one element.
		std::cout << _head->get_element() << "\n";
	} else {
		SLLNode<T>* curr = _head;
		while (curr->get_next() != nullptr) {
			std::cout << curr->get_element() << ", ";
			curr = curr->get_next();
		}
		std::cout << curr->get_element() << "\n";
		curr = nullptr;
	}
}

std::size_t SinglyLinkedList<T>::size() const
{
	return _size;
}

bool SinglyLinkedList<T>::empty() const
{
	return _head == nullptr && _size == 0;
}

void SinglyLinkedList<T>::clear()
{
	while (!empty()) {
		pop_front();
	}
}
