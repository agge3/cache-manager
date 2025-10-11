/**
 * @file concurrent-list-impl.hpp
 *
 * @author agge3
 * @version 1.0
 * @since 2024-09-21
 *
 * ConcurrentList and ConcurrentList helpers implementation.
 */
#include <iostream>
#include <exception>
#include <shared_mutex>

using namespace cm;

template <typename T>
typename ListIterator<T>::const_reference ListIterator<T>::operator*() {
    if (!_node) {
        throw std::runtime_error("Attempt to dereference a null iterator.");
    }
    return _node->getElement();
}

template <typename T>
typename ListIterator<T>::pointer ListIterator<T>::operator->() {
    if (!_node) {
        throw std::runtime_error("Attempt to dereference a null iterator.");
    }
    return &(_node->getElement());
}

template <typename T>
ListIterator<T>& ListIterator<T>::operator++() {
    if (_node) {
        _node = _node->getNext();
    }
    return *this;
}

template <typename T>
ListIterator<T> ListIterator<T>::operator++(int) {
    ListIterator tmp = *this;
    ++(*this);
    return tmp;
}

template <typename T>
ListIterator<T>& ListIterator<T>::operator--() {
    if (_node) {
        _node = _node->getPrev();
    }
    return *this;
}

template <typename T>
ListIterator<T> ListIterator<T>::operator--(int) {
    ListIterator tmp = *this;
    --(*this);
    return tmp;
}

template <typename T>
bool ListIterator<T>::operator==(const ListIterator& other) const {
    return _node == other._node;
}

template <typename T>
bool ListIterator<T>::operator!=(const ListIterator& other) const {
    return _node != other._node;
}

template <typename T>
ConcurrentList<T>::ConcurrentList(const ConcurrentList<T>& other) {
    // Check if list to be copied has any nodes.
    if (!other.empty()) {
        // lock rhs for reading
        std::shared_lock<std::shared_mutex> rhs_lk(rhs._mutex);
        // lock lhs (this) for writing
        std::unique_lock<std::shared_mutex> lk(_mutex);
        copyCallingListEmpty(other);
    }
}

template <typename T>
ConcurrentList<T>::ConcurrentList(ConcurrentList<T>&& other) noexcept {
    // Two steps:
    // 1. Steal the r-value list's resources.
    // 2. NULL the r-value list.
    // std::move does both, and we implement in terms of the move assignment
    // operator to reduce bugs and code duplication.
    *this = std::move(other); // SAFE: move assignment is read/write-locked
}

template <typename T>
ConcurrentList<T>& ConcurrentList<T>::operator=(
    const ConcurrentList<T>& rhs) {
    // Check if both lists have the same address; then they're the same.
    if (&rhs == this) {
        std::cerr << "Attempted assignment to self.";
        // Return out.
        return *this;
    }
    // We've proceeded, the lists have different addresses and are distinct.
    // We don't need to check if _size is the same; not random access!
    {
        // lock rhs for reading
        std::shared_lock<std::shared_mutex> rhs_lk(rhs._mutex);
        // lock lhs (this) for writing
        std::unique_lock<std::shared_mutex> lk(_mutex);
        if (rhs.empty()) {
            this->unsafeClear();
        } else if (this->empty()) {
            this->copyCallingListEmpty(rhs);
        } else if (this->_size == rhs._size) {
            this->copyListsSameLength(rhs);
        } else if (this->_size > rhs._size) {
            this->copyCallingListShorter(rhs);
        } else if (this->_size < rhs._size) {
            this->copyCallingListLonger(rhs);
        }
    }
    // Return calling list.
    return *this;
}

template <typename T>
ConcurrentList<T>& ConcurrentList<T>::operator=(
    ConcurrentList<T>&& rhs) noexcept {
    // Check for self-assignment.
    if (this != &rhs) {
        {
            // lock lhs (this) for writing
            std::unique_lock<std::shared_mutex> lk(_mutex);
            // Release the l-value list's resources.
            unsafeClear();
            // lock rhs for reading
            std::shared_lock<std::shared_mutex> rhs_lk(rhs._mutex);
            // Steal the r-value list's resources.
            _head = rhs._head;
            _tail = rhs._tail;
            _size = rhs._size;
        }
        {
            // now, lock rhs for writing before NULL-ing
            std::unique_lock<std::shared_mutex> rhs_lk(rhs._mutex);
            // NULL the r-value list.
            rhs._head = nullptr;
            rhs._tail = nullptr;
            rhs._size = 0;
        }
    }
    // Return l-value list.
    return *this;
}

