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

//template <typename T>
//SLLIterator<T>& SLLIterator<T>::operator++()
//{
//	if (_node) {
//		_node = _node->getNext();
//	}
//	return *this;
//}
//
//template <typename T>
//SLLIterator<T> SLLIterator<T>::operator++(int)
//{
//	SLLIterator tmp = *this;
//	++(*this);
//	return tmp;
//}
//
//template <typename T>
//T& SLLIterator<T>::operator*()
//{
//	if (!_node) {
//		throw std::out_of_range("Attempt to dereference nullptr iterator");
//	}
//	return _node->getElement();
//}
//
//template <typename T>
//void SLLIterator<T>::add(const T& element)
//{
//	_node->setNext(new SLLNode<T>(element, _node->getNext()));
//}

//template <typename T>
//void SLLIterator<T>::deleteIt()
//{
//	// xxx don't think this syntax is going to work. likely going to need to (1)
//	// create a copy of the list and get one before our position, or (2) pass a
//	// reference to the list as the parameter. (2) is not a solution, that gives
//	// the client a ton of bookkeeping. Hopefully other solutions? We need our
//	// binded list instance
//	SLLNode<T> *node = SinglyLinkedList<T>::search(element);
//	node->setNext() = _node->getNext();
//	delete _node;
//	_node = node->getNext();
//	node = nullptr;
//}

template <typename T>
SinglyLinkedList<T>::SinglyLinkedList(const SinglyLinkedList<T>& other)
{
    if (!other.empty()) {
        _size = other._size;
        _head = new SLLNode<T>(other._head->getElement(),
                               other._head->getNext());
        SLLNode<T>* curr = _head->getNext();
        SLLNode<T>* otherCurr = _head->getNext();
        while(otherCurr != nullptr) {
            curr->setNext(new SLLNode<T>(otherCurr->getElement(),
                           otherCurr->getNext()));
            otherCurr = otherCurr->getNext();
            curr = curr->getNext();
        }
        curr = otherCurr = nullptr;
    }
}

template <typename T>
SinglyLinkedList<T>::SinglyLinkedList(SinglyLinkedList<T>&& other) noexcept :
    _head(other._head), _size(other._size)
{
    other._head = nullptr;
    other._size = 0;
}

//template <typename T>
//SinglyLinkedList<T>::~SinglyLinkedList()
//{
//    SLLNode<T>* curr = _head->getNext();
//    SLLNode<T>* prev = _head;
//    while (curr != nullptr) {
//        delete prev;
//        prev = curr;
//        curr = curr->getNext();
//    }
//    _head = curr = prev = nullptr;
//    _size = 0;
//}

template <typename T>
SinglyLinkedList<T>& SinglyLinkedList<T>::operator=(const SinglyLinkedList<T>& rhs)
{
    if (&rhs == this) {
        std::cerr << "Attempted assignment to self.";
        return *this;
    }

    if (rhs.empty()) {
        this->clear();
    }

    _head = rhs._head;
    _size = rhs._size;
    SLLNode<T>* curr = _head;
    SLLNode<T>* rhsCurr = rhs._head->getNext();

    // XXX test this condition
    while (curr != nullptr && rhsCurr != nullptr) {
        curr->setNext(new SLLNode<T>(rhsCurr->getElement(), rhsCurr->getNext()));
        rhsCurr = rhsCurr->getNext();
        curr = curr->getNext();
    }

    curr = rhsCurr = nullptr;
	return *this;
}

template <typename T>
SinglyLinkedList<T>& SinglyLinkedList<T>::operator=(SinglyLinkedList<T>&& rhs)
	noexcept
{
    if (this != &rhs) {
        clear();

        _head = rhs._head;
        _size = rhs._size;

        rhs._head = nullptr;
        rhs._size = 0;
    }

    return *this;
}

template <typename T>
T SinglyLinkedList<T>::front() const
{
	if (empty()) {
		//return nullptr; do we want to return ptrs or values?
	}
	return _head->getElement();
}

template <typename T>
void SinglyLinkedList<T>::insert(const T& element, SLLNode<T>* node)
{
	if (empty()) {
		_head = new SLLNode<T>(element);
		++_size;
	} else {
		node->setNext(new SLLNode<T>(element, node->getNext()));
		++_size;
	}
}

