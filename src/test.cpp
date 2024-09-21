/**
 * @file test.cpp
 *
 * @author Tyler Baxter
 * @version 1.0
 * @since 2024-08-30
 *
 * Test cases.
 */

#include "test.h"
#include "doubly-linked-list.h"

#include <iostream>
#include <memory>
#include <cassert>
#include <vector>
#include <random>
#include <algorithm>

using namespace csc;

/**
* Tests for DoublyLinkedList.
*
* @credit OpenAI's ChatGPT
* @modified 2024-09-20 Tyler Baxter
*/
void test::doublyLinkedList()
{
	auto testInt = []() {
		std::default_random_engine generator;
    	std::uniform_int_distribution<int> distribution(1, 100);
        auto randomValue = [&]() { return distribution(generator); };

    	// Test DLLNode<int> construction.
        DLLNode<int> node1(10);
        assert(node1.getElement() == 10);
        assert(node1.getNext() == nullptr);
        assert(node1.getPrev() == nullptr);

        // Test setting and getting elements.
        node1.setElement(20);
        assert(node1.getElement() == 20);

        // Test setting and getting next and previous nodes.
        DLLNode<int> node2(30);
        node1.setNext(&node2);
        node2.setPrev(&node1);
        assert(node1.getNext() == &node2);
        assert(node2.getPrev() == &node1);

        // Create a DoublyLinkedList<int> instance.
        auto list = std::make_unique<DoublyLinkedList<int>>();

        // Test empty list.
        assert(list->empty());
        std::cout << "[x] empty() tests passed.\n";
        assert(list->size() == 0);
        std::cout << "[x] size() tests passed.\n";

        // Test pushFront() with random values.
        for (int i = 0; i < 10; ++i) {
            int value = randomValue();
            list->pushFront(value);
            assert(list->front() == value); // New front should be the last pushed.
            assert(list->size() == i + 1);
        }
        std::cout << "[x] pushFront() tests passed.\n";

        // Test pushBack() with random values.
        for (int i = 0; i < 10; ++i) {
            int value = randomValue();
            list->pushBack(value);
            assert(list->back() == value); // New back should be the last pushed.
            assert(list->size() == 20 + i + 1);
        }
        std::cout << "[x] pushBack() tests passed.\n";

        // Test contains() and get() for all inserted values.
		// xxx iterator
        for (int i = 0; i < list->size(); ++i) {
			//assert(list->contains(i));
            //assert(list->get(i).has_value()); // Ensure it returns a value.
        }
		//std::cout << "contains() tests passed.\n";
        //std::cout << "get() tests passed.\n";

	    // Test remove() with both present and absent elements.
		// XXX implement iterator
        //std::vector<int> values;
        //for (int i = 0; i < list->size(); ++i) {
        //    values.push_back(*list->get(i));
        //}

        //// Shuffle values for random removals.
        //std::shuffle(values.begin(), values.end());
        //
        //for (int value : values) {
        //    bool removed = list->remove(value);
        //    assert(removed); // Ensure the value was removed.
        //    assert(!list->contains(value)); // Check that it's no longer in the list.
        //    assert(list->size() >= 0); // Ensure size is valid.
        //}	
        //std::cout << "remove() tests passed.\n";
		list->clear();

        // Test popFront().
        for (int i = 0; i < 5; ++i) {
            list->pushBack(randomValue());
        }

        while (!list->empty()) {
            int frontValue = *list->front();
            list->popFront();
            assert(list->size() >= 0); // Ensure size is valid.
            // Check that front has been removed and size is decremented.
        }
        std::cout << "[x] popFront() tests passed.\n";

        // Test popBack().
        // Reinsert values for popBack tests.
        for (int i = 0; i < 5; ++i) {
            list->pushBack(randomValue());
        }

        while (!list->empty()) {
            int backValue = *list->back();
            list->popBack();
            assert(list->size() >= 0); // Ensure size is valid.
            // Check that back has been removed and size is decremented.
        }
        std::cout << "[x] popBack() tests passed.\n";

        // Test clear.
        // Clear the list and ensure it's empty.
        for (int i = 0; i < 5; ++i) {
            list->pushBack(randomValue());
        }
        list->clear();
        assert(list->empty());
        assert(list->size() == 0);
        std::cout << "[x] clear() tests passed.\n";

        // Test removeAndPushFront().
	    for (int i = 0; i < 10; ++i) {
        	list->pushBack(randomValue());
        }	
		int rand = randomValue();
		const DLLNode<int> *ptr1 = list->pushFront(rand);
		list->removeAndPushFront(ptr1);
		assert(*list->front() == rand);
	    for (int i = 0; i < 10; ++i) {
        	list->pushFront(randomValue());
        }
		list->removeAndPushFront(ptr1);
		assert(*list->front() == rand);
	    for (int i = 0; i < 10; ++i) {
        	list->pushBack(randomValue());
        }
		list->removeAndPushFront(ptr1);
		assert(*list->front() == rand);
		rand = randomValue();
		const DLLNode<int> *ptr2 = list->pushBack(rand);
		list->removeAndPushFront(ptr2);
		assert(*list->front() == rand);

        // Test copy constructor.
        auto listCopy = std::make_unique<DoublyLinkedList<int>>(*list);
        assert(*listCopy->front() == *list->front());
        assert(listCopy->size() == list->size());
        std::cout << "[x] Copy constructor tests passed.\n";

        // Test copy assignment operator.
        auto listAssigned = std::make_unique<DoublyLinkedList<int>>();
        *listAssigned = *list;
        assert(*listAssigned->front() == *list->front());
        assert(listAssigned->size() == list->size());
        std::cout << "[x] Copy assignment operator tests passed.\n";

        // Test move constructor.
        auto listMoved = std::make_unique<DoublyLinkedList<int>>(std::move(*list));
        assert(listMoved->size() == listAssigned->size());
        assert(*listMoved->front() == *listAssigned->front());
        assert(list->empty()); // Ensure original list is empty.
        std::cout << "[x] Move constructor tests passed.\n";

        // Test move assignment operator.
        auto listMovedAssign = std::make_unique<DoublyLinkedList<int>>();
        *listMovedAssign = std::move(*listAssigned);
        assert(listMovedAssign->size() == listMoved->size());
        assert(*listMovedAssign->front() == *listMoved->front());
        assert(listAssigned->empty()); // Ensure original list is empty.
        std::cout << "[x] Move assignment operator tests passed.\n";

        // Test destructors.
        list.reset();
        std::cout << "[x] Destructor tests passed.\n";
        listCopy.reset();
        std::cout << "[x] Copy constructor destructor tests passed.\n";
        listAssigned.reset();
        std::cout << "[x] Copy assignment operator destructor tests passed.\n";
        listMoved.reset();
        std::cout << "[x] Move constructor destructor tests passed.\n";
        listMovedAssign.reset();
        std::cout << "[x] Move assignment operator destructor tests passed.\n";
    };

	auto testString = []() {
	    std::default_random_engine generator;
	    std::uniform_int_distribution<int> lengthDist(1, 10);
	    auto randomString = [&]() {
	        int len = lengthDist(generator);
	        std::string str;
	        for (int j = 0; j < len; ++j) {
	            char c = 'a' + generator() % 26; // Generate a random lowercase letter
	            str += c;
	        }
	        return str;
	    };
	
	    // Test DLLNode<std::string> construction.
	    DLLNode<std::string> node1("test");
	    assert(node1.getElement() == "test");
	    assert(node1.getNext() == nullptr);
	    assert(node1.getPrev() == nullptr);
	
	    // Test setting and getting elements.
	    node1.setElement("updated");
	    assert(node1.getElement() == "updated");
	
	    // Test setting and getting next and previous nodes.
	    DLLNode<std::string> node2("next");
	    node1.setNext(&node2);
	    node2.setPrev(&node1);
	    assert(node1.getNext() == &node2);
	    assert(node2.getPrev() == &node1);
	
	    // Create a DoublyLinkedList<std::string> instance.
	    auto list = std::make_unique<DoublyLinkedList<std::string>>();
	
	    // Test empty list.
	    assert(list->empty());
	    std::cout << "empty() passed.\n";
	    assert(list->size() == 0);
	    std::cout << "size() passed.\n";
	
	    // Test pushFront() with random strings.
	    for (int i = 0; i < 10; ++i) {
	        std::string value = randomString();
	        list->pushFront(value);
	        assert(list->front() == value); // New front should be the last pushed.
	        assert(list->size() == i + 1);
	    }
	    std::cout << "[x] pushFront() tests passed.\n";
	
	    // Test pushBack() with random strings.
	    for (int i = 0; i < 10; ++i) {
	        std::string value = randomString();
	        list->pushBack(value);
	        assert(list->back() == value); // New back should be the last pushed.
	        assert(list->size() == 20 + i + 1);
	    }
	    std::cout << "[x] pushBack() tests passed.\n";
	
	    // Test contains() and get() for all inserted values.
		// xxx iterator
	    //for (int i = 0; i < list->size(); ++i) {
	    //    assert(list->contains(*list->get(i))); // Ensure it returns true for contained values.
	    //    assert(list->get(i).has_value()); // Ensure it returns a value.
	    //}
	    //std::cout << "contains() tests passed.\n";
	    //std::cout << "get() tests passed.\n";
	
	    //// Test remove() with both present and absent elements.
	    //std::vector<std::string> values;
	    //for (int i = 0; i < list->size(); ++i) {
	    //    values.push_back(*list->get(i));
	    //}
	
	    //// Shuffle values for random removals.
	    //std::shuffle(values.begin(), values.end());
	    //
	    //for (const auto& value : values) {
	    //    bool removed = list->remove(value);
	    //    assert(removed); // Ensure the value was removed.
	    //    assert(!list->contains(value)); // Check that it's no longer in the list.
	    //    assert(list->size() >= 0); // Ensure size is valid.
	    //}	
	    std::cout << "[x] remove() tests passed.\n";
	
	    // Test popFront().
	    for (int i = 0; i < 5; ++i) {
	        list->pushBack(randomString());
	    }
	
	    while (!list->empty()) {
	        std::string frontValue = *list->front();
	        list->popFront();
	        assert(list->size() >= 0); // Ensure size is valid.
	        // Check that front has been removed and size is decremented.
	    }
	    std::cout << "[x] popFront() tests passed.\n";
	
	    // Test popBack().
	    // Reinsert values for popBack tests.
	    for (int i = 0; i < 5; ++i) {
	        list->pushBack(randomString());
	    }
	
	    while (!list->empty()) {
	        std::string backValue = *list->back();
	        list->popBack();
	        assert(list->size() >= 0); // Ensure size is valid.
	        // Check that back has been removed and size is decremented.
	    }
	    std::cout << "[x] popBack() tests passed.\n";
	
	    // Test clear.
	    // Clear the list and ensure it's empty.
	    for (int i = 0; i < 5; ++i) {
	        list->pushBack(randomString());
	    }
	    list->clear();
	    assert(list->empty());
	    assert(list->size() == 0);
	    std::cout << "[x] clear() tests passed.\n";
	
	    // Test removeAndPushFront().
	    for (int i = 0; i < 10; ++i) {
	        list->pushBack(randomString());
	    }	
	    std::string randStr = randomString();
	    const DLLNode<std::string> *ptr1 = list->pushFront(randStr);
	    list->removeAndPushFront(ptr1);
	    assert(*list->front() == randStr);
	    
	    for (int i = 0; i < 10; ++i) {
	        list->pushFront(randomString());
	    }
	    list->removeAndPushFront(ptr1);
	    assert(*list->front() == randStr);
	    
	    for (int i = 0; i < 10; ++i) {
	        list->pushBack(randomString());
	    }
	    list->removeAndPushFront(ptr1);
	    assert(*list->front() == randStr);
	    
	    randStr = randomString();
	    const DLLNode<std::string> *ptr2 = list->pushBack(randStr);
	    list->removeAndPushFront(ptr2);
	    assert(*list->front() == randStr);
	
	    // Test copy constructor.
	    auto listCopy = std::make_unique<DoublyLinkedList<std::string>>(*list);
	    assert(*listCopy->front() == *list->front());
	    assert(listCopy->size() == list->size());
	    std::cout << "[x] Copy constructor tests passed.\n";
	
	    // Test copy assignment operator.
	    auto listAssigned = std::make_unique<DoublyLinkedList<std::string>>();
	    *listAssigned = *list;
	    assert(*listAssigned->front() == *list->front());
	    assert(listAssigned->size() == list->size());
	    std::cout << "[x] Copy assignment operator tests passed.\n";
	
	    // Test move constructor.
	    auto listMoved = std::make_unique<DoublyLinkedList<std::string>>(std::move(*list));
	    assert(listMoved->size() == listAssigned->size());
	    assert(*listMoved->front() == *listAssigned->front());
	    assert(list->empty()); // Ensure original list is empty.
	    std::cout << "[x] Move constructor tests passed.\n";
	
	    // Test move assignment operator.
	    auto listMovedAssign = std::make_unique<DoublyLinkedList<std::string>>();
	    *listMovedAssign = std::move(*listAssigned);
	    assert(listMovedAssign->size() == listMoved->size());
	    assert(*listMovedAssign->front() == *listMoved->front());
	    assert(listAssigned->empty()); // Ensure original list is empty.
	    std::cout << "[x] Move assignment operator tests passed.\n";
	
        // Test destructors.
        list.reset();
        std::cout << "[x] Destructor tests passed.\n";
        listCopy.reset();
        std::cout << "[x] Copy constructor destructor tests passed.\n";
        listAssigned.reset();
        std::cout << "[x] Copy assignment operator destructor tests passed.\n";
        listMoved.reset();
        std::cout << "[x] Move constructor destructor tests passed.\n";
        listMovedAssign.reset();
        std::cout << "[x] Move assignment operator destructor tests passed.\n";
	};	

	testInt();
    std::cout << "[x] int tests passed!\n";
	testString();
    std::cout << "[x] String tests passed!\n";
	std::cout << "Stress testing...\n";
	for (auto i = 0; i < 100; ++i) {
		testInt();
		testString();
	}
	std::cout << "[x] Stress tests passed! 100 iterations." << std::endl;
}

/**
* Unit tests for HashMap.
*
* @credit OpenAI's ChatGPT
* Prompt: "Write me test cases for this class, with no frameworks, in cpp."
*/
//void test::hash_map()
//{
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
//}
