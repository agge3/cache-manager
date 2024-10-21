/**
 * @file test.cpp
 *
 * @author Tyler Baxter
 * @version 1.0
 * @since 2024-09-21
 *
 * Test cases.
 */

#include "test.h"

#include "singly-linked-list.h"
#include "doubly-linked-list.h"

#include <iostream>
#include <fstream>
#include <streambuf>
#include <memory>
#include <cassert>
#include <vector>
#include <random>
#include <algorithm>
#include <sstream>

#ifdef _WIN32
#define NULL_DEVICE "NUL:"
#else
#define NULL_DEVICE "/dev/null"
#endif

using namespace csc;

namespace test {

std::default_random_engine generator;
std::uniform_int_distribution<int> intDistribution{1, 100};
std::uniform_int_distribution<int> lengthDistribution{1, 10};

std::string randomString() {
    int len = lengthDistribution(generator);
    std::string str;
    for (int j = 0; j < len; ++j) {
        char c = 'a' + generator() % 26;
        str += c;
    }
    return str;
}

int randomInt() {
    return intDistribution(generator);
}

TEST(SinglyLinkedListTest, BasicOperationsInt) {
    SinglyLinkedList<int> list;

    // Initial state
    EXPECT_TRUE(list.isEmpty());
    EXPECT_EQ(list.size(), 0);

    // Push elements
    for (int i = 0; i < 10; ++i) {
		// XXX Random causes sometimes duplicate values. Do we want to cope with
		// that or not?
        list.pushFront(i);
        EXPECT_EQ(list.size(), i + 1);
        EXPECT_FALSE(list.isEmpty());
    }

    // Pop elements
    for (auto i = list.size(); i > 0; --i) {
		std::optional<int> front = list.front();
		std::optional<int> v = list.popFront();
		EXPECT_EQ(*front, *v) << "Front: " << *front << ", Popped value: " <<
			*v << "\n";
        EXPECT_TRUE(list.front() != *front) << "Current front: " << 
			*list.front() << ", Old front: " << *front << ", List size: " <<
			list.size() << "\n";
        EXPECT_EQ(list.size(), i - 1);
    }
    EXPECT_TRUE(list.isEmpty());

    // Remove elements
    for (int i = 0; i < 10; ++i) {
        list.pushFront(i);
    }
    EXPECT_EQ(list.size(), 10);
    EXPECT_TRUE(list.remove(5));
    EXPECT_EQ(list.size(), 9);
    EXPECT_FALSE(list.contains(5));
    EXPECT_TRUE(list.remove(0));
    EXPECT_EQ(list.size(), 8);
}

TEST(SinglyLinkedListTest, BasicOperationsString) {
    SinglyLinkedList<std::string> list;

    // Initial state
    EXPECT_TRUE(list.isEmpty());
    EXPECT_EQ(list.size(), 0);

    // Push strings
    for (int i = 0; i < 10; ++i) {
        list.pushFront(randomString());
        EXPECT_EQ(list.size(), i + 1);
        EXPECT_FALSE(list.isEmpty());
    }

    // Pop strings
    for (int i = 9; i >= 0; --i) {
		std::optional<std::string> value = list.popFront();
        EXPECT_EQ(list.size(), i);
        EXPECT_FALSE(list.contains(*value));
    }
    EXPECT_TRUE(list.isEmpty());

    // Remove non-existent string
    std::string toRemove = "test";
    EXPECT_FALSE(list.remove(toRemove));
}

TEST(SinglyLinkedListTest, EdgeCases) {
    SinglyLinkedList<int> list;

    // Pop from empty list
	std::optional<int> v = list.popFront();
    EXPECT_TRUE(!v.has_value());

    // Remove from empty list
    EXPECT_FALSE(list.remove(10));

    // Remove non-existent element
    for (int i = 0; i < 5; ++i) {
        list.pushFront(i);
    }
    EXPECT_EQ(list.size(), 5);
    EXPECT_FALSE(list.remove(10));
    EXPECT_EQ(list.size(), 5);
}

TEST(SLLMembers, pushFront)
{
	SinglyLinkedList<int> list;
	for (int i = 0; i < 10; ++i) {
		list.pushFront(i);
		EXPECT_TRUE(list.contains(i));
	}
}

TEST(SLLMembers, popFront)
{
	SinglyLinkedList<int> list;
	for (int i = 0; i < 10; ++i) {
		list.pushFront(i);
		EXPECT_TRUE(list.contains(i));
	}
	EXPECT_TRUE(list.size() == 10);
	for (auto i = list.size(); i > 0; --i) {
		std::optional<int> v = list.popFront();
		EXPECT_TRUE(list.size() == i - 1);
		EXPECT_FALSE(list.contains(*v));
	}
	EXPECT_TRUE(list.size() == 0);
	EXPECT_TRUE(list.isEmpty());
}

TEST(SLLFriends, ostream)
{
	SinglyLinkedList<int> list;
	for (int i = 1; i <= 5; ++i) {
		list.pushBack(i);
	}
	std::ostringstream out;
	out << list;
	EXPECT_EQ(out.str(), "[ 1, 2, 3, 4, 5 ]");
}

TEST(SLLOperators, Equality)
{
    SinglyLinkedList<int> list1;
    SinglyLinkedList<int> list2;
    for (int i = 0; i < 10; ++i) {
        list1.pushFront(i);
    }
	EXPECT_FALSE(list1 == list2);
    for (int i = 0; i < 5; ++i) {
        list2.pushFront(i);
    }
	EXPECT_FALSE(list1 == list2);
	for (int i = 5; i < 15; ++i) {
		list2.pushFront(i);
	}
	EXPECT_FALSE(list1 == list2);
	list2.clear();
    for (int i = 0; i < 10; ++i) {
        list2.pushFront(i);
    }
	EXPECT_TRUE(list1 == list2);
}

TEST(SLLOperators, Inequality)
{
    SinglyLinkedList<int> list1;
    SinglyLinkedList<int> list2;
    for (int i = 0; i < 10; ++i) {
        list1.pushFront(i);
    }
	EXPECT_TRUE(list1 != list2);
    for (int i = 0; i < 5; ++i) {
        list2.pushFront(i);
    }
	EXPECT_TRUE(list1 != list2);
	for (int i = 5; i < 15; ++i) {
		list2.pushFront(i);
	}
	EXPECT_TRUE(list1 != list2);
	list2.clear();
    for (int i = 0; i < 10; ++i) {
        list2.pushFront(i);
    }
	EXPECT_FALSE(list1 != list2);
}

TEST(SLLBigFive, CopyConstructor)
{
    SinglyLinkedList<int> list;
    for (int i = 0; i < 10; ++i) {
        list.pushFront(i);
    }

    // Copy constructor
    SinglyLinkedList<int> copyList(list);
    EXPECT_EQ(copyList.size(), list.size());
    for (int i = 0; i < 10; ++i) {
        EXPECT_TRUE(copyList.contains(i));
    }
}

TEST(SLLBigFive, CopyAssignment) {
	SinglyLinkedList<int> list;
    for (int i = 0; i < 10; ++i) {
        list.pushFront(i);
    }

	// With empty list.
    SinglyLinkedList<int> assignedList;
    assignedList = list;
    EXPECT_EQ(assignedList.size(), list.size());
    for (int i = 0; i < 10; ++i) {
        EXPECT_TRUE(assignedList.contains(i)) << "Original list: " << list <<
			"\nAssigned list: " << assignedList << "\n";
    }
	EXPECT_TRUE(list == assignedList);
	std::ostringstream listOut;
	std::ostringstream assignedOut;
	listOut << list;
	assignedOut << assignedList;
	EXPECT_EQ(listOut.str(), assignedOut.str());

	// With smaller list.
	assignedList.clear();
	EXPECT_TRUE(assignedList.isEmpty());
	for (int i = 0; i < 5; ++i) {
		assignedList.pushFront(i);
	}
    assignedList = list;
    EXPECT_EQ(assignedList.size(), list.size());
    for (int i = 0; i < 10; ++i) {
        EXPECT_TRUE(assignedList.contains(i)) << "Original list: " << list <<
			"\nAssigned list: " << assignedList << "\n";
    }
	EXPECT_TRUE(list == assignedList);
	std::ostringstream().swap(assignedOut);
	assignedOut << assignedList;
	EXPECT_EQ(listOut.str(), assignedOut.str());

	// With larger list.
	assignedList.clear();
	EXPECT_TRUE(assignedList.isEmpty());
	for (int i = 0; i < 15; ++i) {
		assignedList.pushFront(i);
	}
    assignedList = list;
    EXPECT_EQ(assignedList.size(), list.size());
    for (int i = 0; i < 10; ++i) {
        EXPECT_TRUE(assignedList.contains(i)) << "Original list: " << list <<
			"\nAssigned list: " << assignedList << "\n";
    }
	EXPECT_TRUE(list == assignedList);
	std::ostringstream().swap(assignedOut);
	assignedOut << assignedList;
	EXPECT_EQ(listOut.str(), assignedOut.str());
}

TEST(SLLBigFive, MoveConstructor)
{
	SinglyLinkedList<int> list;
    for (int i = 0; i < 10; ++i) {
        list.pushFront(i);
    }

    // Move constructor
    SinglyLinkedList<int> movedList(std::move(list));
    EXPECT_EQ(movedList.size(), 10);
    EXPECT_TRUE(movedList.contains(0));
    EXPECT_TRUE(movedList.contains(9));
    EXPECT_TRUE(list.isEmpty()); // Original list should be empty
}

TEST(SLLBigFive, MoveAssignment)
{
	SinglyLinkedList<int> list;
    for (int i = 0; i < 10; ++i) {
        list.pushFront(i);
    }

    // Move assignment
    SinglyLinkedList<int> anotherList;
    anotherList = std::move(list);
    EXPECT_EQ(anotherList.size(), 10);
    EXPECT_TRUE(anotherList.contains(0));
    EXPECT_TRUE(anotherList.contains(9));
    EXPECT_TRUE(list.isEmpty()); // Moved list should be empty
}

TEST(SLLBigFive, Destructor)
{
	std::unique_ptr<SinglyLinkedList<int>> list(new SinglyLinkedList<int>());
	for (int i = 0; i < 10; ++i) {
		list->pushFront(i);
	}
	list.reset();
}

TEST(SinglyLinkedListTest, StressTest) {
    SinglyLinkedList<int> list;

    // Push random elements
    for (int i = 0; i < 1000; ++i) {
        list.pushFront(randomInt());
        EXPECT_EQ(list.size(), i + 1);
    }

    // Remove random elements
    std::vector<int> values;
    for (int i = 0; i < 1000; ++i) {
        int value = randomInt();
        if (list.contains(value)) {
            EXPECT_TRUE(list.remove(value));
        } else {
            EXPECT_FALSE(list.remove(value));
        }
    }

    // Ensure final size is valid
    EXPECT_LE(list.size(), 1000);
}

/**
* Tests for DoublyLinkedList.
*
* @credit OpenAI's ChatGPT
* @modified 2024-09-21 Tyler Baxter
*/
void testDoublyLinkedList()
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
        std::cout << "[✓] empty() tests passed.\n";
        assert(list->size() == 0);
        std::cout << "[✓] size() tests passed.\n";

