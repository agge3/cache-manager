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
// wrapper in a lamda, so it's an expression.
#define POP_FRONT \
	([&]() -> std::optional<T> {	\
		if (!this->_head) {	\
			return std::nullopt;	\
		}	\
			\
		if (this->_head == this->_tail) {	\
			T ele = this->_head->ele;	\
			delete this->_head;	\
			this->_head = this->_tail = nullptr;	\
			this->_size = 0;	\
			return std::optional<T>(ele);	\
		}	\
	\
		T ele = this->_head->ele;	\
		auto tmp = this->_head;	\
		this->_head = this->_head->next;	\
		this->_head->prev = nullptr;	\
		delete tmp;	\
		tmp = nullptr;	\
		--this->_size;	\
		return std::optional<T>(ele);	\
	})()
#define POP_BACK	\
	([&]() -> std::optional<T> {	\
		if (!this->_tail) {	\
			return std::nullopt;	\
		}	\
			\
		if (this->_head == this->_tail) {	\
			T ele = this->_tail->ele;	\
			delete this->_tail;	\
			this->_head = this->_tail = nullptr;	\
			this->_size = 0;	\
			return std::optional<T>(ele);	\
		}	\
		\
		T ele = this->_tail->ele;	\
		auto tmp = this->_tail;	\
		this->_tail = this->_tail->prev;	\
		this->_tail->next = nullptr;	\
		delete tmp;	\
		tmp = nullptr;	\
		--this->_size;	\
		return std::optional<T>(ele);	\
	})()

