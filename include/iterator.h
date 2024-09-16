/*
 * @file iterator.h
 * @class Iterator
 *
 * @author Kat Powell
 * @version 1.0
 * @since 2024-09-12
 *
 * Iterator.
 */

#pragma once

#include <cstddef>

namespace csc {

template <typename T>
class Iterator {
public:
    /**
     * Constructor.
     *
     * @param Two pointers: one at the beginning of container, one at the end of
     *                      the container.
     */
    Iterator(T *beg, T *end) : _idx(0), _curr(beg), _first(beg),
                               _last(end) {} ;
    /**
     * Returns the element at the front of the list. Throws an exception if the
     * list is empty.
     */
    T* first();
    T* last();
    T* next();
    T* get() const;
    T* get_next();
    T* skip_to(int my_idx) const;
    bool has_next();
    bool is_done() const;
    bool remove();

    void operator++();
    T* operator++(int);
private:
    T *_first;
    T *_last;
    T *_curr;
    std::size_t _idx;
    std::size_t _size;
};
}

#include "iterator.tpp"