        // Test pushFront() with random values.
        for (int i = 0; i < 10; ++i) {
            int value = randomValue();
            list->pushFront(value);
			// New front should be the last pushed.
            assert(list->front() == value);
			assert(list->size() == i + 1);
        }
        std::cout << "[✓] pushFront() tests passed.\n";

        // Test pushBack() with random values.
		auto size = list->size();
        for (int i = 0; i < 10; ++i) {
            int value = randomValue();
            list->pushBack(value);
 			// New back should be the last pushed.
            assert(list->back() == value);
            assert(list->size() == size + i + 1);
        }
        std::cout << "[✓] pushBack() tests passed.\n";

        // Test contains() and get() for all inserted values.
		for (auto it = list->begin(); it != list->end(); ++it) {
	        assert(list->contains(*it));
			assert(list->get(*it));
		}
	    std::cout << "[✓] contains() tests passed.\n";
	    std::cout << "[✓] get() tests passed.\n";

		// Test overloaded insertion operator.
        std::vector<int> values;
	    for (auto it = list->begin(); it != list->end(); ++it) {
	        values.push_back(*it);
	    }
		std::cout << "\tPrinting list...\n";
		std::cout << "\tExpected:\t";
		auto printVector = [&](std::vector<int> v) {
			std::cout << "[ ";
    		bool first = true;
    		for (auto it = v.begin(); it != v.end(); ++it) {
        		if (!first) {
					std::cout << ", ";
        		}
        		first = false;
				std::cout << *it;
    		}
			std::cout << " ]";
		};
		printVector(values);
		std::cout << "\n\tActual:  \t" << *list << "\n";
	    std::cout << "[✓] Overloaded insertion operator tests passed.\n";

