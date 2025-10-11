/**
 * @file concurrent-list-impl.hpp
 *
 * @author agge3
 * @version 1.0
 * @since 2024-09-21
 *
 * ConcurrentList and ConcurrentList helpers implementation.
 */

#include <algorithm>
#include <atomic>
#include <iostream>
#include <exception>
#include <mutex>
#include <shared_mutex>

using namespace cm;

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
ConcurrentList<T>::ConcurrentList(const ConcurrentList<T>& other)
{
	// Check if list to be copied has any nodes.
	// copy with global read lock on other to snapshot
	std::shared_lock<std::shared_mutex> other_g(other._mutex);
	if (!other.isEmpty()) {
		copyCallingListEmpty(other);
	}
}

template <typename T>
ConcurrentList<T>::ConcurrentList(ConcurrentList<T>&& other) noexcept
{
	// Two steps:
	// 	1. Steal the r-value rist's resources.
	// 	2. NULL the r-value list.

	// std::move does both, and we implement in terms of the move assignment
	// operator to reduce bugs and code duplication.
	*this = std::move(other);
}

template <typename T>
ConcurrentList<T>& ConcurrentList<T>::operator=(
	const ConcurrentList<T>& rhs)
{
	// Check if both lists have the same address; then they're the same.
	if (&rhs == this) {
		std::cerr << "Attempted assignment to self.";
		// Return out.
		return *this;
	}

	// acquire both global locks in fixed order to avoid deadlock:
	// lock rhs for reading, then lock lhs (this) for writing
	std::shared_lock<std::shared_mutex> rhs_g(rhs._mutex);
	std::unique_lock<std::shared_mutex> g(_mutex);

	// We've proceeded, the lists have different addresses and are distinct.
	// We don't need to check if _size is the same; not random access!
	if (rhs.isEmpty()) {
		unsafeClear();
	}
	else if (isEmpty()) {
		copyCallingListEmpty(rhs);
	}
	else if (_size == rhs._size) {
		copyListsSameLength(rhs);
	}
	else if (_size > rhs._size) {
		copyCallingListShorter(rhs);
	}
	else if (_size < rhs._size) {
		copyCallingListLonger(rhs);
	}

	// Return calling list.
	return *this;
}

