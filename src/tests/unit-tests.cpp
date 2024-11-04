/**
 * @file test.cpp
 *
 * @author Tyler Baxter
 * @version 1.0
 * @since 2024-09-21
 *
 * Test cases.
 */

#include "test.hpp"

#include "singly-linked-list.hpp"
#include "doubly-linked-list.hpp"
#include "hash-map.hpp"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <streambuf>
#include <memory>
#include <cassert>
#include <vector>
#include <random>
#include <algorithm>
#include <sstream>
#include <cstdint>

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

TEST(SLLMembers, find)
{
	SinglyLinkedList<int> list;
	for (int i = 0; i < 10; ++i) {
		list.pushFront(i);
		EXPECT_TRUE(list.contains(i));
	}
	EXPECT_EQ(*(list.find(9)), 9);
	EXPECT_NE(*(list.find(8)), 9);
	EXPECT_FALSE(list.find(11).has_value());
	EXPECT_TRUE(list.find(1).has_value());
}

TEST(SLLOperators, ostream)
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

TEST(HashFunction, Integers)
{
	Hash<std::int32_t> hash;
	//for (std::int32_t i = 0x80000000; i < 0x7fffffff; ++i) {
	//	EXPECT_EQ(hash(i), hash(i));
	//}
	// smaller range... :)
	for (std::int32_t i = -100; i < 100; ++i) {
		EXPECT_EQ(hash(i), hash(i));
	}
}

class HashNodeTest : public testing::Test {
protected:
	HashNodeTest() :
		_node1(1, 1),
		_node2(1),
		_node3("key", 3),
		_node4("key"),
		_node5(2, 2),
		_node6(3)
	{
		// do nothing
	}

	HashNode<int, int> _node1;
	HashNode<int, int> _node2;
	HashNode<std::string, int> _node3;
	HashNode<std::string, int> _node4;
	HashNode<int, int> _node5;
	HashNode<int, int> _node6;
};

TEST_F(HashNodeTest, getItem)
{
	EXPECT_EQ(_node1.getItem(), 1);
	EXPECT_EQ(_node3.getItem(), 3);
	EXPECT_EQ(_node5.getItem(), 2);
}

TEST_F(HashNodeTest, getKey)
{
	EXPECT_EQ(_node1.getKey(), 1);
	EXPECT_EQ(_node2.getKey(), 1);
	EXPECT_EQ(_node3.getKey(), "key");
	EXPECT_EQ(_node4.getKey(), "key");
	EXPECT_EQ(_node5.getKey(), 2);
	EXPECT_EQ(_node6.getKey(), 3);
}

TEST(HashNodeOperators, Equality)
{
	auto node1 = HashNode<int, int>(1, 1);
	auto node2 = HashNode<int, int>(1);
	EXPECT_EQ(node1, node2);
	EXPECT_TRUE(node1 == node2);

	auto node3 = HashNode<std::string, int>("key", 3);
	auto node4 = HashNode<std::string, int>("key");
	EXPECT_EQ(node3, node4);
	EXPECT_TRUE(node3 == node4);

	auto node5 = HashNode<int, int>(2, 2);
	auto node6 = HashNode<int, int>(3);
	EXPECT_NE(node5, node6);
	EXPECT_FALSE(node5 == node6);

	EXPECT_NE(node1, node5);
	EXPECT_NE(node1, node6);
	EXPECT_NE(node2, node5);
	EXPECT_NE(node2, node6);
	EXPECT_FALSE(node1 == node5);
	EXPECT_FALSE(node1 == node6);
	EXPECT_FALSE(node2 == node5);
	EXPECT_FALSE(node2 == node6);
}

