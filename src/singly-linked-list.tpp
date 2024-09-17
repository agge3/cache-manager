/**
 * @file singly-linked-list.cpp
 * @class SinglyLinkedList<T>
 *
 * @author Tyler Baxter, Kat Powell
 * @version 1.0
 * @since 2024-08-30
 *
 * SinglyLinkedList<T> implementation.
 */

#include "singly-linked-list.h"

#include <exception>

using namespace csc;

template <typename T>
SLLIterator<T>& SLLIterator<T>::operator++()
{
	if (_node) {
		_node = _node->get_next();
	}
	return *this;
}

template <typename T>
SLLIterator<T> SLLIterator::operator++(int)
{
	SLLIterator tmp = *this;
	++(*this);
	return tmp;
}

template <typename T>
T& operator*() const
{
	if (!_node) {
		throw std::out_of_range("Attempt to dereference nullptr iterator");
	}
	return _node->get_element();
}

template <typename T>
void SLLIterator::add(const T& element)
{
	_node->set_next(new SLLNode<T>(element, _node->get_next()));
}

template <typename T>
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

template <typename T>
SinglyLinkedList<T>::SinglyLinkedList(const SinglyLinkedList<T>& other)
{
    if (!other.empty()) {
        _count = other._count;
        _head = new SLLNode<T>(other._head->get_element(),
                               other._head->get_next());
        SLLNode* curr = _head->get_next();
        SLLNode* other_curr = _head->get_next();
        while(other_curr != nullptr) {
            curr->set_next(new SLLNode<T>(other_curr->get_element(),
                           other_curr->get_next()));
            other_curr = other_curr->get_next();
            curr = curr->get_next();
        }
        curr = other_curr = nullptr;
    }
}

template <typename T>
SinglyLinkedList<T>::SinglyLinkedList(SinglyLinkedList<T>&& other) noexcept :
    _head(other._head), _count(other._count)
{
    other._head = nullptr;
    other._count = 0;
}

template <typename T>
SinglyLinkedList<T>& operator=(const SinglyLinkedList<T>& rhs)
{
    if (&rhs == this) {
        std::cerr << "Attempted assignment to self.";
        return *this;
    }

    if (rhs.empty()) {
        this->clear();
    }

    _head = rhs._head;
    _count = rhs._count;
    SLLNode* curr = _head;
    SLLNode* rhs_curr = rhs._head->get_next();

    // XXX test this condition
    while (curr != nullptr && rhs_curr != nullptr) {
        curr->set_next(new SLLNode(rhs_curr, rhs_curr->get_next());
        rhs_curr = rhs_curr->get_next();
        curr = curr->get_next();
    }

    curr = rhs_curr = nullptr;
}

template <typename T>
SinglyLinkedList<T>& SinglyLinkedList<T>::operator=(SinglyLinkedList<T>&& rhs)
{
    if (this != &rhs) {
        clear();

        _head = rhs._head;
        _count = rhs._count;

        rhs._head = nullptr;
        rhs._count = 0;
    }

    return *this;
}

template <typename T>
T* SinglyLinkedList<T>::front() const
{
	if (empty()) {
		return nullptr;
	}
	return _head->get_element();
}

template <typename T>
void SinglyLinkedList<T>::insert(const T& element)
{
	if (empty()) {
		_head = new SLLNode<T>(element);
		++_size;
	}
	_head = new SLLNode<T>(element, _head);
	++_size;
}

template <typename T>
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

template <typename T>
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

template <typename T>
bool SinglyLinkedList<T>::contains(const T& element)
{
	return search() != nullptr;
}

template <typename T>
T* SinglyLinkedList<T>::find(const T& element)
{
	return search()->get_next();
}

template <typename T>
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

template <typename T>
SLLIterator<T> SinglyLinkedList<T>::begin() const
{
	return SLLIterator<T>(_head);
}

template <typename T>
SLLIterator<T> SinglyLinkedList<T>::end() const
{
	return SLLIterator<T>(nullptr);
}

// xxx
template <typename T>
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
template <typename T>
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

template <typename T>
std::size_t SinglyLinkedList<T>::size() const
{
	return _size;
}

template <typename T>
bool SinglyLinkedList<T>::empty() const
{
	return _head == nullptr && _size == 0;
}

template <typename T>
void SinglyLinkedList<T>::clear()
{
	while (!empty()) {
		pop_front();
	}
}
