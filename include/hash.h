#pragma once

#include <cstddef>
#include <string>

/**
* C-String implementation of the Hash function.
*/
template <>
struct Hash<unsigned char *> {
	std::size_t operator()(unsigned char *str) const;
};

/**
* C++ string implementation of the Hash function.
*/
template <>
struct Hash<std::string> {
	std::size_t operator()(const std::string& str) const;
};

/**
* Generic implementation of the Hash function.
*/
template <typename K>
struct Hash<K> {
	std::size_t operator()(const T& key) const;
};
