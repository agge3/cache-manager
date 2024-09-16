using namespace csc;

template <typename T>
T* Iterator<T>::first()
{
    if (_curr == nullptr) {

    }
    return _first;
}

template <typename T>
T* Iterator<T>::last()
{
    return _last;
}

template <typename T>
T* Iterator<T>::next()
{
    return ++_curr;
}

template <typename T>
T* Iterator<T>::get() const
{
    return _curr;
}

template <typename T>
T* Iterator<T>::get_next()
{
    T* temp = _curr;
    T* next_curr = ++_curr;
    _curr = temp;
    return next_curr;
}

template <typename T>
T* Iterator<T>::skip_to(int my_idx) const
{
   if (my_idx == 0) {
       _curr = _first;
   } else if (my_idx
   return _curr; // STUB
}

// xxx has_next or is_done?
template <typename T>
bool Iterator<T>::has_next()
{
    return get_next() != _last;
}

template <typename T>
bool Iterator<T>::is_done() const
{
    return _curr == _last;
}

template <typename T>
bool Iterator<T>::remove()
{
    return false; // STUB
}

template <typename T>
void Iterator<T>::operator++()
{
    _curr = _curr.next();
    _idx++;
}

template <typename T>
T* Iterator<T>::operator++(int a)
{
    _curr = _curr.next();
    _idx++;
    return _curr;
}