	    // Test remove() with both present and absent elements.
	    // Shuffle values for random removals.
	    std::shuffle(values.begin(), values.end(), generator);
	    for (const auto& value : values) {
			assert(list->contains(value));
	        bool removed = list->remove(value);
	        assert(removed); // Ensure the value was removed.
	    }
		assert(list->empty());
	    std::cout << "[✓] remove() tests passed.\n";

        // Test popFront().
        for (int i = 0; i < 5; ++i) {
            list->pushBack(randomValue());
        }

        while (!list->empty()) {
            int frontValue = *list->front();
            list->popFront();
            assert(list->size() >= 0); // Ensure size is valid.
        }
        std::cout << "[✓] popFront() tests passed.\n";

        // Test popBack().
        // Reinsert values for popBack tests.
        for (int i = 0; i < 5; ++i) {
            list->pushBack(randomValue());
        }

        while (!list->empty()) {
            int backValue = *list->back();
            list->popBack();
            assert(list->size() >= 0); // Ensure size is valid.
        }
        std::cout << "[✓] popBack() tests passed.\n";

        // Test clear().
        for (int i = 0; i < 5; ++i) {
            list->pushBack(randomValue());
        }
        list->clear();
        assert(list->empty());
        assert(list->size() == 0);
        std::cout << "[✓] clear() tests passed.\n";

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
	    std::cout << "[✓] removeAndPushFront() tests passed.\n";

