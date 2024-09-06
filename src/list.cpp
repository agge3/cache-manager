/**
 * @file linked-list.cpp
 * @class LinkedList
 *
 * @author Tyler Baxter
 * @version 1.0
 * @since 2024-08-30
 *
 * LinkedList implementation.
 */

#include "list.h"

#include <iostream>
#include <exception>

using namespace csc;

List& List::operator=(const List& rhs)
{
	// Check if both lists have the same address; then they're the same.
    if (&rhs == this) {
		std::cerr << "Attempted assignment to itself.";
    }
	// Else, the lists have different addresses and are distinct.
    else {
        // We don't need to check if _count is the same; not random access!
        if (rhs._count == 0) {
            this->clear();
        }
        else if (this->_count == 0) {
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
    }
    // Return calling list.
    return *this;
}

void List::copy_calling_list_empty(const List& src) {
   // It's assumed calling object is empty, so we don't need to check.
   // Assign caller _count as parameter _count.
   _count = src._count;
   // Create _head for caller.
   _head = new Node(src._head->get_element(), nullptr)
   // curr at _head, src_curr at src _head
   Node* curr = _head;
   Node* src_curr = src._head
   // Loop through all parameter list nodes and create for caller list.
   for (int i = 1; i < _count; ++i) {
       curr->set_next(new Node(src_curr->get_next()->get_element(), nullptr));
       curr = curr->get_next();
       src_curr = src_curr->get_next();
   }
   _tail = _curr;
   // Cleanup dangling pointers.
   curr = src_curr = nullptr;
}

void List::copy_lists_same_length(const List& src) {
    Node* curr = _head;
    Node* src_curr = src._head;
    while (curr != nullptr) {
        curr->set_element(src_curr->get_element());
        curr = curr->get_next();
        src_curr = src_curr->get_next();
    }
   // Cleanup dangling pointers.
   curr = src_curr = nullptr;
}

void List::copy_calling_list_longer(const List& src) {
    // create curr for caller and para, _head
    Node* curr = _head;
    Node* src_curr = src._head;
    // iter through, stopping at _tail node of para list
    while (src_curr != nullptr) {
        curr->set_element(src_curr->get_element());
        if (src_curr->get_next() == nullptr) {
           _tail = curr;
           curr->set_next(nullptr);
        }
        curr = curr->get_next();
        src_curr = src_curr->get_next();
    }
    // curr at new _tail for caller
    // delete everything after...
    while (curr != nullptr) {
        delete curr;
        curr = curr->get_next();
    }
    // cleanup: _count is equal, assign _tail,
    // and delete dangling ptrs
    _count = src._count;
    curr = src_curr = nullptr;
}

void List::copy_calling_list_shorter(const List& src) {
    Node* curr = _head;
    Node* src_curr = src._count;
    while (curr != nullptr) {
        curr->set_element(src_curr->get_element());
        curr = curr->get_next();
        src_curr = src_curr->get_next();
    }
    // reset curr to be at _tail
    curr = _tail;
    // second loop to create new nodes for remaining nodes of caller
    while (src_curr != nullptr) {
        curr->set_next(new Node(src_curr->get_element(),
                    nullptr));
        curr = curr->get_next();
        src_curr = src_curr->get_next();
    }
    // new _tail is next of curr node
    _tail = curr;
    _tail->set_next(nullptr);
    _count = src._count;
    // cleanup: dangling ptrs
    curr = src_curr = nullptr;
}

List::List(const List& src)
{
    // Check if list to be copied has any nodes.
    if (src._head == nullptr) {
		// Create NULL list.
		_head = _tail = _nullptr;
		_count = 0;
    } else {
    	// Assign caller count as parameter count.
    	_count = src._count;
    	// Create _head for caller.
    	_head = new Node(src._head->get_element(), nullptr);
    	// curr at _head, src_curr at src head.
    	Node* curr = _head;
    	Node* src_curr = src._head;
    	// Loop through all parameter nodes and create for caller.
    	for (int i = 1; i < count; ++i) {
    	  curr->set_next(new Node(src_curr->get_next()->get_element(), nullptr));
    	  curr = curr->get_next();
    	  src_curr = src_curr->get_next();
    	}
		_tail = curr;
		// Cleanup dangling pointers.
		curr = src_curr = nullptr;
    }
}

List::List(List&& src) :
	// Steal the r-value list's resources.
	_head(src._head), _tail(src._tail), _count(src._count)
{
	// NULL the r-value list.
	src._head = src._tail = nullptr;
	src._count = 0;
}

List& List::operator=(List&& rhs)
{
	// Check for self-assignment.
	if (this != &rhs)
	{
		// Release the l-value list's resources.
		clear();
		// Steal the r-value list's resources.
		_head = rhs._head;
		_tail = rhs._tail;
		_count = rhs._count;
		// NULL the r-value list.
		src._head = src._tail = nullptr;
		src._count;
	}
	// Return l-value list.
	return *this;
}

std::unique_ptr<Node> List::begin() const
{
    // Return a smart pointer so it has a destructor when it goes out of scope.
    std::unique_ptr<Node> ptr(_head);
	return ptr;
}

std::unique_ptr<Node> List::end() const
{
    // Return a smart pointer so it has a destructor when it goes out of scope.
    std::unique_ptr<Node> ptr(_tail);
	return ptr;
}

bool List::empty() const;
{
	return _head == nullptr;
}

int List::size() const
{
	return _count;
}

int List::front() const
{
	if (_head != nullptr) {
		return _head->get_element();
	}
    else {
        throw std::out_of_range(
			"Attempted to access the front element of an empty list.");
    }

}

int List::back() const
{
	if (_tail != nullptr) {
		return _tail->get_element();
	} else {
        throw std::out_of_range(
			"Attempted to access the back element of an empty list.");
    }
}

void List::push_front(int element)
{
	if (_head == nullptr) {
		_head = new Node(element, nullptr);
		_head = _tail;
	} else {
		_head = new Node(element);
		Node* node = new Node(element);
		node->set_next(_head);
		_head = node;
	}
	// Increment _count, node has been added.
    ++_count;	
}

int List::pop_front()
{
	if (empty()) {
		throw std::out_of_range("Attempted to pop an empty list.");
	}

	int result = front();
	if (_head == _tail) {
		clear();
	} else {
		result = front();
		Node* curr = _head->get_next();
		delete _head;
		_head = curr;
		_head->set_prev(nullptr);
		--_count;
		curr = nullptr;
	}
	return result;
}

void List::push_back(int element)
{
	if (_head == nullptr) {
		_head = new Node(element, nullptr, nullptr);
		_head = _tail;
	} else {
		_tail->set_next(new Node(element, nullptr, _tail->get_prev()));
		_tail = tail->get_next();
	}
    // Increment _count, node has been added.
    ++_count;
}

int List::pop_back()
{
	if (empty()) {
		throw std::out_of_range("Attempted to pop an empty list.");
	}

	int result = back();
	if (_head == _tail) {
		clear();
	} else {
	Node* curr = _tail->get_prev();
	delete _tail;
	_tail = curr;
	_tail->set_next(nullptr);
	--_count;
	curr = nullptr;
	}
	return result;
}

void List::insert(int element, int index)
{
	index_out_of_range(int index);
	// Insert at _head.
	if (index == 0) {
		push_front(element);
	// Insert at _tail.
	} else if (index == _count) {
		push_back(element);
		// Swap the elements so the inserted element is before _tail.
		auto addr = &_tail->get_prev()->get_element();
		std::swap(_tail->get_element(), *addr);
	} else {
		Node* curr = _head->get_next();
		int i = 1
		while (i != index) {
			curr->get_next();
			++i;
		}
		Node* curr_next = curr->get_next();
		curr->set_next(new Node(element, curr_next, curr));
		curr_next->set_prev(curr->get_next());
		++_count;
		curr = curr_next = nullptr;
	}
}

void List::index_out_of_range(int index)
{
	if (index < 0) {
		throw std::out_of_range("Index cannot be negative");
	}
	if (index > _count) {
		throw std::out_of_range("Index out of range");
	}
}


bool List::remove(int element)
{
	// Only delete if list has nodes.
	if (_head != nullptr) {
		// If node to delementte is the head, don't loop.
		if (_head->get_element() == element) {
			Node* tmp = _head;
			_head = tmp->get_next();
			delete tmp;
			tmp = nullptr;
		} else {
			Node* curr = _head;
			while (curr->get_next() != nullptr && 
				curr->get_next()->get_element() != element) {
				curr = curr->get_next();
			}
			if (curr->get_next() == nullptr) {
				// At tail and element not found.
				return false;
			}
			Node* tmp = curr->get_next();
			curr->set_next(tmp->get_next());
			delete tmp;
			tmp = nullptr;
		}
		return true;
	}
	return false;
}

void List::print() const
{
	if (_head == nullptr) {
		std::cout << "Empty list\n";
	} else if (_head->get_next() == nullptr) {
	// Guard loop if there's only one element.
		std::cout << _head->get_element() << "\n";
	} else {
		Node* curr = _head;
		while (curr->get_next() != nullptr) {
			std::cout << curr->get_element() << ", ";
			curr = curr->get_next();
		}
		std::cout << curr->get_element() << "\n";
		curr = nullptr;
	}
}

int List::get(int index) const
{
	index_out_of_range(int index);
	// Check _head.
	if (index == 0) {
		return _head->get_element();
	}
	// Check _tail.
	if (index == _count) {
		return _tail->get_element();
	}
	// _head already checked, _tail already checked.
	Node* curr = _head->get_next();
	int i = 1;
	while (i < _count - 1) {
		if (i == index) {
			return curr->get_element();
		}
		curr = curr->get_next();
	}
	// Cases where the element is not in the list are front-loaded; nothing else
	// to do.
}

std::unique_ptr<Node> List::get(int element) const
{
	Node* curr = _head;
	bool found = false;
    // Guard if list is empty.
    if (_head != nullptr) {
        while (curr != nullptr && !found) {
            if (curr->get_element() == element) {
                found = true;
            } else {
                curr = curr->get_next();
            }
        }
    }
    if (!found) {
		// xxx better error message
        std::cerr << "element not found\n";
	}

    // Make curr unique so it has a destructor when out of scope.
    std::unique_ptr<Node> unique_curr(curr);
    curr = nullptr;
    return unique_curr;
}

bool List::contains(int element) const
{
	return get(element) != nullptr;
}

void List::clear()
{
	if (_head != nullptr) {
		if (_head->get_next() == nullptr) {
			delete _head;
			_head = nullptr;
		} else {
			Node* curr = _head;
			Node* curr_next;
			while (curr != nullptr) {
				curr_next = curr->get_next();
				delete curr;
				curr = curr_next;
			}
			_head = _tail = curr = curr_next = nullptr;
		}
	}
}

List::~List()
{
	// clear() has simple-case conditions built in.
	clear();
}
