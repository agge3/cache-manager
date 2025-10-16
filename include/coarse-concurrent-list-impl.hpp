/**
 * @file coarse-concurrent-list-impl.hpp
 *
 * @author agge3
 * @version 1.0
 * @since 2024-09-21
 *
 * CoarseConcurrentList concrete implementation.
 */
#include "concurrent-list.hpp"
#include <exception>
#include <iostream>
#include <shared_mutex>

using namespace cm;

// need to inline to not lose lock, while reusing code (like a function call).
// wrapper in a lamda, so it's an expression.
#define POP_FRONT                                                              \
	([&]() -> std::optional<T> {                                               \
		if (!this->_head) {                                                    \
			return std::nullopt;                                               \
		}                                                                      \
                                                                               \
		if (this->_head == this->_tail) {                                      \
			T ele = this->_head->ele;                                          \
			delete this->_head;                                                \
			this->_head = this->_tail = nullptr;                               \
			this->_size = 0;                                                   \
			return std::optional<T>(ele);                                      \
		}                                                                      \
                                                                               \
		T ele = this->_head->ele;                                              \
		auto tmp = this->_head;                                                \
		this->_head = this->_head->next;                                       \
		this->_head->prev = nullptr;                                           \
		delete tmp;                                                            \
		tmp = nullptr;                                                         \
		--this->_size;                                                         \
		return std::optional<T>(ele);                                          \
	})()
#define POP_BACK                                                               \
	([&]() -> std::optional<T> {                                               \
		if (!this->_tail) {                                                    \
			return std::nullopt;                                               \
		}                                                                      \
                                                                               \
		if (this->_head == this->_tail) {                                      \
			T ele = this->_tail->ele;                                          \
			delete this->_tail;                                                \
			this->_head = this->_tail = nullptr;                               \
			this->_size = 0;                                                   \
			return std::optional<T>(ele);                                      \
		}                                                                      \
                                                                               \
		T ele = this->_tail->ele;                                              \
		auto tmp = this->_tail;                                                \
		this->_tail = this->_tail->prev;                                       \
		this->_tail->next = nullptr;                                           \
		delete tmp;                                                            \
		tmp = nullptr;                                                         \
		--this->_size;                                                         \
		return std::optional<T>(ele);                                          \
	})()

// NOTE: macro parameter must NOT be `ele`: if `ele`, will shadow `curr->ele`
// (will text replace `ele` with macro parameter)
#define SEARCH_ELE(e)                                                          \
	([&]() -> const typename CoarseConcurrentList<T>::ListNodeT * {            \
		/* Guard if list is empty. */                                          \
		if (!this->_head) {                                                    \
			return nullptr;                                                    \
		}                                                                      \
		const typename CoarseConcurrentList<T>::ListNodeT *curr = this->_head; \
		while (curr) {                                                         \
			if (curr->ele == (e)) {                                            \
				return curr;                                                   \
			}                                                                  \
			curr = curr->next;                                                 \
		}                                                                      \
		return nullptr;                                                        \
	})()

#define SEARCH_PTR(p)                                                          \
	([&]() -> const typename CoarseConcurrentList<T>::ListNodeT * {            \
		/* Guard if list is empty. */                                          \
		if (!this->_head) {                                                    \
			return nullptr;                                                    \
		}                                                                      \
		auto *curr = this->_head;                                              \
		while (curr) {                                                         \
			if (curr == (p)) {                                                 \
				return curr;                                                   \
			}                                                                  \
			curr = curr->next;                                                 \
		}                                                                      \
		return nullptr;                                                        \
	})()

template <typename T> size_t CoarseConcurrentList<T>::size() const {
	std::shared_lock<std::shared_mutex> rg(this->_mutex);
	return this->_size;
}

