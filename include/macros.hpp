/**
* @file macros.hpp
* @author agge3
* @date 2025-16-10
*
* Global utility macros.
*/

#pragma once

#define DEBUG 1

#ifdef DEBUG
#define DPRINT(fmt, ...) \
	std::cout	\
		<< std::format("DEBUG: {}: {}: {}: ", __FILE__, __func__, __LINE__)	\
		<< std::format(fmt __VA_OPT__(, ) __VA_ARGS__) << "\n";
#else 
#define DPRINT(fmt, ...) ((void)0)
#endif
