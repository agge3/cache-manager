template <typename T>
class DLLIterator {
public:
    // Define the iterator traits
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using pointer = T*;
    using reference = T&;

    // Constructor
    DLLIterator(Node<T>* ptr) : _ptr(ptr) {}

    // Dereference operator
    reference operator*() { return _ptr->data; }
    pointer operator->() { return &_ptr->data; }

    // Increment operators
    DLLIterator& operator++() {
        _ptr = _ptr->next;
        return *this;
    }
    DLLIterator operator++(int) {
        DLLIterator temp = *this;
        ++(*this);
        return temp;
    }

    // Decrement operators
    DLLIterator& operator--() {
        _ptr = _ptr->prev;
        return *this;
    }
    DLLIterator operator--(int) {
        DLLIterator temp = *this;
        --(*this);
        return temp;
    }

    // Comparison operators
    bool operator==(const DLLIterator& other) const { return _ptr == other._ptr; }
    bool operator!=(const DLLIterator& other) const { return _ptr != other._ptr; }

private:
    Node<T>* _ptr;  // Pointer to the current node
};