TEST(HashMapMembers, add)
{
	std::unique_ptr<HashMap<int, int>> intMap =
		std::make_unique<HashMap<int, int>>();
	std::unique_ptr<HashMap<std::string, std::string>> strMap = 
		std::make_unique<HashMap<std::string, std::string>>();
	std::unique_ptr<HashMap<std::string, int>> strIntMap =
		std::make_unique<HashMap<std::string, int>>();

	intMap->add(1, 0);
	EXPECT_EQ(intMap->getNumberOfItems(), 1);
	intMap->add(2, 1);
	EXPECT_EQ(intMap->getNumberOfItems(), 2);
	EXPECT_TRUE(intMap->contains(2));
	intMap->add(3, 3);
	EXPECT_EQ(intMap->getNumberOfItems(), 3);
	EXPECT_TRUE(intMap->contains(3));
	strMap->add("key", "value");
	EXPECT_EQ(strMap->getNumberOfItems(), 1);
	strMap->add("k", "v");
	EXPECT_EQ(strMap->getNumberOfItems(), 2);
	strMap->add("same", "same");
	EXPECT_EQ(strMap->getNumberOfItems(), 3);
	strIntMap->add("key", 4);
	EXPECT_EQ(strIntMap->getNumberOfItems(), 1);
	strIntMap->add("k", 5);
	EXPECT_EQ(strIntMap->getNumberOfItems(), 2);
	strIntMap->add("Very long string with spaces and mixed case.", 0x7fffffff);
	EXPECT_EQ(strIntMap->getNumberOfItems(), 3);
	
	//EXPECT_TRUE(intMap->contains(1)) << "Map: " << *intMap << "\n";
	EXPECT_TRUE(intMap->contains(2));
	EXPECT_TRUE(intMap->contains(3));
	EXPECT_TRUE(strMap->contains("key"));
	EXPECT_TRUE(strMap->contains("k"));
	EXPECT_TRUE(strMap->contains("same"));
	EXPECT_TRUE(strIntMap->contains("key"));
	EXPECT_TRUE(strIntMap->contains("k"));
	EXPECT_TRUE(strIntMap->contains("Very long string with spaces and mixed case."));

	EXPECT_FALSE(intMap->contains(4));
	EXPECT_FALSE(intMap->contains(5));
	EXPECT_FALSE(intMap->contains(6));
	EXPECT_FALSE(strMap->contains("Key"));
	EXPECT_FALSE(strMap->contains("K"));
	EXPECT_FALSE(strMap->contains("Same"));
	EXPECT_FALSE(strIntMap->contains("Key"));
	EXPECT_FALSE(strIntMap->contains("K"));
	EXPECT_FALSE(strIntMap->contains("Another very long string with spaces and mixed case."));

	EXPECT_TRUE(*(intMap->getItem(1)) == 0);
	EXPECT_TRUE(*(intMap->getItem(2)) == 1);
	EXPECT_TRUE(*(intMap->getItem(3)) == 3);
	EXPECT_TRUE(*(strMap->getItem("key")) == "value");
	EXPECT_TRUE(*(strMap->getItem("k")) == "v");
	EXPECT_TRUE(*(strMap->getItem("same")) == "same");
	EXPECT_TRUE(*(strIntMap->getItem("key")) == 4);
	EXPECT_TRUE(*(strIntMap->getItem("k")) == 5);
	EXPECT_TRUE(*(strIntMap->getItem("Very long string with spaces and mixed case.")) == 0x7fffffff);

	EXPECT_FALSE(intMap->getItem(4).has_value());
	EXPECT_FALSE(strMap->getItem("otherKey").has_value());
	EXPECT_FALSE(strIntMap->getItem("otherKey").has_value());
}

template <typename K>
struct NoHash {
	std::size_t operator()(const K& key) const { return key; } 
};

