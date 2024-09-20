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
   // Test 1: Default constructor and empty() method
    SinglyLinkedList<int> list;
    std::cout << "Test 1: Default constructor and empty()\n";
    std::cout << (list.empty() ? "Pass" : "Fail") << " - List is empty.\n";

    // Test 2: insert() and size() method
    std::cout << "\nTest 2: insert() and size()\n";
    SLLNode<int> *node1 = new SLLNode<int>(10);
    list.insert(20, node1); // Inserting 20 after node1
    std::cout << (list.size() == 1 ? "Pass" : "Fail") << " - Size is 1 after first insert.\n";

    // Test 3: contains() method
    std::cout << "\nTest 3: contains()\n";
    std::cout << (list.contains(20) ? "Pass" : "Fail") << " - List contains 20.\n";
    std::cout << (!list.contains(30) ? "Pass" : "Fail") << " - List does not contain 30.\n";

    // Test 4: remove() method
    std::cout << "\nTest 4: remove()\n";
    std::cout << (list.remove(20) ? "Pass" : "Fail") << " - Removed 20.\n";

	std::cout << "\nTest 5: push_front()\n";
	list.push_front(37);
	std::cout << (list.contains(37) ? "Pass" : "Fail") << " - Pushed front 37.\n";

	std::cout << "\nTest 6: push_back()\n";
	list.push_back(73);
	std::cout << (list.contains(73) ? "Pass" : "Fail") << " - Pushed back 73.\n";

    //std::cout << (!list.contains(20) ? "Pass" : "Fail") << " - List no longer contains 20.\n";
    //std::cout << (list.size() == 1 ? "Pass" : "Fail") << " - Size is 1 after remove.\n";

    //// Test 5: Copy constructor
    //std::cout << "\nTest 5: Copy constructor\n";
    //SinglyLinkedList<int> copiedList(list);
    //std::cout << (copiedList.size() == list.size() ? "Pass" : "Fail") << " - Copied list has same size.\n";
    //std::cout << (copiedList.front() == list.front() ? "Pass" : "Fail") << " - Copied list has same front element.\n";

    //// Test 6: Move constructor
    //std::cout << "\nTest 6: Move constructor\n";
    //SinglyLinkedList<int> movedList(std::move(list));
    //std::cout << (list.empty() ? "Pass" : "Fail") << " - Original list is empty after move.\n";
    //std::cout << (!movedList.empty() ? "Pass" : "Fail") << " - Moved list is not empty.\n";

    //// Test 7: Assignment operator
    //std::cout << "\nTest 7: Assignment operator\n";
    //SinglyLinkedList<int> assignedList;
    //assignedList = movedList;
    //std::cout << (assignedList.size() == movedList.size() ? "Pass" : "Fail") << " - Assigned list has same size as moved list.\n";
    //std::cout << (assignedList.front() == movedList.front() ? "Pass" : "Fail") << " - Assigned list has same front element.\n";

    //// Test 8: Move assignment operator
    //std::cout << "\nTest 9: Move assignment operator\n";
    //SinglyLinkedList<int> moveAssignedList;
    //moveAssignedList = std::move(movedList);
    //std::cout << (movedList.empty() ? "Pass" : "Fail") << " - Moved list is empty after move assignment.\n";
    //std::cout << (!moveAssignedList.empty() ? "Pass" : "Fail") << " - Move-assigned list is not empty.\n";

    // Clean up dynamic memory
    delete node1; 
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
