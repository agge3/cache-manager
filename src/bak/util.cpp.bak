#include "util.h"

#include <random>

using namespace util;

std::vector<int> generate_random(int n, int min, int max)
{
    std::vector<int> numbers;
    
    // Create a random number generator with a random device seed
    std::random_device rd;
    std::mt19937 gen(rd()); // Mersenne Twister engine
    
    // Define the range for the random numbers
    std::uniform_int_distribution<> dis(min, max);

    // Generate 'n' random numbers
    for (int i = 0; i < n; ++i) {
        numbers.push_back(dis(gen));
    }

    return numbers;
}