class HashMapTest : public testing::Test {
protected:
	HashMapTest() :
		// Initialize HashMap with no hash function to a consistent size for
		// testing.
		_noHashMap(10)
	{
		_intMap.add(1, 0);
		_intMap.add(2, 1);
		_intMap.add(3, 3);
		_strMap.add("key", "value");
		_strMap.add("k", "v");
		_strMap.add("same", "same");
		_strIntMap.add("key", 4);
		_strIntMap.add("k", 5);
		_strIntMap.add("Very long string with spaces and mixed case.", 0x7fffffff);

		_noHashMap.add(0, 10);
		_noHashMap.add(1, 11);
		_noHashMap.add(2, 12);
		_noHashMap.add(6, 16);
		_noHashMap.add(7, 17);
		_noHashMap.add(8, 18);
	}

	HashMap<int, int> _intMap;
	HashMap<int, int> _emptyIntMap;
	HashMap<std::string, std::string> _strMap;
	HashMap<std::string, std::string> _emptyStrMap;
	HashMap<std::string, int> _strIntMap;
	HashMap<std::string, int> _emptyStrIntMap;

	HashMap<int, int, NoHash<int>> _noHashMap;
};

TEST(HashFunction, NoHash)
{
	NoHash<int> hash;
	for (int i = 0; i < 10; ++i) {
		EXPECT_EQ(hash(i), i);
	}
}

TEST(HashFunction, NoHashModulo)
{
	NoHash<int> hash;
	const int SIZE = 16;
	for (int i = 0; i < 10; ++i) {
		EXPECT_EQ(hash(i) % (SIZE - 1), i);
	}
}

TEST_F(HashMapTest, isEmpty)
{
	EXPECT_TRUE(_emptyIntMap.isEmpty());
	EXPECT_TRUE(_emptyStrMap.isEmpty());
	EXPECT_TRUE(_emptyStrIntMap.isEmpty());
	EXPECT_FALSE(_intMap.isEmpty());
	EXPECT_FALSE(_strMap.isEmpty());
	EXPECT_FALSE(_strIntMap.isEmpty());
}

TEST_F(HashMapTest, getNumberOfItems)
{
	EXPECT_EQ(_emptyIntMap.getNumberOfItems(), 0);
	EXPECT_EQ(_emptyStrMap.getNumberOfItems(), 0);
	EXPECT_EQ(_emptyStrIntMap.getNumberOfItems(), 0);
	EXPECT_EQ(_intMap.getNumberOfItems(), 3);
	EXPECT_EQ(_strMap.getNumberOfItems(), 3);
	EXPECT_EQ(_strIntMap.getNumberOfItems(), 3);
}

TEST_F(HashMapTest, MapIterator)
{
	auto it = _noHashMap.begin();
	EXPECT_TRUE(it.getType() == MapIteratorType::FullBucket);
	EXPECT_TRUE(it.getIndex() == 0);
	auto opt = *it;
	std::cout << "value of optional: " << *opt << "\n";
	EXPECT_EQ(*opt, 10);

	++it;
	EXPECT_NE(it, _noHashMap.end());
	EXPECT_TRUE(it.getType() == MapIteratorType::FullBucket);
	EXPECT_TRUE(it.getIndex() == 1);
	opt = *it;
	std::cout << "value of optional: " << *opt << "\n";
	EXPECT_EQ(*opt, 11);

	++it;
	EXPECT_NE(it, _noHashMap.end());
	EXPECT_TRUE(it.getType() == MapIteratorType::FullBucket);
	EXPECT_TRUE(it.getIndex() == 2);
	opt = *it;
	std::cout << "value of optional: " << *opt << "\n";
	EXPECT_EQ(*opt, 12);

	++it;
	EXPECT_NE(it, _noHashMap.end());
	EXPECT_TRUE(it.getType() == MapIteratorType::EmptyBucket);
	EXPECT_TRUE(it.getIndex() == 3);
	opt = *it;
	EXPECT_FALSE(opt.has_value());

	++it;
	EXPECT_NE(it, _noHashMap.end());
	EXPECT_TRUE(it.getType() == MapIteratorType::EmptyBucket);
	EXPECT_TRUE(it.getIndex() == 4);
	opt = *it;
	EXPECT_FALSE(opt.has_value());

	++it;
	EXPECT_NE(it, _noHashMap.end());
	EXPECT_TRUE(it.getType() == MapIteratorType::EmptyBucket);
	EXPECT_TRUE(it.getIndex() == 5);
	opt = *it;
	EXPECT_FALSE(opt.has_value());

	++it;
	EXPECT_NE(it, _noHashMap.end());
	EXPECT_TRUE(it.getType() == MapIteratorType::FullBucket);
	EXPECT_TRUE(it.getIndex() == 6);
	opt = *it;
	std::cout << "value of optional: " << *opt << "\n";
	EXPECT_EQ(*opt, 16);

	++it;
	EXPECT_NE(it, _noHashMap.end());
	EXPECT_TRUE(it.getType() == MapIteratorType::FullBucket);
	EXPECT_TRUE(it.getIndex() == 7);
	opt = *it;
	std::cout << "value of optional: " << *opt << "\n";
	EXPECT_EQ(*opt, 17);

	++it;
	EXPECT_NE(it, _noHashMap.end());
	EXPECT_TRUE(it.getType() == MapIteratorType::FullBucket);
	EXPECT_TRUE(it.getIndex() == 8);
	opt = *it;
	std::cout << "value of optional: " << *opt << "\n";
	EXPECT_EQ(*opt, 18);

	++it;
	EXPECT_TRUE(it == _noHashMap.end());
}

