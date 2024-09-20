/**
 * @file main.cpp
 *
 * @author Tyler Baxter, Kat Powell
 * @version 1.0
 * @since 2024-08-30
 *
 * Main entry point. Test cases.
 */

#include "test.h"
#include "singly-linked-list.h"
//#include "doubly-linked-list.h"

#include <iostream>
#include <memory>
#include <cassert>
#include <vector>

using namespace csc;

/**
* Unit tests for Node.
*
* @credit OpenAI's ChatGPT
* Prompt: "Write me test cases for this class, with no frameworks, in cpp."
*/
//void test::node()
//{
//    // Test Node creation
//    Node<int> node1(10);
//    assert(node1.get_element() == 10);
//    assert(node1.get_next() == nullptr);
//    assert(node1.get_prev() == nullptr);
//
//    // Test setting and getting elements
//    node1.set_element(20);
//    assert(node1.get_element() == 20);
//
//    // Test setting and getting next and previous nodes
//    Node<int> node2(30);
//    node1.set_next(&node2);
//    node2.set_prev(&node1);
//    assert(node1.get_next() == &node2);
//    assert(node2.get_prev() == &node1);
//}

/**
* Unit tests for List.
*
* @credit OpenAI's ChatGPT
* Prompt: "Write me test cases for this class, with no frameworks, in cpp."
*/
//void test::linked_list()
//{
//    // Create a LinkedList instance.
//    auto list = std::make_unique<LinkedList<int>>();
//
//    // Test empty list
//    assert(list->empty() == true);
//	std::cout << "empty() passed.\n";
//    assert(list->size() == 0);
//	std::cout << "size() passed.\n";
//
//    // Test push_front
//    list->push_front(1);
//    assert(list->front() == 1);
//    assert(list->back() == 1);
//    assert(list->size() == 1);
//	std::cout << "push_front() passed.\n";
//
//    // Test push_back
//    list->push_back(2);
//    assert(list->front() == 1);
//    assert(list->back() == 2);
//    assert(list->size() == 2);
//	std::cout << "push_back() passed.\n";
//
//	// Test get
//	assert(list->get(0) == 1);
//    assert(list->get(1) == 2);
//	std::cout << "get() passed.\n";
//
//    // Test insert
//    list->insert(3, 1); // Insert 3 at index 1
//	std::cout << "Element inserted into the list.\n";
//    assert(list->get(0) == 1);
//    assert(list->get(1) == 3);
//    assert(list->get(2) == 2);
//    assert(list->size() == 3);
//	std::cout << "insert() passed.\n";
//
//    // Test remove
//    assert(list->remove(3) == true);
//	std::cout << "Element removed from the list.\n";
//    assert(list->size() == 2);
//    assert(list->get(0) == 1);
//    assert(list->get(1) == 2);
//	std::cout << "remove() passed.\n";
//
//    // Test pop_front
//    list->pop_front();
//    assert(list->size() == 1);
//    assert(list->front() == 2);
//	std::cout << "pop_front() passed.\n";
//
//    // Test pop_back
//    list->pop_back();
//    assert(list->empty() == true);
//    assert(list->size() == 0);
//	std::cout << "pop_back() passed.\n";
//
//    // Test exception handling for empty list
//    try {
//        list->front(); // Should throw exception
//        assert(false); // Should not reach this line
//    } catch (const std::out_of_range& e) {
//		std::cout << "front() out of range exception passed. " << e.what() <<
//			"\n";
//    }
//
//    try {
//        list->back(); // Should throw exception
//        assert(false); // Should not reach this line
//    } catch (const std::out_of_range& e) {
//		std::cout << "back() out of range exception passed. " << e.what() <<
//			"\n";
//    }
//
//    // Test clear
//    list->push_front(4);
//    list->push_back(5);
//    list->clear();
//    assert(list->empty() == true);
//    assert(list->size() == 0);
//	std::cout << "clear() passed.\n";
//
//    // Test copy constructor
//    list->push_front(6);
//    list->push_back(7);
//    auto listCopy = std::make_unique<LinkedList<int>>(*list);
//    assert(listCopy->front() == 6);
//    assert(listCopy->back() == 7);
//    assert(listCopy->size() == 2);
//	std::cout << "Copy constructor passed.\n";
//
//    // Test copy assignment operator
//    auto listAssigned = std::make_unique<LinkedList<int>>();
//    *listAssigned = *list;
//    assert(listAssigned->front() == 6);
//    assert(listAssigned->back() == 7);
//    assert(listAssigned->size() == 2);
//	std::cout << "Copy assignment operator passed.\n";
//
//    // Test move constructor
//    auto listMoved = std::make_unique<LinkedList<int>>(std::move(*list));
//    assert(listMoved->front() == 6);
//    assert(listMoved->back() == 7);
//    assert(listMoved->size() == 2);
//    assert(list->empty() == true); // Ensure list is empty after move
//	std::cout << "Move constructor passed.\n";
//
//    // Test move assignment operator
//    auto listMovedAssign = std::make_unique<LinkedList<int>>();
//    *listMovedAssign = std::move(*listAssigned);
//    assert(listMovedAssign->front() == 6);
//    assert(listMovedAssign->back() == 7);
//    assert(listMovedAssign->size() == 2);
//    assert(listAssigned->empty() == true); // Ensure listAssigned is empty after move
//	std::cout << "Move assignment operator passed.\n";
//
//    // Test contains
//    assert(listMoved->contains(6) == true);
//    assert(listMoved->contains(8) == false);
//	std::cout << "contains() passed.\n";
//
//    const Node<int>* beginNode = listMoved->begin();
//    assert(beginNode != nullptr); // Check if beginNode is not null
//    assert(beginNode->get_element() == 6);
//	beginNode = nullptr;
//	std::cout << "begin() passed.\n";
//
//    const Node<int>* endNode = listMoved->end();
//    assert(endNode != nullptr); // Check if endNode is not null
//    assert(endNode->get_element() == 7);
//	endNode = nullptr;
//	std::cout << "end() passed.\n";
//
//	list.reset();
//	std::cout << "Destructor of list passed.\n";
//	listCopy.reset();
//	std::cout << "Destructor of copy constructor list passed.\n";
//	listAssigned.reset();
//	std::cout << "Destructor of copy assignment operator list passed.\n";
//	listMoved.reset();
//	std::cout << "Destructor of move constructor list passed.\n";
//    listMovedAssign.reset();
//	std::cout << "Destructor of move assignment operator list passed.\n";
//
//    std::cout << "All tests passed!" << std::endl;
//}

