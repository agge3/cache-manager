/**
 * @file file-concurrent-list-impl.hpp
 *
 * @author agge3
 * @version 1.0
 * @since 2024-09-21
 *
 * FineConcurrentList concrete implementation.
 */

#include <algorithm>
#include <atomic>
#include <iostream>
#include <exception>
#include <mutex>
#include <shared_mutex>

using namespace cm;

// need to inline to not lose lock, while reusing code (like a function call)
#define POP_FRONT \
	do {	\
		if (!_head) {	\
			return std::nullopt;	\
		}	\
			\
		if (_head == _tail) {	\
			T ele = _head->ele;	\
			delete _head;	\
			_head = _tail = nullptr;	\
			_size = 0;	\
			return std::optional<T>(ele);	\
		}	\
	\
		T ele = _head->ele;	\
		auto tmp = _head;	\
		_head = _head->next;	\
		_head->prev = nullptr;	\
		delete tmp;	\
		tmp = nullptr;	\
		--_size;	\
		return std::optional<T>(ele);	\
	} while (0)
#define POP_BACK	\
	do {	\
		if (!_tail) {	\
			return std::nullopt;	\
		}	\
			\
		if (_head == _tail) {	\
			T ele = _tail->ele;	\
			delete _tail;	\
			_head = _tail = nullptr;	\
			_size = 0;	\
			return std::optional<T>(ele);	\
		}	\
		\
		T ele = _tail->ele;	\
		auto tmp = _tail;	\
		_tail = _tail->prev;	\
		_tail->next = nullptr;	\
		delete tmp;	\
		tmp = nullptr;	\
		--_size;	\
		return std::optional<T>(ele);	\
	} while (0)

template <typename T>
typename ListIterator<T>::const_reference ListIterator<T>::operator*()
{
	if (!_node) {
		throw std::runtime_error("Attempt to dereference a null iterator.");
	}
	return _node->ele;
}

template <typename T>
typename ListIterator<T>::pointer ListIterator<T>::operator->()
{
	if (!_node) {
		throw std::runtime_error("Attempt to dereference a null iterator.");
	}
	return &(_node->ele);
}

template <typename T>
ListIterator<T>& ListIterator<T>::operator++()
{
	if (_node) {
		_node = _node->next;
	}
	return *this;
}

template <typename T>
ListIterator<T> ListIterator<T>::operator++(int)
{
	ListIterator tmp = *this;
	++(*this);
	return tmp;
}

template <typename T>
ListIterator<T>& ListIterator<T>::operator--()
{
	if (_node) {
		_node = _node->prev;
	}
	return *this;
}

template <typename T>
ListIterator<T> ListIterator<T>::operator--(int)
{
	ListIterator tmp = *this;
	--(*this);
	return tmp;
}

template <typename T>
bool ListIterator<T>::operator==(const ListIterator& other) const
{
	return _node == other._node;
}

template <typename T>
bool ListIterator<T>::operator!=(const ListIterator& other) const
{
	return _node != other._node;
}

template <typename T>
void FineConcurrentList<T>::copyCallingListEmpty(
	const FineConcurrentList<T>& other) {
	// It's assumed calling object is empty, so we don't need to check.
	// Assign caller _size as other _size.
	_size = other._size;
	// Create _head for caller.
	_head = new ListNode<T>(other._head->ele);
	// curr at _head, otherCurr at other _head
	ListNode<T> *curr = _head;
	ListNode<T> *otherCurr = other._head;
	// Loop through all other list nodes and create for caller list.
	for (int i = 1; i < _size; ++i) {
		otherCurr = otherCurr->next;
		curr->next = new ListNode<T>(otherCurr->ele, nullptr, curr);
		curr = curr->next;
	}
	_tail = curr;
	// Cleanup dangling pointers.
	curr = otherCurr = nullptr;
}

template <typename T>
void FineConcurrentList<T>::copyListsSameLength(
	const FineConcurrentList<T>& other)
{
	ListNode<T> *curr = _head;
	ListNode<T> *otherCurr = other._head;
	while (curr != nullptr) {
		curr->ele = otherCurr->ele;
		curr = curr->next;
		otherCurr = otherCurr->next;
	}
	// Cleanup dangling pointers.
	curr = otherCurr = nullptr;
}

template <typename T>
void FineConcurrentList<T>::copyCallingListLonger(
	const FineConcurrentList<T>& other)
{
	// Create curr for caller and other _head.
	ListNode<T>* curr = _head;
	ListNode<T>* otherCurr = other._head;
	// Iterate through, stopping at _tail node of other.
	while (otherCurr != nullptr) {
		curr->ele = otherCurr->ele;
		if (otherCurr->next == nullptr) {
		   _tail = curr;
		   curr->next = nullptr;
		}
		curr = curr->next;
		otherCurr = otherCurr->next;
	}
	// curr at new _tail for caller, delete everything after.
	while (curr != nullptr) {
		delete curr;
		curr = curr->next;
	}
	// Cleanup: _size is equal, assign _tail, and delete dangling pointers.
	_size = other._size;
	curr = otherCurr = nullptr;
}

