/**
 * @file hash-map-impl.hpp
 * @class HashMap
 *
 * @author Tyler Baxter, Kat Powell
 * @version 1.0
 * @since 2024-08-30
 *
 * HashMap and HashMap helpers.
 */

#include <stdexcept>
#include <cassert>

using namespace csc;

// Hash functions, to exist in this scope only.
namespace {
	std::size_t djb2(unsigned char *str)
    {
		std::size_t hash = 5381;
        int c;

        while (c = *str++)
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

        return hash;
    }	
	std::size_t djb2(const std::string& str)
	{
		std::size_t hash = 5381;
		for (char c : str) {
			hash = ((hash << 5) + hash) + static_cast<unsigned char>(c);
		}
		return hash;
	}
	template <typename K>
	std::size_t djb2(const K& key)
	{
		std::size_t hash = 5381;
		const unsigned char *ptr = 
			reinterpret_cast<const unsigned char *>(&key);
		for (std::size_t i = 0; i < sizeof(K); ++i) {
			hash = ((hash << 5) + hash) + ptr[i];
		}
		return hash;
	}
	std::size_t sdbm(unsigned char *str)
    {
		std::size_t hash = 0;
        int c;

        while (c = *str++)
            hash = c + (hash << 6) + (hash << 16) - hash;

        return hash;
    }
}

template <typename K>
std::size_t Hash<K>::operator()(const K& key) const 
{
	return djb2(key);
}

std::size_t Hash<unsigned char *>::operator()(unsigned char *str) const
{
	return djb2(str);
}

std::size_t Hash<std::string>::operator()(const std::string& str) const
{
	return djb2(str);
}

template <typename K, typename V>
constexpr bool csc::operator==(const HashNode<K, V>& rhs,
							   const HashNode<K, V>& lhs)
{
	return rhs.getKey() == lhs.getKey();
}

template <typename K, typename V>
constexpr bool csc::operator!=(const HashNode<K, V>& rhs,
							   const HashNode<K, V>& lhs)
{
	return !(rhs == lhs);
}

template <typename K, typename V, typename F>
std::optional<V> MapIterator<K, V, F>::operator*()
{
	if (_type != MapIteratorType::EmptyBucket &&
		_listIt != SLLIterator<HashNode<K, V>>(nullptr)) {
		auto node = *_listIt;
    	return std::optional<V>(node.getItem());
	}
	return std::nullopt;
}

template <typename K, typename V, typename F>
typename MapIterator<K, V, F>::pointer MapIterator<K, V, F>::operator->() 
{ 
	if (_type != MapIteratorType::EmptyBucket) {
		return &(_listIt->getItem()); 
	}
	return nullptr;
}

template <typename K, typename V, typename F>
MapIterator<K, V, F>& MapIterator<K, V, F>::operator++()
{
	if (_index > _buckets - 1) {
		throw std::runtime_error("Attempt to increment iterator past end.");
	}
	if (_type == MapIteratorType::EmptyBucket) {
		advance();
	} else {
		if (_listIt != _table[_index]->end()) {
			++_listIt;
		}
		if (_listIt == _table[_index]->end()) {
			advance();
		}
	}
	return *this;
}

template <typename K, typename V, typename F>
void MapIterator<K, V, F>::advance()
{
	++_index;
	setType();
	if (_type == MapIteratorType::FullBucket) {
		_listIt = _table[_index]->begin();
	} else {
		_listIt = SLLIterator<HashNode<K, V>>(nullptr);
	}
}

template <typename K, typename V, typename F>
void MapIterator<K, V, F>::setType()
{
	if (_table[_index] == nullptr) {
		_type = MapIteratorType::EmptyBucket;
	}
	else {
		_type = MapIteratorType::FullBucket;
	}
}

template <typename K, typename V, typename F>
MapIteratorType MapIterator<K, V, F>::getType() const
{
	return _type;
}

template <typename K, typename V, typename F>
std::size_t MapIterator<K, V, F>::getIndex() const
{
	return _index;
}

template <typename K, typename V, typename F>
MapIterator<K, V, F> MapIterator<K, V, F>::operator++(int)
{
	MapIterator tmp = *this;
	++(*this);
	return tmp;
}

template <typename K, typename V, typename F>
bool MapIterator<K, V, F>::operator==(const MapIterator& other) const 
{
	// Just because we're at the last index doesn't mean there's not still
	// chained list entries we're iterating.
    return _index == other._index && _listIt == other._listIt;
}

template <typename K, typename V, typename F>
bool MapIterator<K, V, F>::operator!=(const MapIterator& other) const 
{
	return !(*this == other);
}

template <typename K, typename V, typename F>
HashMap<K, V, F>::HashMap() : 
	_buckets(TABLE_BUCKETS),
	_table(new ListPtr[TABLE_BUCKETS]),
	_size(0),
	_hash()
{
}

template <typename K, typename V, typename F>
HashMap<K, V, F>::HashMap(std::size_t buckets) : 
	_buckets(buckets), 
	_table(new ListPtr[buckets]),
	_size(0),
	_hash()
{
	// do nothing
}

// Copy constructor
template <typename K, typename V, typename F>
HashMap<K, V, F>::HashMap(const HashMap<K, V, F>& src) 
    : _buckets(src._buckets), _size(src._size)
{
    // Copy elements
	// xxx
}

// Move constructor
template <typename K, typename V, typename F>
HashMap<K, V, F>::HashMap(HashMap<K, V, F>&& src) noexcept : 
	_buckets(src._buckets),
	_table(std::move(src._table)),
	_size(src._size)
{
    // xxx
}

