/**
 * @file test.hpp
 * @namespace test
 *
 * @author Tyler Baxter
 * @version 1.0
 * @since 2024-08-30
 *
 * Namespace for test cases.
 */

#pragma once

#include <string>

namespace test {

std::string randomString();
int randomInt();

/**
* Unit tests for DoublyLinkedList.
*
* @credit OpenAI's ChatGPT
* Prompt: "Write me test cases for this class, with no frameworks, in cpp."
*/
void testDoublyLinkedList();

/**
 * Main driver function for all test cases.
 */
void test();

}