template <typename T>
ConcurrentList<T>& ConcurrentList<T>::operator=(
	ConcurrentList<T>&& rhs) noexcept
{
	// Check for self-assignment.
	if (this != &rhs) {
		std::unique_lock<std::shared_mutex> g(_mutex);
		// clear this
		unsafeClear();

		std::unique_lock<std::shared_mutex> rhs_g(rhs._mutex);
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
std::ostream& cm::operator<<(std::ostream& out,
	const ConcurrentList<T>& list)
{
	out << "[ ";
	bool first = true;
	{
		std::shared_lock<std::shared_mutex> g(list._mutex);
		for (auto it = list.unsafeBegin(); it != list.unsafeEnd(); ++it) {
			if (!first) {
				out << ", ";
			}
			first = false;
			out << *it;
		}
	}
	out << " ]";

	return out;
}

template <typename T>
void ConcurrentList<T>::copyCallingListEmpty(
	const ConcurrentList<T>& other) {
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
void ConcurrentList<T>::copyListsSameLength(
	const ConcurrentList<T>& other)
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
void ConcurrentList<T>::copyCallingListLonger(
	const ConcurrentList<T>& other)
{
	// Create curr for caller and other _head.
	ListNode<T>* curr = _head;
	ListNode<T>* otherCurr = other._head;
	// Iterate through, stopping at _tail node of other.
	while (otherCurr != nullptr) {
		curr->ele = otherCurr->ele;
		if (otherCurr->next() == nullptr) {
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
void ConcurrentList<T>::copyCallingListShorter(
	const ConcurrentList<T>& other)
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
ListIterator<T> ConcurrentList<T>::unsafeBegin() const
{
	return ListIterator<T>(_head);
}

template <typename T>
ListIterator<T> ConcurrentList<T>::unsafeEnd() const
{
	return ListIterator<T>(nullptr);
}

template <typename T>
bool ConcurrentList<T>::isEmpty() const
{
	std::shared_lock<std::shared_mutex> g(_mutex);
	return _head == nullptr && _tail == nullptr && _size == 0;
}

template <typename T>
std::size_t ConcurrentList<T>::size() const
{
	std::shared_lock<std::shared_mutex> g(_mutex);

	size_t size = 0;

	auto cur = _head;
	if (!cur) {
		return size;
	}

	while (cur) {
		++size;
		cur = cur->_next;
	}

	return size;
}

template <typename T>
std::size_t ConcurrentList<T>::unsafeSize() const
{
	return _size;
}

template <typename T>
std::optional<T> ConcurrentList<T>::front() const
{
	std::shared_lock<std::shared_mutex> g(_mutex);
	return !_head ? std::nullopt : std::optional<T>(_head->ele);
}

template <typename T>
std::optional<T> ConcurrentList<T>::back() const
{
	std::shared_lock<std::shared_mutex> g(_mutex);
	return !_tail ? std::nullopt : std::optional<T>(_tail->ele);
}

template <typename T>
const ListNode<T>* ConcurrentList<T>::pushFront(const T& element)
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

	ptr->_next = _head;
	_head->_prev = ptr;
	_head = ptr;

	// Increment size, node has been added.
	++_size;

	ptr = nullptr;
	return _head;
}

template <typename T>
std::optional<T> ConcurrentList<T>::popFront()
{
	std::unique_lock<std::shared_mutex> g(_mutex);

	if (!_head) {
		return std::nullopt;
	}
	
	if (_head == _tail) {
		T ele = _head->_ele;
		delete _head;
		_head = _tail = nullptr;
		_size = 0;
		return std::optional<T>(ele);
	}

	T ele = _head->_ele;
	auto tmp = _head;
	_head = _head->_next;
	_head->_prev = nullptr;
	delete tmp;
	tmp = nullptr;
	--_size;
	return std::optional<T>(ele);
}

template <typename T>
const ListNode<T>* ConcurrentList<T>::pushBack(const T& element)
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

	_tail->_next = ptr;
	ptr->_prev = _tail;
	_tail = ptr;

	++_size;	// Increment _size, node has been added.

	ptr = nullptr;
	return _tail;
}

template <typename T>
std::optional<T> ConcurrentList<T>::popBack()
{
	std::unique_lock<std::shared_mutex> g(_mutex);

	if (!_tail) {
		return std::nullopt;
	}
	
	if (_head == _tail) {
		T ele = _tail->_ele;
		delete _tail;
		_head = _tail = nullptr;
		_size = 0;
		return std::optional<T>(ele);
	}

	T ele = _tail->_ele;
	auto tmp = _tail;
	_tail = _tail->_prev;
	_tail->_next = nullptr;
	delete tmp;
	tmp = nullptr;
	--_size;
	return std::optional<T>(ele);
}

template <typename T>
const ListNode<T>* ConcurrentList<T>::get(const T& element)
{
	return search(element);
}

template <typename T>
std::optional<T> ConcurrentList<T>::get(const ListNode<T> *ptr)
{
	return !ptr ? std::nullopt : std::optional<T>(ptr->_ele);
}

template <typename T>
bool ConcurrentList<T>::remove(const T& element)
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
			popFront();
			return true;
		} else if (_tail->_ele == element) {
			// `else if` to lock control flow into `size > 1` for tail case.
			popBack();
			return true;
		}
	}

	// General case:
	// Already handled head and tail, so safe to assume `size() > 2`.
	ListNode<T> *node = const_cast<ListNode<T>*>(search(element));
	if (!node) {
		return false;
	}

	unlinkImpl(node);
	delete node;
	node = nullptr;

	std::unique_lock g{_mutex};
	--_size;

	return true;
}

template <typename T>
bool ConcurrentList<T>::unlinkImpl(ListNode<T> *ptr) {
	std::unique_lock lk{ptr->_mtx, std::defer_lock};
	std::unique_lock nlk{ptr->_next->_mtx, std::defer_lock};
	std::unique_lock plk{ptr->_prev->_mtx, std::defer_lock};
	std::lock(lk, nlk, plk);

	ptr->_next->_prev = ptr->_prev;
	ptr->_prev->_next = ptr->_next;
	ptr->_next = ptr->_prev = nullptr;
	
	return true;
}

template <typename T>
bool ConcurrentList<T>::unlink(const ListNode<T> *ptr)
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
			_head->next->_prev = nullptr;
			_head = _head->_next;
			node->_next = node->_prev = nullptr;
			return true;
		} else if (node == _tail) {
			// `else if` will trap us in a condition where `size() > 1`.
			_tail->_prev->_next = nullptr;
			_tail = _tail->_prev;
			node->_next = node->_prev = nullptr;
			return true;
		}
	}

	// General case:
	unlinkImpl(node);

	return true;
}

