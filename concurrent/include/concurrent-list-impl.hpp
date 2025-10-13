/**
 * @file concurrent-list-impl.hpp
 *
 * @author agge3
 * @version 1.0
 * @since 2024-09-21
 *
 * IConcurrentList interface implementation.
 */

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
IConcurrentList<T>::IConcurrentList(const IConcurrentList<T>& other)
{
	// Check if list to be copied has any nodes.
	// copy with global read lock on other to snapshot
	std::shared_lock<std::shared_mutex> other_g(other._mutex);
	if (!other.isEmpty()) {
		copyCallingListEmpty(other);
	}
}

template <typename T>
IConcurrentList<T>::IConcurrentList(IConcurrentList<T>&& other) noexcept
{
	// Two steps:
	// 	1. Steal the r-value rist's resources.
	// 	2. NULL the r-value list.

	// std::move does both, and we implement in terms of the move assignment
	// operator to reduce bugs and code duplication.
	*this = std::move(other);
}

template <typename T>
IConcurrentList<T>& IConcurrentList<T>::operator=(
	const IConcurrentList<T>& rhs)
{
	// Check if both lists have the same address; then they're the same.
	if (&rhs == this) {
		std::cerr << "Attempted assignment to self.";
		// Return out.
		return *this;
	}

	// acquire both global locks in fixed order to avoid deadlock:
	// xxx can we acquire other read and self write?
	std::scoped_lock lock g(_mutex, rhs._mutex);

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
IConcurrentList<T>& IConcurrentList<T>::operator=(
	IConcurrentList<T>&& rhs) noexcept
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
	const IConcurrentList<T>& list)
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
void IConcurrentList<T>::copyCallingListEmpty(
	const IConcurrentList<T>& other) {
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
void IConcurrentList<T>::copyListsSameLength(
	const IConcurrentList<T>& other)
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
void IConcurrentList<T>::copyCallingListLonger(
	const IConcurrentList<T>& other)
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
		auto tmp = curr;
		curr = curr->next;
		delete tmp;
		tmp = nullptr;
	}
	// Cleanup: _size is equal, assign _tail, and delete dangling pointers.
	_size = other._size;
	curr = otherCurr = nullptr;
}

template <typename T>
void IConcurrentList<T>::copyCallingListShorter(
	const IConcurrentList<T>& other)
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
bool IConcurrentList<T>::isEmpty() const
{
	std::shared_lock<std::shared_mutex> g(_mutex);
	return _head == nullptr && _tail == nullptr && _size == 0;
}

template <typename T>
std::optional<T> IConcurrentList<T>::front() const
{
	std::shared_lock<std::shared_mutex> g(_mutex);
	return !_head ? std::nullopt : std::optional<T>(_head->ele);
}

template <typename T>
std::optional<T> IConcurrentList<T>::back() const
{
	std::shared_lock<std::shared_mutex> g(_mutex);
	return !_tail ? std::nullopt : std::optional<T>(_tail->ele);
}

template <typename T>
std::size_t IConcurrentList<T>::unsafeSize() const
{
	return _size;
}

template <typename T>
ListIterator<T> IConcurrentList<T>::unsafeBegin() const
{
	return ListIterator<T>(_head);
}

template <typename T>
ListIterator<T> IConcurrentList<T>::unsafeEnd() const
{
	return ListIterator<T>(nullptr);
}

template <typename T>
void IConcurrentList<T>::unsafeClear()
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
