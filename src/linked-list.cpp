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

#include "linked-list.h"

#include <iostream>
#include <exception>

using namespace csc;

void LinkedList::insertAtBeginning(int val)
{
	// Check if list already has a head.
	if (_head != nullptr) {
		Node* node = new Node(val);
		node->setNext(_head);
		_head = node;
	} else {
	// Empty list, create head.
		_head = new Node(val);
	}
}

void LinkedList::insertAtEnd(int val)
{
	// Check if list is empty and guard loop.
	if (_head != nullptr) {
		Node* curr = _head;
		while (curr->getNext() != nullptr) {
			curr = curr->getNext();
		}
		curr->setNext(new Node(val));
		curr = nullptr;
	} else {
		_head = new Node(val);
	}
}

bool LinkedList::deleteNode(int val)
{
	// Only delete if list has nodes.
	if (_head != nullptr) {
		// If node to delete is the head, don't loop.
		if (_head->getData() == val) {
			Node* tmp = _head;
			_head = tmp->getNext();
			delete tmp;
			tmp = nullptr;
		} else {
			Node* curr = _head;
			while (curr->getNext() != nullptr && 
				curr->getNext()->getData() != val) {
				curr = curr->getNext();
			}
			if (curr->getNext() == nullptr) {
				// At tail and value not found.
				return false;
			}
			Node* tmp = curr->getNext();
			curr->setNext(tmp->getNext());
			delete tmp;
			tmp = nullptr;
		}
		return true;
	}
	return false;
}

void LinkedList::printList() const
{
	if (_head == nullptr) {
		std::cout << "Empty list\n";
	} else if (_head->getNext() == nullptr) {
	// Guard loop if there's only one element.
		std::cout << _head->getData() << "\n";
	} else {
		Node* curr = _head;
		while (curr->getNext() != nullptr) {
			std::cout << curr->getData() << ", ";
			curr = curr->getNext();
		}
		std::cout << curr->getData() << "\n";
		curr = nullptr;
	}
}

// XXX Very quick and dirty. 
int LinkedList::get(int idx) const {
	if (idx < 0) {
		throw std::out_of_range("Index cannot be negative");
	}
	int count = 0;
	Node* curr = _head;
	while (curr != nullptr) {
		if (count == idx) {
			return curr->getData();
		}
		++count;
		curr = curr->getNext();
	}
	throw std::out_of_range("Index out of range");
}

LinkedList::~LinkedList() {
	if (_head != nullptr) {
		if (_head->getNext() == nullptr) {
			delete _head;
			_head = nullptr;
		} else {
			Node* curr = _head;
			Node* currNext;
			while (curr != nullptr) {
				currNext = curr->getNext();
				delete curr;
				curr = currNext;
			}
			curr = currNext = nullptr;
		}
	}
}
