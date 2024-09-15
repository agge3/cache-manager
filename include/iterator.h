#pragma once

namespace csc {

template <typename T>
class Iterator {
public:
	virtual Iterator& operator++() = 0;
	virtual Iterator operator++(int) = 0;
	virtual T& operator*() const = 0;
	virtual void add(const T& element) = 0;
	virtual void delete() = 0;
	bool operator==(Iterator other) const { return this == &other; }
	bool operator!=(Iterator other) const { return this != &other; }
protected:
	/**
	* new and delete are protected so heap allocation is disallowed. Must be 
	* allocated on the stack, for RAII.
	*/
	explicit Iterator() : {}
	~Iterator();
};
}