void test::singly_linked_list()
{
    // Testing Default Constructor.
    SinglyLinkedList<int> list;
    assert(list.size() == 0);
    assert(list.empty() == true);
    std::cout << "Default constructor test passed." << std::endl;

    // Testing insert and front.
    list.insert(10);
    assert(list.size() == 1);
    assert(*list.front() == 10);
    std::cout << "Insert and front test passed." << std::endl;

    // Testing pop_front.
    list.insert(20);
    list.insert(30);
    assert(*list.pop_front() == 30);
    assert(list.size() == 1);
    assert(*list.front() == 20);
    std::cout << "Pop front test passed." << std::endl;

    // Test contains.
    list.insert(60);
    list.insert(70);
    assert(list.contains(60) == true);
    assert(list.contains(80) == false);
    std::cout << "Contains test passed." << std::endl;

    // Test remove.
    list.insert(40);
    list.insert(50);
    assert(list.remove(40) == true);
    assert(list.size() == 1);
    assert(list.contains(40) == false);
    assert(list.contains(50) == true);
    std::cout << "Remove test passed." << std::endl;

    // Test copy constructor.
    SinglyLinkedList<int> list1;
    list1.insert(90);
    list1.insert(100);
    SinglyLinkedList<int> list2(list1); // Copy
    assert(list2.size() == 2);
    assert(list2.contains(90) == true);
    assert(list2.contains(100) == true);
    std::cout << "Copy constructor test passed." << std::endl;

    // Test move constructor.
    SinglyLinkedList<int> list3;
    list3.insert(110);
    list3.insert(120);
    SinglyLinkedList<int> list4(std::move(list3)); // Move
    assert(list4.size() == 2);
    assert(list3.size() == 0); // list1 should be empty after move
    std::cout << "Move constructor test passed." << std::endl;

    // Test assigment operator.
    SinglyLinkedList<int> list5;
    list5.insert(130);
    list5.insert(140);
    SinglyLinkedList<int> list6;
    list6 = list5; // Copy assignment
    assert(list6.size() == 2);
    assert(list6.contains(130) == true);
    assert(list6.contains(140) == true);
    std::cout << "Assignment operator test passed." << std::endl;

    // Test move assignment operator.
    SinglyLinkedList<int> list7;
    list7.insert(150);
    list7.insert(160);
    SinglyLinkedList<int> list8;
    list8 = std::move(list7); // Move assignment
    assert(list8.size() == 2);
    assert(list7.size() == 0); // list1 should be empty after move
    std::cout << "Move assignment operator test passed." << std::endl;

    //// Test clear.
    //list.clear();
    //assert(list.size() == 0);
    //assert(list.empty() == true);
    //std::cout << "Clear test passed." << std::endl;
}