template <typename T>
void FineConcurrentList<T>::copyCallingListShorter(
	const FineConcurrentList<T>& other)
{
	ListNode<T>* curr = _head;
	ListNode<T>* otherCurr = other._head;
	while (curr != nullptr) {
		curr->ele = otherCurr->ele;
		curr = curr->next;
		otherCurr = otherCurr->next;
	}
	// Reset current to be at _tail.
	curr = _tail;
	// Second loop to create new nodes for remaining nodes of caller.
	while (otherCurr != nullptr) {
		curr->next = new ListNode<T>(otherCurr->ele, nullptr, curr);
		curr = curr->next;
		otherCurr = otherCurr->next;
	}
	// New _tail is next of current node.
	_tail = curr;
	_tail->next = nullptr;
	_size = other._size;
	// Cleanup dangling pointers.
	curr = otherCurr = nullptr;
}

template <typename T>
ListIterator<T> FineConcurrentList<T>::unsafeBegin() const
{
	return ListIterator<T>(_head);
}

template <typename T>
ListIterator<T> FineConcurrentList<T>::unsafeEnd() const
{
	return ListIterator<T>(nullptr);
}

template <typename T>
bool FineConcurrentList<T>::isEmpty() const
{
	std::shared_lock<std::shared_mutex> g(_mutex);
	return _head == nullptr && _tail == nullptr && _size == 0;
}

template <typename T>
std::size_t FineConcurrentList<T>::size() const
{
	std::shared_lock<std::shared_mutex> g(_mutex);

	size_t size = 0;

	auto cur = _head;
	if (!cur) {
		return size;
	}

	while (cur) {
		++size;
		cur = cur->next;
	}

	return size;
}

template <typename T>
std::size_t FineConcurrentList<T>::unsafeSize() const
{
	return _size;
}

template <typename T>
std::optional<T> FineConcurrentList<T>::front() const
{
	std::shared_lock<std::shared_mutex> g(_mutex);
	return !_head ? std::nullopt : std::optional<T>(_head->ele);
}

template <typename T>
std::optional<T> FineConcurrentList<T>::back() const
{
	std::shared_lock<std::shared_mutex> g(_mutex);
	return !_tail ? std::nullopt : std::optional<T>(_tail->ele);
}

template <typename T>
const ListNode<T>* FineConcurrentList<T>::pushFront(const T& element)
{
	// heap allocate before write lock to optimize allocation out of
	// critical section
	auto *ptr = new ListNode<T>(element);

	std::unique_lock<std::shared_mutex> g(_mutex);

	if (!_head) {
		_head = _tail = ptr;

		// Increment size, node has been added.
		++_size;

		ptr = nullptr;
		return _head;
	}

	ptr->next = _head;
	_head->prev = ptr;
	_head = ptr;

	// Increment size, node has been added.
	++_size;

	ptr = nullptr;
	return _head;
}

template <typename T>
std::optional<T> FineConcurrentList<T>::popFront()
{
	std::unique_lock<std::shared_mutex> g(_mutex);
	return POP_FRONT;
}

template <typename T>
const ListNode<T>* FineConcurrentList<T>::pushBack(const T& element)
{
	// heap allocate before write lock to optimize allocation out of
	// critical section
	auto *ptr = new ListNode<T>(element);

	std::unique_lock<std::shared_mutex> g(_mutex);
	if (!_head) {
		_head = _tail = ptr;
		++_size;	// Increment _size, node has been added.
		return _head;
	}

	_tail->next = ptr;
	ptr->prev = _tail;
	_tail = ptr;

	++_size;	// Increment _size, node has been added.

	ptr = nullptr;
	return _tail;
}

template <typename T>
std::optional<T> FineConcurrentList<T>::popBack()
{
	std::unique_lock<std::shared_mutex> g(_mutex);
	return POP_BACK;
}

template <typename T>
const ListNode<T>* FineConcurrentList<T>::get(const T& element)
{
	return search(element);
}

template <typename T>
std::optional<T> FineConcurrentList<T>::get(const ListNode<T> *ptr)
{
	return !ptr ? std::nullopt : std::optional<T>(ptr->_ele);
}

template <typename T>
bool FineConcurrentList<T>::remove(const T& element)
{
	{
		// fast checks under global lock
		std::shared_lock<std::shared_mutex> g(_mutex);

		// Only remove if list has nodes.
		if (!_head) {
			return false;
		}

		// Handle head and tail cases.
		if (_head->_ele == element) {
			POP_FRONT;
			return true;
		} else if (_tail->_ele == element) {
			// `else if` to lock control flow into `size > 1` for tail case.
			POP_BACK;
			return true;
		}
	}

	// General case:
	// Already handled head and tail, so safe to assume `size() > 2`.
	ListNode<T> *node = const_cast<ListNode<T>*>(search(element));
	if (!node) {
		return false;
	}

	unlink(node);
	delete node;
	node = nullptr;

	std::unique_lock g{_mutex};
	--_size;

	return true;
}