template <typename T>
const ListNode<T>* ConcurrentList<T>::search(const T& element) const
{
	// Guard if list is empty.
	std::shared_lock g{_mutex};
	if (!_head) {
		return nullptr;
	}
	if (!_head->_next) {
		return _head;
	}
	const ListNode<T> *curr = _head->_next;
	g.unlock();

	std::shared_lock lk{curr->_mtx};
	while (curr) {
		if (curr->_ele == element) {
			return curr;
		}

		const ListNode<T> *next = curr->_next;
		if (!next) {
			return nullptr;
		}

		std::shared_lock nlk{next->_mtx};
		lk.unlock();
		curr = next;
		lk = std::move(nlk);
	}

	return nullptr;
}

template <typename T>
bool ConcurrentList<T>::contains(const T& element) const
{
	return search(element);
}

template <typename T>
bool ConcurrentList<T>::contains(const ListNode<T> *ptr) const
{
	if (!ptr) {
		return false;
	}

	// Guard if list is empty.
	std::shared_lock g{_mutex};
	if (!_head) {
		return nullptr;
	}
	if (!_head->_next) {
		return _head;
	}
	const ListNode<T> *curr = _head->_next;
	g.unlock();

	// Iterate looking for matching address of parameter pointer against
	// a ConcurrentList pointer.
	std::shared_lock lk{curr->_mtx};
	while (curr) {
		if (curr == ptr) {
			return curr;
		}

		const ListNode<T> *next = curr->_next;
		if (!next) {
			return nullptr;
		}

		std::shared_lock nlk{next->_mtx};
		lk.unlock();
		curr = next;
		lk = std::move(nlk);
	}

	return nullptr;
}

template <typename T>
bool ConcurrentList<T>::isConsistent() const {
    std::shared_lock<std::shared_mutex> g(_mutex);

    auto node = _head;
    ListNode<T>* prev = nullptr;
    size_t count = 0;

    while (node) {
        if (node->_prev != prev) return false;
        prev = node;
        node = node->_next;
        ++count;
    }

    return count == _size;  // assuming _size is atomically maintained
}

template <typename T>
void ConcurrentList<T>::clear()
{
	std::shared_lock g{_mutex};
	if (_head) {
		ListNode<T>* curr = _head;
		delete _head;
		_head = nullptr;
		curr = curr->_next;
		g.unlock();
		std::unique_lock lk{curr->_mtx};
		ListNode<T>* currNext = curr->_next;
		while (currNext) {
			currNext = curr->_next;
			std::unique_lock nlk{currNext->_mtx};
			delete curr;
			curr = currNext;
			lk = std::move(nlk);
		}
		if (curr) {
			delete curr;
		}
		g.lock();
		_size = 0;
		_head = _tail = curr = currNext = nullptr;
	}
}

template <typename T>
void ConcurrentList<T>::unsafeClear()
{
    ListNode<T>* curr = _head;
    while (curr) {
        ListNode<T>* next = curr->_next;
        delete curr;
        curr = next;
    }
    _head = nullptr;
    _tail = nullptr;
    _size = 0;
}