template <typename T>
const typename CoarseConcurrentList<T>::ListNodeT *
CoarseConcurrentList<T>::pushFront(const T &element) {
	// heap allocate before write lock to optimize allocation out of
	// critical section
	auto *ptr = new typename CoarseConcurrentList<T>::ListNodeT(element);
	std::unique_lock<std::shared_mutex> wg(this->_mutex);
	if (!this->_head) {
		this->_head = ptr;
		this->_tail = this->_head;
		// Increment size, node has been added.
		++this->_size;
		ptr = nullptr;
		return this->_head;
	}
	ptr->prev = nullptr;
	ptr->next = this->_head;
	this->_head->prev = ptr;
	this->_head = ptr;
	// Increment size, node has been added.
	++this->_size;
	ptr = nullptr;
	return this->_head;
}

template <typename T> std::optional<T> CoarseConcurrentList<T>::popFront() {
	std::unique_lock<std::shared_mutex> wg(this->_mutex);
	return POP_FRONT;
}

template <typename T>
const typename CoarseConcurrentList<T>::ListNodeT *
CoarseConcurrentList<T>::pushBack(const T &element) {
	// heap allocate before write lock to optimize allocation out of
	// critical section
	auto *ptr = new typename CoarseConcurrentList<T>::ListNodeT(element);
	std::unique_lock<std::shared_mutex> wg(this->_mutex);
	if (this->_head == nullptr) {
		this->_head = ptr;
		this->_tail = this->_head;
		++this->_size; // Increment this->_size, node has been added.
		return this->_tail;
	}
	ptr->next = nullptr;
	ptr->prev = this->_tail;
	this->_tail->next = ptr;
	this->_tail = ptr;
	++this->_size;
	return this->_tail;
}

template <typename T> std::optional<T> CoarseConcurrentList<T>::popBack() {
	std::unique_lock<std::shared_mutex> rg(this->_mutex);
	return POP_BACK;
}

template <typename T>
const typename CoarseConcurrentList<T>::ListNodeT *
CoarseConcurrentList<T>::get(const T &element) {
	std::shared_lock<std::shared_mutex> rg(this->_mutex);
	return SEARCH_ELE(element);
}

template <typename T>
std::optional<T> CoarseConcurrentList<T>::get(
	const typename CoarseConcurrentList<T>::ListNodeT *ptr) {
	// NOTE: need to global read lock, or else pointer can be invalidated by
	// other threads during dereference
	std::shared_lock<std::shared_mutex> rg(this->_mutex);
	return ptr ? std::optional<T>(ptr->ele) : std::nullopt;
}

template <typename T> bool CoarseConcurrentList<T>::remove(const T &element) {
	// simple O(1) cases (just hold write lock because head check is cheap):
	{
		std::unique_lock<std::shared_mutex> wg(this->_mutex);
		// Only remove if list has nodes.
		if (!this->_head) {
			return false;
		}

		// Handle head and tail cases.
		if (this->_head->ele == element) {
			POP_FRONT;
			return true;
		} else if (this->_tail->ele == element) {
			// else if to lock control flow into size > 1 for tail case.
			POP_BACK;
			return true;
		}
	}
	// NOTE: Handled head and tail, so safe to assume size() > 2.

	// try optimistic read under read lock only first:
	const typename CoarseConcurrentList<T>::ListNodeT *node;
	{
		std::shared_lock<std::shared_mutex> rg(this->_mutex);
		node = const_cast<typename CoarseConcurrentList<T>::ListNodeT *>(
			SEARCH_ELE(element));
		if (!node) {
			return false;
		}
	}

	// validate and remove:
	{
		std::unique_lock<std::shared_mutex> wg(this->_mutex);
		// verify consistency
		if (node == this->_head || node == this->_tail ||
			node->prev->next != node || node->next->prev != node) {
			// NOTE: two choices:
			// 	(1) recursively call, with possible unbounded removal loop
			// 	(2) find under write lock and guarantee removal happens in this
			// 	    function call, even though optimistic search failed
			// we choose (2), so that unbounded edge-case can't occur
			node = const_cast<typename CoarseConcurrentList<T>::ListNodeT *>(
				SEARCH_ELE(element));
		}

		node->next->prev = node->prev;
		node->prev->next = node->next;
		--this->_size;

		delete node;
		node = nullptr;
		return true;
	}
}

