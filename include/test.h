/**
 * @file test.h
 * @namespace test
 *
 * @author Tyler Baxter
 * @version 1.0
 * @since 2024-08-30
 *
 * Namespace for running tests.
 */

#pragma once

namespace csc {
	template <typename T>
	class Node;
	template <typename T>
	class LinkedList;
}

namespace test {

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

/**
* Unit tests for Node.
*
* @credit OpenAI's ChatGPT
* Prompt: "Write me test cases for this class, with no frameworks, in cpp."
*/
void node();

/**
* Unit tests for SinglyLinkedList.
*
* @credit OpenAI's ChatGPT
* Prompt: "Write me test cases for this class, with no frameworks, in cpp."
*/
void singly_linked_list();

/**
* Unit tests for HashMap.
*
* @credit OpenAI's ChatGPT
* Prompt: "Write me test cases for this class, with no frameworks, in cpp."
*/
void hash_map();

}