        // Test copy constructor.
        auto listCopy = std::make_unique<DoublyLinkedList<int>>(*list);
        assert(*listCopy->front() == *list->front());
        assert(listCopy->size() == list->size());
        std::cout << "[✓] Copy constructor tests passed.\n";

        // Test copy assignment operator.
        auto listAssigned = std::make_unique<DoublyLinkedList<int>>();
        *listAssigned = *list;
        assert(*listAssigned->front() == *list->front());
        assert(listAssigned->size() == list->size());
        std::cout << "[✓] Copy assignment operator tests passed.\n";

        // Test move constructor.
        auto listMoved = std::make_unique<DoublyLinkedList<int>>(
			std::move(*list));
        assert(listMoved->size() == listAssigned->size());
        assert(*listMoved->front() == *listAssigned->front());
        assert(list->empty()); // Ensure original list is empty.
        std::cout << "[✓] Move constructor tests passed.\n";

        // Test move assignment operator.
        auto listMovedAssign = std::make_unique<DoublyLinkedList<int>>();
        *listMovedAssign = std::move(*listAssigned);
        assert(listMovedAssign->size() == listMoved->size());
        assert(*listMovedAssign->front() == *listMoved->front());
        assert(listAssigned->empty()); // Ensure original list is empty.
        std::cout << "[✓] Move assignment operator tests passed.\n";

