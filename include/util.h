/**
 * @file util.h
 * @namespace util
 *
 * @author Tyler Baxter
 * @version 1.0
 * @since 2024-08-30
 *
 * Namespace for utility functions.
 */

#pragma once

#include <vector>

namespace util {

/**
 * Function to generate 'n' random numbers within a specified range.
 *
 * CREDIT: OpenAI's ChatGPT
 * PROMPT: in cpp give me a random number generator that generates n numbers 
 */
std::vector<int> generate_random(int n, int min, int max);

}