// Copy assignment operator
template <typename K, typename V, typename F>
HashMap<K, V, F>& HashMap<K, V, F>::operator=(const HashMap<K, V, F>& rhs)
{
    if (this != &rhs) {
        clear();
        _buckets = rhs._buckets;
        _size = rhs._size;
        // Copy elements
		// xxx
    }
    return *this;
}

// Move assignment operator
template <typename K, typename V, typename F>
HashMap<K, V, F>& HashMap<K, V, F>::operator=(HashMap<K, V, F>&& rhs) noexcept
{
    if (this != &rhs) {
        clear();
		_buckets = rhs._buckets;
        _table = std::move(rhs._table);
    }
    return *this;
}



template <typename K, typename V, typename F>
void HashMap<K, V, F>::add(const K& key, const V& value)
{
	std::size_t idx = _hash(key) % (_buckets - 1);
	if (_table[idx] == nullptr) {
		_table[idx] = std::make_unique<SinglyLinkedList<HashNode<K, V>>>();
	}
	_table[idx]->pushFront(HashNode<K, V>(key, value));
	assert(_table[idx]->contains(HashNode<K, V>(key, value)));
	assert(_table[idx]->contains(HashNode<K, V>(key)));
	++_size;
}

template <typename K, typename V, typename F>
bool HashMap<K, V, F>::remove(const K& key)
{
	if (isEmpty()) {
		return false;
	}
	auto ptr = _table[_hash(key) % (_buckets - 1)].get();
	if (ptr == nullptr) {
		return false;
	}
	ptr->remove(HashNode<K, V>(key));
	--_size;
	return true;
}

template <typename K, typename V, typename F>
std::optional<V> HashMap<K, V, F>::getItem(const K& key) const
{
	if (isEmpty()) {
		return std::nullopt;
	}

	auto ptr = _table[_hash(key) % (_buckets - 1)].get();
	if (ptr == nullptr) {
		return std::nullopt;
	}

	auto node = ptr->find(HashNode<K, V>{key});
	return node.has_value() ? std::optional<V>(node->getItem()) : std::nullopt;
}

template <typename K, typename V, typename F>
bool HashMap<K, V, F>::contains(const K& key) const
{
	if (isEmpty()) {
		return false;
	}
	assert(_table != nullptr);
	auto ptr = _table[_hash(key) % (_buckets - 1)].get();
	if (ptr == nullptr) {
		return false;
	}
	return ptr->contains(HashNode<K, V>(key));
}

template <typename K, typename V, typename F>
bool HashMap<K, V, F>::replace(const K& key, const V& value)
{
	if (isEmpty()) {
		return false;
	}

	auto ptr = _table[_hash(key) % (_buckets - 1)].get();
	if (!ptr) {
		return false;
	}

	HashNode<K, V> *node = ptr->find(HashNode<K, V>{key});
	if (node == nullptr) {
		return false;
	}

	node->setValue(value);
	node == nullptr;
	return true;
}

template <typename K, typename V, typename F>
bool HashMap<K, V, F>::isEmpty() const
{
	// XXX Maybe an additional check?
	return _size == 0;
}

template <typename K, typename V, typename F>
std::size_t HashMap<K, V, F>::getNumberOfItems() const
{
	return _size;
}

template <typename K, typename V, typename F>
std::size_t HashMap<K, V, F>::capacity() const
{
	return _buckets;
}

template <typename K, typename V, typename F>
MapIterator<K, V, F> HashMap<K, V, F>::begin() const
{
	// Start at table index zero.
	return MapIterator<K, V, F>(_table, _buckets, 0);
}

template <typename K, typename V, typename F>
MapIterator<K, V, F> HashMap<K, V, F>::end() const
{
	// End at table index is table size.
	return MapIterator<K, V, F>(_table, _buckets, _buckets - 1);
}

template <typename K, typename V, typename F>
std::ostream& csc::operator<<(std::ostream& out, const HashMap<K, V, F>& map)
{
    bool empty = false;
	bool full = false;
	bool first = true;

    for (auto it = map.begin(); it != map.end(); ++it) {
        auto type = it.getType();
        std::size_t index = it.getIndex();

        if (type == MapIteratorType::EmptyBucket) {
			full = false;
			if (!empty) {
				if (first) {
        			out << "Empty: " << index;
					first = false;
					empty = true;
				} else { 
					out << "\n\nEmpty: " << index;
		 			empty = true;
				}
			} else {
		 		out << ", " << index;
			}
        } else if (type == MapIteratorType::FullBucket) {
			empty = false;
			auto valueOpt = *it;  // Get the optional value
        	if (valueOpt.has_value()) {  // Check if it has a value
            	if (!full) {
                	if (first) {
                    	out << "Index: " << index << ": " << valueOpt.value(); // Print the value
                    	first = false;
                	} else {
						out << "\n\nIndex: " << index << ": " << valueOpt.value(); // Print the value
                		full = true;
					}
            	} else {
                	out << ", " << valueOpt.value(); // Print the value
            	}
        	}
    	}
	}
	out << "\n";
    return out;
}

template <typename K, typename V, typename F>
void HashMap<K, V, F>::clear()
{
	// When table is deleted, its smart ListPtrs will lose scope and call their
	// destructors.
	delete[] _table;
	_table = new ListPtr[_buckets];
	_size = 0;
}
