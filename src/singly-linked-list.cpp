/**
 * @file singly-linked-list.cpp
 * @class SinglyLinkedList<T>
 *
 * @author Kat Powell
 * @version 1.0
 * @since 2024-09-19
 *
 * SinglyLinkedList implementation.
 */

#include <exception>

using namespace csc;

template <typename T>
typename SLLIterator<T>::const_reference SLLIterator<T>::operator*()
{ 
    if (!_node) {
        throw std::runtime_error("Attempt to dereference a null iterator.");
    }
    return _node->getElement();
}

template <typename T>
typename SLLIterator<T>::pointer SLLIterator<T>::operator->() 
{ 
	if (!_node) {
    	throw std::runtime_error("Attempt to dereference a null iterator.");
	}
	return &(_node->getElement()); 
}

template <typename T>
SLLIterator<T>& SLLIterator<T>::operator++()
{
	if (_node) {
		_node = _node->getNext();
	}
	return *this;
}

template <typename T>
SLLIterator<T> SLLIterator<T>::operator++(int)
{
	SLLIterator tmp = *this;
	++(*this);
	return tmp;
}

template <typename T>
bool SLLIterator<T>::operator==(const SLLIterator& other) const 
{
    return _node == other._node;
}

template <typename T>
bool SLLIterator<T>::operator!=(const SLLIterator& other) const 
{
    return _node != other._node;
}

template <typename T>
SinglyLinkedList<T>::SinglyLinkedList(const SinglyLinkedList<T>& other)
{
    if (!other.isEmpty()) {
        _size = other._size;
        _head = new SLLNode<T>(other._head->getElement(),
                               other._head->getNext());
        SLLNode<T>* curr = _head->getNext();
        SLLNode<T>* other_curr = _head->getNext();
        while(other_curr != nullptr) {
            curr->setNext(new SLLNode<T>(other_curr->getElement(),
                           other_curr->getNext()));
            other_curr = other_curr->getNext();
            curr = curr->getNext();
        }
        curr = other_curr = nullptr;
    }
}

template <typename T>
SinglyLinkedList<T>::SinglyLinkedList(SinglyLinkedList<T>&& other) noexcept :
    _head(other._head), _size(other._size)
{
    other._head = nullptr;
    other._size = 0;
}

template <typename T>
SinglyLinkedList<T>& SinglyLinkedList<T>::operator=(const SinglyLinkedList<T>& rhs)
{
    if (&rhs == this) {
        std::cerr << "Attempted assignment to self.";
        return *this;
    }

    if (rhs.isEmpty()) {
        this->clear();
    }

    _head = rhs._head;
    _size = rhs._size;
    SLLNode<T>* curr = _head;
    SLLNode<T>* rhs_curr = rhs._head->getNext();

    // XXX test this condition
    while (curr != nullptr && rhs_curr != nullptr) {
        curr->setNext(new SLLNode<T>(rhs_curr->getElement(), rhs_curr->getNext()));
        rhs_curr = rhs_curr->getNext();
        curr = curr->getNext();
    }

    curr = rhs_curr = nullptr;
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
	if (isEmpty()) {
		//return nullptr; do we want to return ptrs or values?
	}
	return _head->getElement();
}

template <typename T>
SLLNode<T>* SinglyLinkedList<T>::front_ptr() const
{
	if (isEmpty()) {
		return nullptr;
	}
	SLLNode<T>* ptr = _head;
	return ptr;
}

template <typename T>
void SinglyLinkedList<T>::insert(const T& element, SLLNode<T>* node)
{
	if (isEmpty()) {
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
	// Check if list is isEmpty -> return out if so
	if (isEmpty()) {
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
	// List is isEmpty, return nullptr.
	if (isEmpty()) {
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
	SLLNode<T> *curr_next = curr;
	while (curr_next != nullptr) {
		if (curr_next->getElement() == element) {
			return curr;
		}
		curr = curr->getNext();
		curr_next = curr_next->getNext();
	}
	return nullptr;
}

template <typename T>
bool SinglyLinkedList<T>::contains(const T& element) const
{
	return search(element) != nullptr;
}

template <typename T>
T SinglyLinkedList<T>::popFront()
{
	// Guard if the list is isEmpty.
	if (isEmpty()) {
		throw std::out_of_range("Attempt to pop an isEmpty list.");
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
		++_size;
	} else {
	// Empty list, create head.
		_head = new SLLNode<T>(element);
		++_size;
	}
}

template <typename T>
void SinglyLinkedList<T>::pushBack(const T& element)
{
	// Check if list is isEmpty and guard loop.
	if (_head != nullptr) {
		SLLNode<T>* curr = _head;
		while (curr->getNext() != nullptr) {
			curr = curr->getNext();
		}
		curr->setNext(new SLLNode<T>(element));
		curr = nullptr;
		++_size;
	} else {
		_head = new SLLNode<T>(element);
		++_size;
	}
}

template <typename T>
void SinglyLinkedList<T>::print() const {
    if (!isEmpty()) {
    	SLLNode<T>* curr = _head;
    	std::cout << "[ ";
    	while (curr) {
    	    std::cout << curr->getElement();
    	    curr = curr->getNext();
    	    if (curr) {
    	        std::cout << ", ";
    	    }
    	}
    	std::cout << " ]" << std::endl;
	} else {
    	std::cout << "Empty list" << std::endl;
	}
}

template <typename T>
std::size_t SinglyLinkedList<T>::size() const
{
	return _size;
}

template <typename T>
bool SinglyLinkedList<T>::isEmpty() const
{
	return _head == nullptr && _size == 0;
}

template <typename T>
void SinglyLinkedList<T>::clear()
{
	while (!isEmpty()) {
		popFront();
	}
}