/**
* Unit tests for HashMap.
*
* @credit OpenAI's ChatGPT
* Prompt: "Write me test cases for this class, with no frameworks, in cpp."
*/
void test::hash_map()
{
//    // Create a HashMap instance
//    HashMap map;
//
//    // Test empty map
//    assert(map.empty() == true);
//    assert(map.size() == 0);
//
//    // Test insertion
//    map.insert(1, 100);
//    assert(map.size() == 1);
//    assert(map.contains_key(1) == true);
//    assert(map.contains_value(100) == true);
//
//    // Test get value
//    assert(map.get(1) == 100);
//
//    // Test insertion of another element
//    map.insert(2, 200);
//    assert(map.size() == 2);
//    assert(map.contains_key(2) == true);
//    assert(map.contains_value(200) == true);
//
//    // Test replacing a value
//    map.replace(1, 150);
//    assert(map.get(1) == 150);
//
//    // Test replace with old_value match
//    map.replace(2, 200, 250);
//    assert(map.get(2) == 250);
//    assert(map.contains_value(200) == false);
//
//    // Test remove by key
//    assert(map.remove(1) == 150);
//    assert(map.contains_key(1) == false);
//    assert(map.size() == 1);
//
//    // Test remove by key and value
//    map.insert(3, 300);
//    map.insert(4, 300); // Adding duplicate value
//    assert(map.remove(4, 300) == true);
//    assert(map.contains_key(4) == false);
//    assert(map.contains_value(300) == true);
//    assert(map.size() == 2); // size should be 2 after removing one element
//
//    // Test clear
//    map.clear();
//    assert(map.empty() == true);
//    assert(map.size() == 0);
//
//    // Test remove from empty map
//    try {
//        map.remove(10);
//        std::cerr << "Expected exception not thrown\n";
//    } catch (...) {
//        // Expected behavior
//    }
//
//    try {
//        map.get(10);
//        std::cerr << "Expected exception not thrown\n";
//    } catch (...) {
//        // Expected behavior
//    }
//
//    try {
//        map.remove(10, 100);
//        std::cerr << "Expected exception not thrown\n";
//    } catch (...) {
//        // Expected behavior
//    }
//
//    // Test move operations (if implemented)
//    HashMap map2;
//    map2.insert(5, 500);
//    HashMap map3(std::move(map2));
//    assert(map3.get(5) == 500);
//    assert(map2.empty() == true);
//
//    HashMap map4;
//    map4.insert(6, 600);
//    map4 = std::move(map3);
//    assert(map4.get(5) == 500);
//    assert(map3.empty() == true);
//
//    std::cout << "All tests passed!" << std::endl;
}
