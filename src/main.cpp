/**
 * @file main.cpp
 *
 * @author Tyler Baxter
 * @version 1.0
 * @since 2024-08-30
 *
 * Main entry point. Test cases.
 */

#include "linked-list.h"
#include "bst.h"

#include <iostream>
#include <memory>
#include <cassert>
#include <vector>
#include <random>
#include <sstream>
#include <string>

// Anonymous namespace to trap the globals in this file's scope.
namespace {
	/**
	 * Function to generate 'n' random numbers within a specified range.
	 *
	 * CREDIT: OpenAI's ChatGPT
	 * PROMPT: in cpp give me a random number generator that generates n numbers 
	 */
	std::vector<int> generateRandomNumbers(int n, int min, int max) {
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

	/**
	 * @struct Dataset
     * A struct to carry constants for the randomly generated data: the amount 
	 * of data points, the minimum value, and the maximum value.
     */ 
	struct Dataset {
		int amt = 10;
		int min = 0;
		int max = 10;
	};
}

// xxx remove global, put into a namespace or class
void testSLL()
{
	std::unique_ptr<csc::LinkedList> list(new csc::LinkedList());
	std::unique_ptr<Dataset> dataset(new Dataset);
	std::unique_ptr<std::vector<int>> data = std::make_unique<std::vector<int>>(
	    generateRandomNumbers(dataset->amt, dataset->min, dataset->max)
	);

	// Unit tests:
	for (auto i = 0; i < 5; ++i) {
		list->insertAtBeginning(data->at(i));
		assert(list->get(0) == data->at(i));
		std::cout << "insertAtBeginning() passed. List head: " << list->get(0) << 
			"\n";
	}

	for (auto i = 5; i < 10; ++i) {
		list->insertAtEnd(data->at(i));
		assert(list->get(i) == data->at(i));
		std::cout << "insertAtEnd() passed. List index " << i << ": " <<
			list->get(i) << "\n";
	}

	try {
		list->get(-1);
	} catch (const std::out_of_range& e) {
		std::cout << "Out of range exception passed. " << e.what() << "\n";
	}

	try {
		list->get(11);
	} catch (const std::out_of_range& e) {
		std::cout << "Out of range exception passed. " << e.what() << "\n";
	}

	list->deleteNode(data->at(8));
	assert(list->get(8) != data->at(8));
	std::cout << "deleteNode() passed. Deleted: " << data->at(8) << "\n";

	// Print final list:
	// Push a newline to separate final list output for unit tests.
	std::cout << "\n";
	std::cout << "List: ";
	list->printList();

	// Cleanup:
	list.reset();
	dataset.reset();
	data.reset();

	// Stress tests:
	// Push a newline to separate stress testing from final list output.
	std::cout << "\n";
	std::cout << "Stress testing...\n";
	auto stress_test = []() {
		std::unique_ptr<csc::LinkedList> list(new csc::LinkedList());
		std::unique_ptr<Dataset> dataset(new Dataset);
		std::unique_ptr<std::vector<int>> data = 
			std::make_unique<std::vector<int>>(
			generateRandomNumbers(dataset->amt, dataset->min, dataset->max)
		);
		for (auto i = 0; i < 5; ++i) {
			list->insertAtBeginning(data->at(i));
			assert(list->get(0) == data->at(i));
		}
		for (auto i = 5; i < 10; ++i) {
			list->insertAtEnd(data->at(i));
			assert(list->get(i) == data->at(i));
		}
		try {
			list->get(-1);
		} catch (const std::out_of_range& e) {
			// Do nothing; catching the exception is what we want.
		}
		try {
			list->get(11);
		} catch (const std::out_of_range& e) {
			// Do nothing; catching the exception is what we want.
		}
		list->deleteNode(data->at(8));
		assert(list->get(8) != data->at(8));
		list.reset();
		dataset.reset();
		data.reset();
	};
	for (int i = 0; i < 100; ++i) {
		stress_test;
	}
	std::cout << "Stress testing passed! 100 iterations passed." << std::endl;
}

// xxx remove global, put into a namespace or class
void testBST()
{
    auto tree = std::make_unique<BST>();

    // helper lambda to display binary search tree. capture tree's pointer
    auto display = [&tree]() {
        std::cout << "BINARY SEARCH TREE:\n";
        std::cout << "Inorder:\n";
        tree->display();
        std::cout << "\nPostorder:\n";
        tree->rdisplay();
        // push a newline
        std::cout << "\n";
    };

    std::cout << "Enter data to insert into the binary search tree " <<
        "(press ENTER to stop entering data):\n";

    std::string line;
    int value;

    std::getline(std::cin, line);
    std::istringstream stream(line);
    while (stream >> value) {
        tree->insert(new Node(value));
    }
    stream.clear();
    // push a newline
    std::cout << "\n";

    display();

    std::cout << "Enter the data to search for (integer):\n";
    std::getline(std::cin, line);
    stream.str(line);
    // push a newline
    std::cout << "\n";

    if (stream >> value && tree->search(value))
        std::cout << "Data found!\n";
    else 
        std::cout << "Data not found!\n";
    stream.clear();
    // push a newline
    std::cout << "\n";

    std::cout << "Enter data to remove from the binary search tree, " <<
        "or nothing to not remove any data " <<
        "(press ENTER to stop removing data):\n";

    std::getline(std::cin, line);
    stream.str(line);
    while (stream >> value) {
        tree->remove(value);
    }
    stream.clear();
    // push a newline
    std::cout << "\n";

    display();
}

int main()
{
	testSLL();
	testBST();
}