template <typename T>
std::ostream& cm::operator<<(std::ostream& out, const ConcurrentList<T>& list) {
    static std::mutex mutex;
    out << "[ ";
    bool first = true;
    {
        std::lock_guard<std::mutex> lk(mutex);
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
    _head = new ListNode<T>(other._head->getElement());
    // curr at _head, otherCurr at other _head
    ListNode<T> *curr = _head;
    ListNode<T> *otherCurr = other._head;
    // Loop through all other list nodes and create for caller list.
    for (int i = 1; i < _size; ++i) {
        otherCurr = otherCurr->getNext();
        curr->setNext(new ListNode<T>(otherCurr->getElement(), nullptr, curr));
        curr = curr->getNext();
    }
    _tail = curr;
    // Cleanup dangling pointers.
    curr = otherCurr = nullptr;
}

template <typename T>
void ConcurrentList<T>::copyListsSameLength(
    const ConcurrentList<T>& other) {
    ListNode<T> *curr = _head;
    ListNode<T> *otherCurr = other._head;
    while (curr != nullptr) {
        curr->setElement(otherCurr->getElement());
        curr = curr->getNext();
        otherCurr = otherCurr->getNext();
    }
    // Cleanup dangling pointers.
    curr = otherCurr = nullptr;
}

template <typename T>
void ConcurrentList<T>::copyCallingListLonger(
    const ConcurrentList<T>& other) {
    // Create curr for caller and other _head.
    ListNode<T>* curr = _head;
    ListNode<T>* otherCurr = other._head;
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
    // curr at new _tail for caller, delete everything after.
    while (curr != nullptr) {
        delete curr;
        curr = curr->getNext();
    }
    // Cleanup: _size is equal, assign _tail, and delete dangling pointers.
    _size = other._size;
    curr = otherCurr = nullptr;
}

template <typename T>
void ConcurrentList<T>::copyCallingListShorter(
    const ConcurrentList<T>& other) {
    ListNode<T>* curr = _head;
    ListNode<T>* otherCurr = other._head;
    while (curr != nullptr) {
        curr->setElement(otherCurr->getElement());
        curr = curr->getNext();
        otherCurr = otherCurr->getNext();
    }
    // Reset current to be at _tail.
    curr = _tail;
    // Second loop to create new nodes for remaining nodes of caller.
    while (otherCurr != nullptr) {
        curr->setNext(new ListNode<T>(otherCurr->getElement(), nullptr, curr));
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

template <typename T>
ListIterator<T> ConcurrentList<T>::unsafeBegin() const {
    std::shared_lock<std::shared_mutex> lk(_mutex);
    return ListIterator<T>(_head);
}

template <typename T>
ListIterator<T> ConcurrentList<T>::unsafeEnd() const {
    std::shared_lock<std::shared_mutex> lk(_mutex);
    return ListIterator<T>(nullptr);
}

template <typename T>
bool ConcurrentList<T>::empty() const {
    std::shared_lock<std::shared_mutex> lk(_mutex);
    return _head == nullptr && _tail == nullptr && _size == 0;
}

template <typename T>
std::size_t ConcurrentList<T>::size() const {
    std::shared_lock<std::shared_mutex> lk(_mutex);
    return _size;
}

template <typename T>
std::optional<T> ConcurrentList<T>::front() const {
    std::shared_lock<std::shared_mutex> lk(_mutex);
    return !empty() ? std::optional<T>(_head->getElement()) : std::nullopt;
}

template <typename T>
std::optional<T> ConcurrentList<T>::back() const {
    std::shared_lock<std::shared_mutex> lk(_mutex);
    return !empty() ? std::optional<T>(_tail->getElement()) : std::nullopt;
}

template <typename T>
const ListNode<T>* ConcurrentList<T>::pushFront(const T& element) {
    // heap allocate before write lock to optimize allocation out of
    // critical section
    auto *ptr = new ListNode<T>(element);
    std::unique_lock<std::shared_mutex> lk(_mutex);
    if (empty()) { // LOCK: TOCTOU
        _head = ptr;
        _tail = _head;
        // Increment size, node has been added.
        ++_size;
        ptr = nullptr;
        return _head;
    }
    ptr->setPrev(nullptr);
    ptr->setNext(head);
    _head->setPrev(ptr);
    _head = ptr;
    // Increment size, node has been added.
    ++_size;
    ptr = nullptr;
    return _head;
}

template <typename T>
std::optional<T> ConcurrentList<T>::popFront() {
    std::unique_lock<std::shared_mutex> lk(_mutex);
    if (_head == nullptr) {
        return std::nullopt;
    }
    std::optional<T> ele = _head->getElement();
    if (_head == _tail) {
        unsafeClear();
        return ele;
    }
    auto *ptr = _head->getNext();
    delete _head;
    _head = ptr;
    _head->setPrev(nullptr);
    --_size;
    ptr = nullptr;
    return ele;
}

template <typename T>
const ListNode<T>* ConcurrentList<T>::pushBack(const T& element) {
    // heap allocate before write lock to optimize allocation out of
    // critical section
    auto *ptr = new ListNode<T>(element);
    std::unique_lock<std::shared_mutex> lk(_mutex);
    if (_head == nullptr) {
        _head = ptr;
        _tail = _head;
        ++_size; // Increment _size, node has been added.
        return _tail;
    }
    ptr->setNext(nullptr);
    ptr->setPrev(_tail);
    _tail->setNext(ptr);
    _tail = ptr;
    return _tail;
}

template <typename T>
std::optional<T> ConcurrentList<T>::popBack() {
    std::unique_lock<std::shared_mutex> write_lk(_mutex);
    if (empty()) { // SAFE: empty is read-locked
        return std::nullopt;
    }
    std::optional<T> ele = back(); // SAFE: back is read-locked
    {
        std::shared_lock<std::shared_mutex> read_lk(_mutex);
        if (_head == _tail) {
            read_lk.unlock();
            unsafeClear();
            return ele;
        }
    }
    {
        std::unique_lock<std::shared_mutex> lk(_mutex);
        ListNode<T> *ptr = _tail->getPrev();
        delete _tail;
        _tail = ptr;
        _tail->setNext(nullptr);
        --_size;
    }
    ptr = nullptr;
    return ele;
}

template <typename T>
const ListNode<T>* ConcurrentList<T>::get(const T& element) {
    return search(element); // SAFE: search is read-locked
}

template <typename T>
std::optional<T> ConcurrentList<T>::get(const ListNode<T> *ptr) {
    // SAFE: can always dereference the pointer given a pointer without a lock
    return ptr ? std::optional<T>(ptr->getElement()) : std::nullopt;
}

template <typename T>
bool ConcurrentList<T>::remove(const T& element) {
    // Only remove if list has nodes.
    if (empty()) { // SAFE: empty is read-locked
        return false;
    }
    // Handle head and tail cases.
    {
        std::shared_lock<std::shared_mutex> lk(_mutex);
        if (_head->getElement() == element) {
            lk.unlock();
            popFront(); // SAFE: popFront is write-locked
            return true;
        } else if (_tail->getElement() == element) {
            // else if to lock control flow into size > 1 for tail case.
            lk.unlock();
            popBack(); // SAFE: popBack is write-locked
            return true;
        }
    }
    // General case:
    // Already handled head and tail, so safe to assume size() > 2.
    // SAFE: search is read-locked
    ListNode<T> *node = const_cast<ListNode<T>*>(search(element));
    if (!node) {
        return false;
    }
    std::unique_lock<std::shared_mutex> lk(_mutex);
    node->getNext()->setPrev(node->getPrev());
    node->getPrev()->setNext(node->getNext());
    --_size;
    delete node;
    node = nullptr;
    return true;
}

template <typename T>
bool ConcurrentList<T>::removeAndPushFront(const ListNode<T> *ptr) {
    // Empty list or nullptr node.
    if (!ptr || empty()) { // SAFE: given ptr, empty is read-locked
        return false;
    }
    // Cast away the client's const, we're in our owned instance.
    ListNode<T> *node = const_cast<ListNode<T>*>(ptr);
    // Handle head and tail.
    {
        std::shared_lock<std::shared_mutex> read_lk(_mutex);
        if (node == _head) {
            // Do nothing, but return TRUE for success.
            return true;
        } else if (node == _tail) {
            // else if will trap us in a condition where size() > 1.
            std::unique_lock<std::shared_mutex> write_lk(_mutex);
            _tail->getPrev()->setNext(nullptr);
            _tail = _tail->getPrev();
            _head->setPrev(node);
            node->setPrev(nullptr);
            node->setNext(_head);
            _head = node;
            return true;
        }
    }
    // General case:
    std::unique_lock<std::shared_mutex> lk(_mutex);
    node->getPrev()->setNext(node->getNext());
    node->getNext()->setPrev(node->getPrev());
    _head->setPrev(node);
    node->setNext(_head);
    _head = node;
    return true;
}

template <typename T>
const ListNode<T>* ConcurrentList<T>::search(const T& element) const {
    // Guard if list is empty.
    if (empty()) { // SAFE: empty is read-locked
        return nullptr;
    }
    std::shared_lock<std::shared_mutex> lk(_mutex);
    const ListNode<T> *curr = _head;
    while (curr) {
        if (curr->getElement() == element) {
            return curr;
        }
        curr = curr->getNext();
    }
    return nullptr;
}

template <typename T>
bool ConcurrentList<T>::contains(const T& element) const {
    return search(element); // SAFE: search is read-locked
}

template <typename T>
bool ConcurrentList<T>::contains(const ListNode<T> *ptr) const {
    // Guard if list is empty.
    if (empty()) { // SAFE: empty is read-locked
        return false;
    }
    // Iterate looking for matching address of parameter pointer against
    // a ConcurrentList pointer.
    std::shared_lock<std::shared_mutex> lk(_mutex);
    const ListNode<T> *curr = _head;
    while (curr) {
        if (curr == ptr) {
            return true;
        }
        curr = curr->getNext();
    }
    return false;
}

template <typename T>
void ConcurrentList<T>::unsafeClear() {
    if (!empty()) {
        ListNode<T>* curr = _head;
        ListNode<T>* currNext;
        while (curr != nullptr) {
            currNext = curr->getNext();
            delete curr;
            curr = currNext;
        }
        _size = 0;
        _head = _tail = curr = currNext = nullptr;
    }
}

