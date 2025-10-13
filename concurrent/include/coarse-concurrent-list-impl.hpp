/**
 * @file coarse-concurrent-list-impl.hpp
 *
 * @author agge3
 * @version 1.0
 * @since 2024-09-21
 *
 * CoarseConcurrentList concrete implementation.
 */
#include <iostream>
#include <exception>
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

#define SEARCH_ELE(ele) \
	do {	\
	    /* Guard if list is empty. */ \
	    if (!_head) { \
	        return nullptr;	\
	    }	\
	    const ListNode<T> *curr = _head;	\
	    while (curr) {	\
	        if (curr->ele == (ele)) {	\
	            return curr;	\
	        }	\
	        curr = curr->next;	\
	    }	\
	    return nullptr;	\
	} while (0)

#define SEARCH_PTR(PTR) \
	do {	\
	    /* Guard if list is empty. */ \
	    if (!_head) { \
	        return nullptr;	\
	    }	\
	    const ListNode<T> *curr = _head;	\
	    while (curr) {	\
	        if (curr == (ptr)) {	\
	            return curr;	\
	        }	\
	        curr = curr->next;	\
	    }	\
	    return nullptr;	\
	} while (0)

template <typename T>
std::size_t CoarseConcurrentList<T>::size() const {
    std::shared_lock<std::shared_mutex> lk(_mutex);
    return _size;
}

template <typename T>
const ListNode<T>* CoarseConcurrentList<T>::pushFront(const T& element) {
    // heap allocate before write lock to optimize allocation out of
    // critical section
    auto *ptr = new ListNode<T>(element);
    std::unique_lock<std::shared_mutex> lk(_mutex);
    if (!_head) {
        _head = ptr;
        _tail = _head;
        // Increment size, node has been added.
        ++_size;
        ptr = nullptr;
        return _head;
    }
	ptr->prev = nullptr;
	ptr->next = _head;
	_head->prev = ptr;
    _head = ptr;
    // Increment size, node has been added.
    ++_size;
    ptr = nullptr;
    return _head;
}

template <typename T>
std::optional<T> CoarseConcurrentList<T>::popFront() {
    std::unique_lock<std::shared_mutex> lk(_mutex);
	return POP_FRONT;
}

template <typename T>
const ListNode<T>* CoarseConcurrentList<T>::pushBack(const T& element) {
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
	ptr->next = nullptr;
	ptr->prev = _tail;
	_tail->next = ptr;
    _tail = ptr;
    return _tail;
}

template <typename T>
std::optional<T> CoarseConcurrentList<T>::popBack() {
    std::unique_lock<std::shared_mutex> write_lk(_mutex);
	return POP_BACK;
}

template <typename T>
const ListNode<T>* CoarseConcurrentList<T>::get(const T& element) {
	std::shared_lock<std::shared_mutex> rg(_mutex);
	return SEARCH_ELE(element);
}

template <typename T>
std::optional<T> CoarseConcurrentList<T>::get(const ListNode<T> *ptr) {
	// NOTE: need to global read lock, or else pointer can be invalidated by
	// other threads during dereference
	std::shared_lock<std::shared_mutex> rg(_mutex);
    return ptr ? std::optional<T>(ptr->ele) : std::nullopt;
}

template <typename T>
bool CoarseConcurrentList<T>::remove(const T& element) {
    std::shared_lock<std::shared_mutex> read_lk(_mutex);
    // Only remove if list has nodes.
	if (!_head) {
		return false;
	}

    // Handle head and tail cases.
    if (_head->ele == element) {
		std::unique_lock<std::shared_mutex> write_lk(_mutex);
		POP_FRONT;
        return true;
    } else if (_tail->ele == element) {
        // else if to lock control flow into size > 1 for tail case.
		std::unique_lock<std::shared_mutex> write_lk(_mutex);
		POP_BACK;
        return true;
    }

    // General case:
    // Already handled head and tail, so safe to assume size() > 2.
    ListNode<T> *node = const_cast<ListNode<T>*>(SEARCH_ELE(element));
    if (!node) {
        return false;
    }

    std::unique_lock<std::shared_mutex> write_lk(_mutex);
	node->next->prev = node->prev;
	node->prev->next = node->next;
    --_size;

    delete node;
    node = nullptr;
    return true;
}

template <typename T>
bool CoarseConcurrentList<T>::remove(const ListNode<T>& *node) {
    std::shared_lock<std::shared_mutex> read_lk(_mutex);
    // Only remove if list has nodes.
	if (!_head) {
		return false;
	}

    // Handle head and tail cases.
    if (_head == node) {
		std::unique_lock<std::shared_mutex> write_lk(_mutex);
		POP_FRONT;
        return true;
    } else if (_tail == node) {
        // else if to lock control flow into size > 1 for tail case.
		std::unique_lock<std::shared_mutex> write_lk(_mutex);
		POP_BACK;
        return true;
    }

    // General case:
    // Already handled head and tail, so safe to assume size() > 2.
    ListNode<T> *mut = const_cast<ListNode<T>*>(node);

    std::unique_lock<std::shared_mutex> write_lk(_mutex);
	mut->next->prev = mut->prev;
	mut->prev->next = mut->next;
    --_size;

    delete mut;
    mut = nullptr;
    return true;
}

template <typename T>
bool CoarseConcurrentList<T>::removeAndPushFront(const ListNode<T> *ptr) {
	// don't lock list if nullptr
	if (!ptr) {
		return false;
	}

	std::shared_lock<std::shared_mutex> rg(_mutex);
    // empty list
	if (!_head) {
        return false;
    }
    // Cast away the client's const, we're in our owned instance.
    ListNode<T> *mut = const_cast<ListNode<T>*>(ptr);

    // Handle head and tail.
    if (node == _head) {
        // Do nothing, but return TRUE for success.
        return true;
	}
    if (node == _tail) {
        // else if will trap us in a condition where size() > 1.
        std::unique_lock<std::shared_mutex> wg(_mutex);
		_tail->prev->next = nullptr;
		_tail = _tail->prev;
		_head->prev = mut;
		mut->prev = nullptr;
		mut->next = _head;
		_head = mut;
        return true;
    }

    // General case:
    std::unique_lock<std::shared_mutex> wg(_mutex);
	mut->prev->next = mut->next;
	mut->next->prev = mut->prev;
	_head->prev = mut;
	mut->next = _head;
	_head = mut;
    return true;
}

template <typename T>
const ListNode<T>* CoarseConcurrentList<T>::search(const T& element) const {
	std::shared_lock<std::shared_mutex> rg(_mutex);
	return SEARCH_ELE(element);
}

template <typename T>
bool CoarseConcurrentList<T>::contains(const T& element) const {
	std::shared_lock<std::shared_mutex> rg(_mutex);
	return SEARCH_ELE(element);

}

template <typename T>
bool CoarseConcurrentList<T>::contains(const ListNode<T> *ptr) const {
	std::shared_lock<std::shared_mutex> rg(_mutex);
	return SEARCH_PTR(ptr);
}

template <typename T>
void CoarseConcurrentList<T>::clear() {
	// don't global read lock first, because if elements exist then we're
	// always going to modify - save the lock allocation
	std::unique_lock<std::shared_mutex> wg(_mutex);
    if (_head) {
        ListNode<T>* curr = _head;
        ListNode<T>* currNext;
        while (curr != nullptr) {
            currNext = curr->next;
            delete curr;
            curr = currNext;
        }
        _size = 0;
        _head = _tail = curr = currNext = nullptr;
    }
}