template <typename T>
bool SinglyLinkedList<T>::remove(const T& element)
{
	// Check if list is empty -> return out if so
	if (empty()) {
		return false;
	}

	if (_head->getElement() == element) {
		SLLNode<T>* temp = _head;
		_head = _head->getNext();
		delete temp;
		temp = nullptr;
		--_size;
		return true;
	}
	
	// Else -> set up delete sequence for node inside list
	SLLNode<T> *prev = _head;
	SLLNode<T> *curr = _head->getNext();
	while (curr) {
		if (curr->getElement() == element) {
			break;
		}
		prev = curr;
		curr = curr->getNext();
	}

	if (!curr) {
		return false;
	}

	prev->setNext(curr->getNext());
	delete curr;
	curr = prev = nullptr;
	--_size;
	return true;
}

template <typename T>
SLLNode<T>* SinglyLinkedList<T>::search(const T& element) const
{
	// List is empty, return nullptr.
	if (empty()) {
		return nullptr;
	}
	// Search element is head, return a pointer to head.
	if (_head->getElement() == element) {
		return _head;
	}
	// Already checked head.
	SLLNode<T> *curr = _head->getNext();
	if (curr == nullptr) {
		return nullptr;
	}
	// General case:
	SLLNode<T> *currNext = curr->getNext();
	while (currNext != nullptr) {
		if (currNext->getElement() == element) {
			return curr;
		}
		curr = curr->getNext();
		currNext = currNext->getNext();
	}
	return nullptr;
}

template <typename T>
bool SinglyLinkedList<T>::contains(const T& element) const
{
	return search(element) != nullptr;
}

//template <typename T>
//T* SinglyLinkedList<T>::find(const T& element)
//{
//	return search()->getNext();
//}

template <typename T>
T SinglyLinkedList<T>::popFront()
{
	// Guard if the list is empty.
	if (empty()) {
		throw std::out_of_range("Attempt to pop an empty list.");
	}
	// Guard if the list only has one element.
	if (_head->getNext() == nullptr) {
		T ele = _head->getElement();
		delete _head;
		_head = nullptr;
		_size = 0;
		return ele;
	}
	// General case:
	T ele = _head->getElement();
	SLLNode<T>* curr = _head;
	_head = _head->getNext();
	delete curr;
	curr = nullptr;
	--_size;
	return ele;
}

template <typename T>
void SinglyLinkedList<T>::pushFront(const T& element)
{
	// Check if list already has a head.
	if (_head != nullptr) {
		SLLNode<T>* node = new SLLNode<T>(element);
		node->setNext(_head);
		_head = node;
	} else {
	// Empty list, create head.
		_head = new SLLNode<T>(element);
	}
}

template <typename T>
void SinglyLinkedList<T>::push_back(const T& element)
{
	// Check if list is empty and guard loop.
	if (_head != nullptr) {
		SLLNode<T>* curr = _head;
		while (curr->getNext() != nullptr) {
			curr = curr->getNext();
		}
		curr->setNext(new SLLNode<T>(element));
		curr = nullptr;
	} else {
		_head = new SLLNode<T>(element);
	}
}

//template <typename T>
//SLLIterator<T> SinglyLinkedList<T>::begin() const
//{
//	return SLLIterator<T>(_head);
//}
//
//template <typename T>
//SLLIterator<T> SinglyLinkedList<T>::end() const
//{
//	return SLLIterator<T>(nullptr);
//}

// xxx
template <typename T>
std::ostream& operator<<(std::ostream& out, const SinglyLinkedList<T>& sll)
{
	if (sll._head == nullptr) {
		std::cout << "Empty list\n";
	} else if (sll._head->getNext() == nullptr) {
	// Guard loop if there's only one element.
		std::cout << sll._head->getElement() << "\n";
	} else {
		SLLNode<T>* curr = sll._head;
		while (curr->getNext() != nullptr) {
			std::cout << curr->getElement() << ", ";
			curr = curr->getNext();
		}
		std::cout << curr->getElement() << "\n";
		curr = nullptr;
	}
}

// xxx
template <typename T>
std::istream& operator>>(std::istream& in, SinglyLinkedList<T>& sll)
{
	if (sll._head == nullptr) {
		std::cout << "Empty list\n";
	} else if (sll._head->getNext() == nullptr) {
	// Guard loop if there's only one element.
		std::cout << sll._head->getElement() << "\n";
	} else {
		SLLNode<T>* curr = sll._head;
		while (curr->getNext() != nullptr) {
			std::cout << curr->getElement() << ", ";
			curr = curr->getNext();
		}
		std::cout << curr->getElement() << "\n";
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
		popFront();
	}
}
