#include "hash.h"

// Hash functions. Hash functions exist in this scope only.
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
	template <typename T>
	std::size_t djb2(const T& key)
	{
		std::size_t hash = 5381;
		const unsigned char *ptr = 
			reinterpret_cast<const unsigned char *>(&key);
		for (std::size_t i = 0; i < sizeof(T); ++i) {
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

template <>
std::size_t Hash<unsigned char *>::operator()(unsigned char *str) const
{
	return djb2(str);
}

template <>
Hash<std::string>::std::size_t operator()(const std::string& str) const
{
	return djb2(str);
}

template <typename K>
std::size_t Hash<K>::operator()(const T& key) const 
{
	return djb2(key);
}