template <typename T>
std::size_t FineConcurrentList<T>::size() const
{
	std::shared_lock<std::shared_mutex> rg(this->_mutex);

	size_t size = 0;

	auto cur = this->_head;
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
const typename FineConcurrentList<T>::ListNodeT* FineConcurrentList<T>::pushFront(const T& element)
{
	// heap allocate before write lock to optimize allocation out of
	// critical section
	auto *ptr = new typename FineConcurrentList<T>::ListNodeT(element);

	std::unique_lock<std::shared_mutex> wg(this->_mutex);

	if (!this->_head) {
		this->_head = this->_tail = ptr;

		// Increment size, node has been added.
		++this->_size;

		ptr = nullptr;
		return this->_head;
	}

	ptr->next = this->_head;
	this->_head->prev = ptr;
	this->_head = ptr;

	// Increment size, node has been added.
	++this->_size;

	ptr = nullptr;
	return this->_head;
}

template <typename T>
std::optional<T> FineConcurrentList<T>::popFront()
{
	std::unique_lock<std::shared_mutex> wg(this->_mutex);
	return POP_FRONT;
}

template <typename T>
const typename FineConcurrentList<T>::ListNodeT* FineConcurrentList<T>::pushBack(const T& element)
{
	// heap allocate before write lock to optimize allocation out of
	// critical section
	auto *ptr = new typename FineConcurrentList<T>::ListNodeT(element);

	std::unique_lock<std::shared_mutex> wg(this->_mutex);
	if (!this->_head) {
		this->_head = this->_tail = ptr;
		++this->_size;	// Increment this->_size, node has been added.
		return this->_head;
	}

	this->_tail->next = ptr;
	ptr->prev = this->_tail;
	this->_tail = ptr;

	++this->_size;	// Increment this->_size, node has been added.

	ptr = nullptr;
	return this->_tail;
}

template <typename T>
std::optional<T> FineConcurrentList<T>::popBack()
{
	std::unique_lock<std::shared_mutex> wg(this->_mutex);
	return POP_BACK;
}

template <typename T>
const typename FineConcurrentList<T>::ListNodeT* FineConcurrentList<T>::get(const T& element)
{
	return search(element);
}

template <typename T>
std::optional<T> FineConcurrentList<T>::get(const typename FineConcurrentList<T>::ListNodeT *ptr)
{
	return !ptr ? std::nullopt : std::optional<T>(ptr->ele);
}

template <typename T>
bool FineConcurrentList<T>::remove(const T& element)
{
	// fast checks under global lock (write lock because head/tail check is
	// cheap and modification can occur in same operation):
	{
		std::unique_lock wg{this->_mutex};

		// Only remove if list has nodes.
		if (!this->_head) {
			return false;
		}

		// Handle head and tail cases.
		if (this->_head->ele == element) {
			POP_FRONT;
			return true;
		} else if (this->_tail->ele == element) {
			// `else if` to lock control flow into `size > 1` for tail case.
			POP_BACK;
			return true;
		}
	}
	// NOTE: Handled head and tail, so safe to assume `size() > 2`.

	// General case:
	typename FineConcurrentList<T>::ListNodeT *node = const_cast<typename FineConcurrentList<T>::ListNodeT*>(search(element));
	if (!node) {
		return false;
	}

	unlink(node);
	delete node;
	node = nullptr;

	std::unique_lock g{this->_mutex};
	--this->_size;

	return true;
}

template <typename T>
bool FineConcurrentList<T>::remove(const typename FineConcurrentList<T>::ListNodeT *node)
{
	if (!node) {
		return false;
	}

	// fast checks under global lock (write lock because head/tail check is
	// cheap and modification can occur in same operation):
	{
		std::unique_lock wg{this->_mutex};

		// Only remove if list has nodes.
		if (!this->_head) {
			return false;
		}

		// Handle head and tail cases.
		if (this->_head == node) {
			POP_FRONT;
			node = nullptr;
			return true;
		} else if (this->_tail == node) {
			// `else if` to lock control flow into `size > 1` for tail case.
			POP_BACK;
			node = nullptr;
			return true;
		}
	}

	// General case:
	// Already handled head and tail, so safe to assume `size() > 2`.
	typename FineConcurrentList<T>::ListNodeT *n = const_cast<typename FineConcurrentList<T>::ListNodeT*>(node);

	unlink(n);
	delete n;
	n = nullptr;

	std::unique_lock g{this->_mutex};
	--this->_size;

	return true;
}

template <typename T>
bool FineConcurrentList<T>::removeAndPushFront(const typename FineConcurrentList<T>::ListNodeT *node) {
	if (!node) {
		return false;
	}

	auto *mut = const_cast<typename FineConcurrentList<T>::ListNodeT*>(node);
	// check if already at front under read lock, before releasing to rely on
	// hand-over-hand locks in middle of list:
	// xxx does this actually increase performance? these operations might be so
	// fast the lock overhead is greater
	{
		std::shared_lock<std::shared_mutex> rg(this->_mutex);
		if (mut == this->_head) {
			return true;
		}
	}

	if (!unlink(mut)) {
		return false;
	}

	std::unique_lock<std::shared_mutex> wg(this->_mutex);
	mut->next = this->_head;
	if (this->_head) {
		this->_head->prev = mut;
	}
	this->_head = mut;
	if (!this->_tail) {
		this->_tail = this->_head;
	}

	return true;
}

template <typename T>
bool FineConcurrentList<T>::unlink(const typename FineConcurrentList<T>::ListNodeT *node)
{
	// Validate node before locking:
	if (!node) {
		return false;
	}

	// Cast away the client's const, we're in our owned instance.
	auto *mut = const_cast<typename FineConcurrentList<T>::ListNodeT*>(node);

	// Handle head and tail:
	{
		std::unique_lock<std::shared_mutex> wg(this->_mutex);
		if (!this->_head) {	// empty list
			return false;
		}
		if (!this->_head->next) {
			this->_head = this->_tail = nullptr;
			mut->next = mut->prev = nullptr;
			return true;
		}
		if (mut == this->_head) {
			this->_head->next->prev = nullptr;
			this->_head = this->_head->next;
			mut->next = mut->prev = nullptr;
			return true;
		} else if (node == this->_tail) {
			// `else if` will trap us in a condition where `size() > 1`.
			this->_tail->prev->next = nullptr;
			this->_tail = this->_tail->prev;
			mut->next = mut->prev = nullptr;
			return true;
		}
	}

	// General case:
	std::unique_lock lk{mut->mtx, std::defer_lock};
	std::unique_lock nlk{mut->next->mtx, std::defer_lock};
	std::unique_lock plk{mut->prev->mtx, std::defer_lock};
	std::lock(lk, nlk, plk);

	mut->next->prev = mut->prev;
	mut->prev->next = mut->next;
	mut->next = mut->prev = nullptr;

	return true;
}

template <typename T>
const typename FineConcurrentList<T>::ListNodeT* FineConcurrentList<T>::search(const T& element) const
{
	std::shared_lock g{this->_mutex};
	// Guard if list is empty.
	if (!this->_head) {
		return nullptr;
	}
	// simple O(1) cases:
	if (this->_head->ele == element) {
		return this->_head;
	}
	if (!this->_head->next) {
		return nullptr;
	}
	if (this->_tail->ele == element) {
		return this->_tail;
	}
	if (this->_head->next->ele == element) {
		return this->_head->next;
	}
	if (this->_tail->prev && this->_tail->prev->ele == element) {
		return this->_tail->prev;
	}

	const typename FineConcurrentList<T>::ListNodeT *curr = this->_head->next;
	std::shared_lock lk{curr->mtx};
	// release global lock after node is protected
	g.unlock();

	while (curr) {
		if (curr->ele == element) {
			return curr;
		}

		const typename FineConcurrentList<T>::ListNodeT *next = curr->next;
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
bool FineConcurrentList<T>::contains(const typename FineConcurrentList<T>::ListNodeT *ptr) const
{
	if (!ptr) {
		return false;
	}

	// Guard if list is empty.
	std::shared_lock g{this->_mutex};
	if (!this->_head) {
		return false;
	}
	if (!this->_head->next) {
		return true;
	}
	const typename FineConcurrentList<T>::ListNodeT *curr = this->_head->next;
	g.unlock();

	// Iterate looking for matching address of parameter pointer against
	// a FineConcurrentList pointer.
	std::shared_lock lk{curr->mtx};
	while (curr) {
		if (curr == ptr) {
			return true;
		}

		const typename FineConcurrentList<T>::ListNodeT *next = curr->next;
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
	std::unique_lock<std::shared_mutex> g{this->_mutex};

	typename FineConcurrentList<T>::ListNodeT* curr = this->_head;
	while (curr) {
		typename FineConcurrentList<T>::ListNodeT *next = curr->next;
		delete curr;
		curr = next;
	}

	this->_head = this->_tail = nullptr;
	this->_size = 0;
}