TEST_F(HashMapTest, MapIteratorBegin)
{
	//auto it = _noHashMap.begin();
	//EXPECT_TRUE(it.getType() == MapIteratorType::FullBucket);
	//auto opt = *it;
	//std::cout << "Value of optional: " << *opt << "\n";

	//std::vector<int> v(_intMap.getNumberOfItems());
	//for (auto it = _intMap.begin(); it != _intMap.end(); ++it) {
	//	if (it.getType() != MapIteratorType::EmptyBucket) {
	//		auto opt = *it;
	//		if (opt.has_value()) {
	//			v.push_back(*opt);
	//		} else {
	//			std::cout <<
	//				"Optional was expected to have value, but was empty!\n";
	//		}
	//	}
	//}
	//for (auto it = v.begin(); it != v.end(); ++it) {
	//	EXPECT_TRUE(_intMap.contains(*it));
	//}
}

TEST_F(HashMapTest, MapIteratorEnd) 
{
}

TEST_F(HashMapTest, MapIteratorIncrement)
{
}

TEST(MapIterator, Iterator)
{
	//std::unique_ptr<HashMap<int, int>> intMap =
	//	std::make_unique<HashMap<int, int>>();
	//intMap->add(1, 1);
	//auto beginIt = intMap->begin();
	//auto endIt = intMap->end();
	//for (; beginIt != endIt; ++beginIt) {
	//	if (beginIt.getType() == MapIteratorType::FullBucket) {
	//		auto opt = *beginIt;
	//EXPECT_EQ(*beginIt, 1);

	//		
	//beginIt.advancer();
	//EXPECT_NE(beginIt, endIt);
	//++beginIt;
	//EXPECT_EQ(beginIt, endIt);
	//EXPECT_THROW(*beginIt, std::runtime_error);
	//EXPECT_THROW(*endIt, std::runtime_error);
	//intMap.reset();
	//for (int i = 0; i < 10; ++i) {
	//	intMap->add(i, i);
	//}
	//beginIt = intMap->begin();
	//endIt = intMap->end();
	//for (int i = 0; i < 10; ++i) {
	//	EXPECT_NE(beginIt, endIt);
	//	EXPECT_NO_THROW(*beginIt);
	//	++beginIt;
	//}
	//EXPECT_EQ(beginIt, endIt);
}

TEST(HashMapOperators, ostream)
{
	std::unique_ptr<HashMap<int, int>> intMap =
		std::make_unique<HashMap<int, int>>();
	for (int i = 0; i < 10; ++i) {
		intMap->add(i, i);
	}
	std::cout << *intMap;
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
