/**
 * @file test.h
 * @namespace test
 *
 * @author Tyler Baxter
 * @version 1.0
 * @since 2024-08-30
 *
 * Namespace for test cases.
 */

#pragma once

#include <gtest/gtest.h>	// include gtest when test.h is included

namespace test {

/**
 * Main driver function for all test cases.
 */
void test();

/**
* Unit tests for DoublyLinkedList.
*
* @credit OpenAI's ChatGPT
* Prompt: "Write me test cases for this class, with no frameworks, in cpp."
*/
void doublyLinkedList();

}