template <typename T>
bool CoarseConcurrentList<T>::remove(
	const typename CoarseConcurrentList<T>::ListNodeT *node) {
	if (!node) {
		return false;
	}

	std::unique_lock<std::shared_mutex> wg(this->_mutex);
	// Cast away the client's const, we're in our owned instance.
	auto *mut = const_cast<typename CoarseConcurrentList<T>::ListNodeT *>(node);
	// Only remove if list has nodes.
	if (!this->_head) {
		return false;
	}

	// Handle head and tail cases.
	if (this->_head == node) {
		POP_FRONT;
		return true;
	} else if (this->_tail == node) {
		// else if to lock control flow into size > 1 for tail case.
		POP_BACK;
		return true;
	}

	// General case:
	// Already handled head and tail, so safe to assume size() > 2.
	mut->next->prev = mut->prev;
	mut->prev->next = mut->next;
	--this->_size;

	delete mut;
	mut = nullptr;
	return true;
}

template <typename T>
bool CoarseConcurrentList<T>::unlink(
	const typename CoarseConcurrentList<T>::ListNodeT *node) {
	return true;
}

template <typename T>
bool CoarseConcurrentList<T>::removeAndPushFront(
	const typename CoarseConcurrentList<T>::ListNodeT *node) {
	// don't lock list if nullptr
	if (!node) {
		return false;
	}

	std::unique_lock<std::shared_mutex> wg(this->_mutex);
	// Cast away the client's const, we're in our owned instance.
	auto *mut = const_cast<typename CoarseConcurrentList<T>::ListNodeT *>(node);
	// empty list
	if (!this->_head) {
		return false;
	}

	// Handle head.
	if (mut == this->_head) {
		// Do nothing, but return TRUE for success.
		return true;
	}

	// Set up for tail vs. non-tail.
	if (mut == this->_tail) {
		// else if will trap us in a condition where size() > 1.
		this->_tail = this->_tail->prev;
		this->_tail->next = nullptr;
	} else {
		mut->prev->next = mut->next;
		mut->next->prev = mut->prev;
	}

	// Finish swapping node to front.
	this->_head->prev = mut;
	mut->prev = nullptr;
	mut->next = this->_head;
	this->_head = mut;

	return true;
}

template <typename T>
const typename CoarseConcurrentList<T>::ListNodeT *
CoarseConcurrentList<T>::search(const T &element) const {
	std::shared_lock<std::shared_mutex> rg(this->_mutex);
	return const_cast<typename CoarseConcurrentList<T>::ListNodeT *>(
		SEARCH_ELE(element));
}

template <typename T>
bool CoarseConcurrentList<T>::contains(const T &element) const {
	std::shared_lock<std::shared_mutex> rg(this->_mutex);
	return SEARCH_ELE(element);
}

template <typename T>
bool CoarseConcurrentList<T>::contains(
	const typename CoarseConcurrentList<T>::ListNodeT *ptr) const {
	std::shared_lock<std::shared_mutex> rg(this->_mutex);
	return SEARCH_PTR(ptr);
}

template <typename T> void CoarseConcurrentList<T>::clear() {
	// don't global read lock first, because if elements exist then we're
	// always going to modify - save the lock allocation
	std::unique_lock<std::shared_mutex> wg(this->_mutex);
	if (this->_head) {
		typename CoarseConcurrentList<T>::ListNodeT *curr = this->_head;
		typename CoarseConcurrentList<T>::ListNodeT *currNext;
		while (curr != nullptr) {
			currNext = curr->next;
			delete curr;
			curr = currNext;
		}
		this->_size = 0;
		this->_head = this->_tail = curr = currNext = nullptr;
	}
}
