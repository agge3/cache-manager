#define _CRT_SECURE_NO_WARNINGS

#include "singly-linked-list.hpp"
#include "hash-map.hpp"
#include "cache-manager.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <random>

namespace {

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
	EXPECT_EQ(*opt, 10);

	++it;
	EXPECT_NE(it, _noHashMap.end());
	EXPECT_TRUE(it.getType() == MapIteratorType::FullBucket);
	EXPECT_TRUE(it.getIndex() == 1);
	opt = *it;
	EXPECT_EQ(*opt, 11);

	++it;
	EXPECT_NE(it, _noHashMap.end());
	EXPECT_TRUE(it.getType() == MapIteratorType::FullBucket);
	EXPECT_TRUE(it.getIndex() == 2);
	opt = *it;
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
	EXPECT_EQ(*opt, 16);

	++it;
	EXPECT_NE(it, _noHashMap.end());
	EXPECT_TRUE(it.getType() == MapIteratorType::FullBucket);
	EXPECT_TRUE(it.getIndex() == 7);
	opt = *it;
	EXPECT_EQ(*opt, 17);

	++it;
	EXPECT_NE(it, _noHashMap.end());
	EXPECT_TRUE(it.getType() == MapIteratorType::FullBucket);
	EXPECT_TRUE(it.getIndex() == 8);
	opt = *it;
	EXPECT_EQ(*opt, 18);

	++it;
	EXPECT_TRUE(it == _noHashMap.end());
}

TEST(HashMapOperators, ostream)
{
	std::unique_ptr<HashMap<int, int>> intMap =
		std::make_unique<HashMap<int, int>>();
	for (int i = 0; i < 10; ++i) {
		intMap->add(i, i);
	}
	std::ostringstream out;
	EXPECT_NO_THROW(out << *intMap);
}

} // End namespace anonymous

int main(int argc, char **argv)
{
	// Run Google test suite.
	std::cout << "\nRunning Google test suite:\n";
	::testing::InitGoogleTest(&argc, argv);	
    return RUN_ALL_TESTS();
}