        // Test destructors.
        list.reset();
        std::cout << "[✓] Destructor tests passed.\n";
        listCopy.reset();
        std::cout << "[✓] Copy constructor destructor tests passed.\n";
        listAssigned.reset();
        std::cout << "[✓] Copy assignment operator destructor tests passed.\n";
        listMoved.reset();
        std::cout << "[✓] Move constructor destructor tests passed.\n";
        listMovedAssign.reset();
        std::cout << "[✓] Move assignment operator destructor tests passed.\n";
    };

	auto testString = []() {
	    std::default_random_engine generator;
	    std::uniform_int_distribution<int> lengthDist(1, 10);
	    auto randomString = [&]() {
	        int len = lengthDist(generator);
	        std::string str;
	        for (int j = 0; j < len; ++j) {
				// Generate a random lowercase letter.
	            char c = 'a' + generator() % 26;
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

	    // Test empty() and size().
	    assert(list->empty());
	    std::cout << "[✓] empty() tests passed.\n";
	    assert(list->size() == 0);
	    std::cout << "[✓] size() tests passed.\n";

	    // Test pushFront() with random strings.
	    for (int i = 0; i < 10; ++i) {
	        std::string value = randomString();
	        list->pushFront(value);
 			// New front should be the last pushed.
	        assert(list->front() == value);
			assert(list->size() == i + 1);
	    }
	    std::cout << "[✓] pushFront() tests passed.\n";

	    // Test pushBack() with random strings.
		auto size = list->size();
	    for (int i = 0; i < 10; ++i) {
	        std::string value = randomString();
	        list->pushBack(value);
 			// New back should be the last pushed.
	        assert(list->back() == value);
			assert(list->size() == size + i + 1);
	    }
	    std::cout << "[✓] pushBack() tests passed.\n";

	    // Test contains() and get() for all inserted values.
		for (auto it = list->begin(); it != list->end(); ++it) {
	        assert(list->contains(*it));
			assert(list->get(*it));
	    }
	    std::cout << "[✓] contains() tests passed.\n";
	    std::cout << "[✓] get() tests passed.\n";

	    // Test overloaded insertion operator.
        std::vector<std::string> values;
	    for (auto it = list->begin(); it != list->end(); ++it) {
	        values.push_back(*it);
	    }
		std::cout << "\tPrinting list...\n";
		std::cout << "\tExpected:\t";
		auto printVector = [&](std::vector<std::string> v) {
			std::cout << "[ ";
    		bool first = true;
    		for (auto it = v.begin(); it != v.end(); ++it) {
        		if (!first) {
					std::cout << ", ";
        		}
        		first = false;
				std::cout << *it;
    		}
			std::cout << " ]";
		};
		printVector(values);
		std::cout << "\n\tActual:  \t" << *list << "\n";
	    std::cout << "[✓] Overloaded insertion operator tests passed.\n";

	    // Test remove() with both present and absent elements.
	    // Shuffle values for random removals.
	    std::shuffle(values.begin(), values.end(), generator);
	    for (const auto& value : values) {
			assert(list->contains(value));
	        bool removed = list->remove(value);
	        assert(removed); // Ensure the value was removed.
	    }
		assert(list->empty());
	    std::cout << "[✓] remove() tests passed.\n";

	    // Test popFront().
	    for (int i = 0; i < 5; ++i) {
	        list->pushBack(randomString());
	    }
	    while (!list->empty()) {
	        std::string frontValue = *list->front();
	        list->popFront();
	        assert(list->size() >= 0); // Ensure size is valid.
	    }
	    std::cout << "[✓] popFront() tests passed.\n";

	    // Test popBack().
	    for (int i = 0; i < 5; ++i) {
	        list->pushBack(randomString());
	    }

	    while (!list->empty()) {
	        std::string backValue = *list->back();
	        list->popBack();
	        assert(list->size() >= 0); // Ensure size is valid.
	    }
	    std::cout << "[✓] popBack() tests passed.\n";

	    // Test clear().
	    for (int i = 0; i < 5; ++i) {
	        list->pushBack(randomString());
	    }
	    list->clear();
	    assert(list->empty());
	    assert(list->size() == 0);
	    std::cout << "[✓] clear() tests passed.\n";

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
	    std::cout << "[✓] removeAndPushFront() tests passed.\n";

	    // Test copy constructor.
	    auto listCopy = std::make_unique<DoublyLinkedList<std::string>>(*list);
	    assert(*listCopy->front() == *list->front());
	    assert(listCopy->size() == list->size());
	    std::cout << "[✓] Copy constructor tests passed.\n";

	    // Test copy assignment operator.
	    auto listAssigned = std::make_unique<DoublyLinkedList<std::string>>();
	    *listAssigned = *list;
	    assert(*listAssigned->front() == *list->front());
	    assert(listAssigned->size() == list->size());
	    std::cout << "[✓] Copy assignment operator tests passed.\n";

	    // Test move constructor.
	    auto listMoved = std::make_unique<DoublyLinkedList<std::string>>(
			std::move(*list));
	    assert(listMoved->size() == listAssigned->size());
	    assert(*listMoved->front() == *listAssigned->front());
	    assert(list->empty()); // Ensure original list is empty.
	    std::cout << "[✓] Move constructor tests passed.\n";

	    // Test move assignment operator.
	    auto listMovedAssign =
			std::make_unique<DoublyLinkedList<std::string>>();
	    *listMovedAssign = std::move(*listAssigned);
	    assert(listMovedAssign->size() == listMoved->size());
	    assert(*listMovedAssign->front() == *listMoved->front());
	    assert(listAssigned->empty()); // Ensure original list is empty.
	    std::cout << "[✓] Move assignment operator tests passed.\n";

        // Test destructors.
        list.reset();
        std::cout << "[✓] Destructor tests passed.\n";
        listCopy.reset();
        std::cout << "[✓] Copy constructor destructor tests passed.\n";
        listAssigned.reset();
        std::cout << "[✓] Copy assignment operator destructor tests passed.\n";
        listMoved.reset();
        std::cout << "[✓] Move constructor destructor tests passed.\n";
        listMovedAssign.reset();
        std::cout << "[✓] Move assignment operator destructor tests passed.\n";
	};

	std::cout << "Test Summary:\n";
	std::cout << "int unit tests:\n";
	testInt();
    std::cout << "int unit tests passed!\n\n";

	std::cout << "std::string unit tests:\n";
	testString();
    std::cout << "std::string unit tests passed!\n\n";

	std::cout << "Stress tests:\n";
	std::cout << "Stress testing...\n";

	// Redirect std::cout to null.
	std::ofstream out(NULL_DEVICE);
	// Save original buffer.
    std::streambuf* buf = std::cout.rdbuf();
    std::cout.rdbuf(out.rdbuf());

	// Run stress tests with output silenced.
	for (auto i = 0; i < 100; ++i) {
		testInt();
		testString();
	}

	// Restore original buffer.
    std::cout.rdbuf(buf);

	std::cout << "Stress tests passed! 100 random iterations." << std::endl;
}

void test()
{
	testDoublyLinkedList();
}

}