template <typename T>
bool FineConcurrentList<T>::remove(const ListNode<T> *node)
{
	if (!node) {
		return false;
	}

	{
		// fast checks under global lock
		std::shared_lock<std::shared_mutex> g(_mutex);

		// Only remove if list has nodes.
		if (!_head) {
			return false;
		}

		// Handle head and tail cases.
		if (_head == node) {
			POP_FRONT;
			node = nullptr;
			return true;
		} else if (_tail == node) {
			// `else if` to lock control flow into `size > 1` for tail case.
			POP_BACK;
			node = nullptr;
			return true;
		}
	}

	// General case:
	// Already handled head and tail, so safe to assume `size() > 2`.
	ListNode<T> *n = const_cast<ListNode<T>*>(node);

	unlink(n);
	delete n;
	n = nullptr;

	std::unique_lock g{_mutex};
	--_size;

	return true;
}

template <typename T>
bool FineConcurrentList<T>::removeAndPushFront(const ListNode<T> *node) {
	if (!node) {
		return false;
	}

	ListNode<T> *n = const_cast<ListNode<T>*>(node);

	unlink(n);

	std::unique_lock<std::shared_mutex> g(_mutex);
	n->next = _head;
	_head = node;
}

template <typename T>
bool FineConcurrentList<T>::unlink(const ListNode<T> *ptr)
{
	// Validate node before locking:
	if (!ptr) {
		return false;
	}

	// Cast away the client's const, we're in our owned instance.
	ListNode<T> *node = const_cast<ListNode<T>*>(ptr);

	// Handle head and tail:
	{
		std::unique_lock<std::shared_mutex> g(_mutex);
		if (!_head) {	// empty list
			return false;
		}

		if (node == _head) {
			_head->next->prev = nullptr;
			_head = _head->next;
			node->next = node->prev = nullptr;
			return true;
		} else if (node == _tail) {
			// `else if` will trap us in a condition where `size() > 1`.
			_tail->prev->next = nullptr;
			_tail = _tail->prev;
			node->next = node->prev = nullptr;
			return true;
		}
	}

	// General case:
	std::unique_lock lk{ptr->mtx, std::defer_lock};
	std::unique_lock nlk{ptr->next->mtx, std::defer_lock};
	std::unique_lock plk{ptr->prev->mtx, std::defer_lock};
	std::lock(lk, nlk, plk);

	ptr->next->prev = ptr->prev;
	ptr->prev->next = ptr->next;
	ptr->next = ptr->prev = nullptr;

	return true;
}

template <typename T>
const ListNode<T>* FineConcurrentList<T>::search(const T& element) const
{
	std::shared_lock g{_mutex};
	// Guard if list is empty.
	if (!_head) {
		return nullptr;
	}
	// simple O(1) cases:
	if (_head->ele == element) {
		return _head;
	}
	if (!_head->next) {
		return nullptr;
	}
	if (_tail->ele == element) {
		return _tail;
	}
	if (_head->next->ele == element) {
		return _head->next;
	}
	if (_tail->prev && _tail->prev->ele == element) {
		return _tail->prev;
	}

	const ListNode<T> *curr = _head->next;
	std::shared_lock lk{curr->mtx};
	// release global lock after node is protected
	g.unlock();

	while (curr) {
		if (curr->_ele == element) {
			return curr;
		}

		const ListNode<T> *next = curr->next;
		if (!next) {
			return nullptr;
		}

		std::shared_lock nlk{next->mtx};
		lk.unlock();
		curr = next;
		lk = std::move(nlk);
	}

	return nullptr;
}

template <typename T>
bool FineConcurrentList<T>::contains(const T& element) const
{
	return search(element);
}

template <typename T>
bool FineConcurrentList<T>::contains(const ListNode<T> *ptr) const
{
	if (!ptr) {
		return false;
	}

	// Guard if list is empty.
	std::shared_lock g{_mutex};
	if (!_head) {
		return false;
	}
	if (!_head->next) {
		return _true;
	}
	const ListNode<T> *curr = _head->next;
	g.unlock();

	// Iterate looking for matching address of parameter pointer against
	// a FineConcurrentList pointer.
	std::shared_lock lk{curr->mtx};
	while (curr) {
		if (curr == ptr) {
			return true;
		}

		const ListNode<T> *next = curr->next;
		if (!next) {
			return false;
		}

		std::shared_lock nlk{next->mtx};
		lk.unlock();
		curr = next;
		lk = std::move(nlk);
	}

	return false;
}

template <typename T>
void FineConcurrentList<T>::clear()
{
	// Need exclusive control over list when modifying
	std::unique_lock<std::shared_mutex> g{_mutex};

	ListNode<T>* curr = _head;
	while (curr) {
		ListNode<T> *next = curr->next;
		delete curr;
		curr = next;
	}

	_head = _tail = nullptr;
	_size = 0;
}

template <typename T>
void FineConcurrentList<T>::unsafeClear()
{
    ListNode<T>* curr = _head;
    while (curr) {
        ListNode<T>* next = curr->next;
        delete curr;
        curr = next;
    }
    _head = nullptr;
    _tail = nullptr;
    _size = 0;
}

