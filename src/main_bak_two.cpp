/**
*
* milestone4.cpp : This file contains the 'main' function. Program execution
begins and ends there.
*
* 09/23/24 - Created by ChatGPT with prompt "write C++ program reads and parses
the file: milestone4.json"
*            The file: "milestones4.json" is in the following format:
*

{
	"cacheManager": [
		{"testCase1": [
				{"add": 100},
				{"add": 10},
				{"add": 20}
			],
		{"testCase2": [
				{"add": 30},
				{"add": 40},
				{"add": 50},
				{"add": 60},
				{"add": 1000},
				{"remove": 0}
			]
		}
	]
}
*/

#define _CRT_SECURE_NO_WARNINGS

#include "concurrent-list.hpp"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <thread>

namespace {

using json = nlohmann::json;

struct Data {
	Data(const std::string &fullName, const std::string &address,
		 const std::string &city, const std::string &state,
		 const std::string &zip)
		: _fullName(fullName), _address(address), _city(city), _state(state),
		  _zip(zip) {}
	std::string _fullName;
	std::string _address;
	std::string _city;
	std::string _state;
	std::string _zip;
};
using DataPtr = Data *;

class ConcurrentListTest : public testing::Test {
  protected:
	ConcurrentListTest() {
		std::mt19937 gen(1337);
		std::uniform_int_distribution<int> dist(1, 1000);

		// Generate 20 random integers for _intList
		for (int i = 0; i < 20; ++i) {
			_intList.pushFront(dist(gen));
		}

		// Generate 20 random strings for _strList
		for (int i = 0; i < 20; ++i) {
			_strList.pushFront("Item_" + std::to_string(dist(gen)));
		}
	}

	ConcurrentList<int> _intList;
	ConcurrentList<std::string> _strList;
	ConcurrentList<int> _emptyIntList;
};

TEST_F(ConcurrentListTest, ConcurrentRandomInsertRemove) {
	ConcurrentList<int> list;
	const int numThreads = 4;
	const int opsPerThread = 1000;

	std::vector<std::thread> threads;
	std::mt19937 globalGen(42);
	std::uniform_int_distribution<int> dist(1, 10000);

	for (int t = 0; t < numThreads; ++t) {
		threads.emplace_back(
			[&list, seed = globalGen(), dist, opsPerThread]() mutable {
				std::mt19937 gen(seed);

				for (int i = 0; i < opsPerThread; ++i) {
					int val = dist(gen);
					if (i % 2 == 0) {
						list.pushFront(val);
					} else {
						list.remove(val);
					}
				}
			});
	}

	for (auto &th : threads)
		th.join();

	// Validate that the list remains in a valid state.
	EXPECT_GE(list.size(), 0);
	EXPECT_TRUE(list.isConsistent());
}

/*
TEST(SinglyLinkedListTest, BasicOperationsString) {
	SinglyLinkedList<std::string> list;

	// Initial state
	EXPECT_TRUE(list.isEmpty());
	EXPECT_EQ(list.size(), 0);

	// Push strings
	for (int i = 0; i < 10; ++i) {
		list.pushFront(test::randomString());
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
		list.pushFront(test::randomInt());
		EXPECT_EQ(list.size(), i + 1);
	}

	// Remove random elements
	std::vector<int> values;
	for (int i = 0; i < 1000; ++i) {
		int value = test::randomInt();
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
	EXPECT_TRUE(strIntMap->contains("Very long string with spaces and mixed
case."));

	EXPECT_FALSE(intMap->contains(4));
	EXPECT_FALSE(intMap->contains(5));
	EXPECT_FALSE(intMap->contains(6));
	EXPECT_FALSE(strMap->contains("Key"));
	EXPECT_FALSE(strMap->contains("K"));
	EXPECT_FALSE(strMap->contains("Same"));
	EXPECT_FALSE(strIntMap->contains("Key"));
	EXPECT_FALSE(strIntMap->contains("K"));
	EXPECT_FALSE(strIntMap->contains("Another very long string with spaces and
mixed case."));

	EXPECT_TRUE(*(intMap->getItem(1)) == 0);
	EXPECT_TRUE(*(intMap->getItem(2)) == 1);
	EXPECT_TRUE(*(intMap->getItem(3)) == 3);
	EXPECT_TRUE(*(strMap->getItem("key")) == "value");
	EXPECT_TRUE(*(strMap->getItem("k")) == "v");
	EXPECT_TRUE(*(strMap->getItem("same")) == "same");
	EXPECT_TRUE(*(strIntMap->getItem("key")) == 4);
	EXPECT_TRUE(*(strIntMap->getItem("k")) == 5);
	EXPECT_TRUE(*(strIntMap->getItem("Very long string with spaces and mixed
case.")) == 0x7fffffff);

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
		_strIntMap.add("Very long string with spaces and mixed case.",
0x7fffffff);

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

class CacheManagerTest : public testing::Test {
protected:
	CacheManagerTest() :
		_capacity(10),
		_intCache(_capacity),
		_emptyIntCache(_capacity),
		_strCache(_capacity),
		_emptyStrCache(_capacity),
		_strIntCache(_capacity),
		_emptyStrIntCache(_capacity)
	{
		_intCache.add(1, 0);
		_intCache.add(2, 1);
		_intCache.add(3, 3);
		_strCache.add("key", "value");
		_strCache.add("k", "v");
		_strCache.add("same", "same");
		_strIntCache.add("key", 4);
		_strIntCache.add("k", 5);
		_strIntCache.add("Very long string with spaces and mixed case.",
0x7fffffff);
	}

	std::size_t _capacity;

	CacheManager<int, int> _intCache;
	CacheManager<int, int> _emptyIntCache;
	CacheManager<std::string, std::string> _strCache;
	CacheManager<std::string, std::string> _emptyStrCache;
	CacheManager<std::string, int> _strIntCache;
	CacheManager<std::string, int> _emptyStrIntCache;
};

TEST_F(CacheManagerTest, clear)
{
	_intCache.clear();
	EXPECT_EQ(_intCache.getNumberOfItems(), 0);
	EXPECT_TRUE(_intCache.isEmpty());


	_strCache.clear();
	EXPECT_EQ(_strCache.getNumberOfItems(), 0);
	EXPECT_TRUE(_strCache.isEmpty());

	_strIntCache.clear();
	EXPECT_EQ(_strIntCache.getNumberOfItems(), 0);
	EXPECT_TRUE(_strIntCache.isEmpty());
}

TEST_F(CacheManagerTest, remove)
{
	EXPECT_TRUE(_intCache.remove(1));
	EXPECT_FALSE(_intCache.contains(1));
	EXPECT_FALSE(_emptyIntCache.remove(1));
	EXPECT_FALSE(_emptyIntCache.contains(1));

	EXPECT_TRUE(_strCache.remove("key"));
	EXPECT_FALSE(_strCache.contains("key"));
	EXPECT_FALSE(_emptyStrCache.remove("key"));
	EXPECT_FALSE(_emptyStrCache.contains("key"));

	EXPECT_TRUE(_strIntCache.remove("key"));
	EXPECT_FALSE(_strIntCache.contains("key"));
	EXPECT_FALSE(_emptyStrIntCache.remove("key"));
	EXPECT_FALSE(_emptyStrIntCache.contains("key"));
}

TEST_F(CacheManagerTest, isEmpty)
{
	EXPECT_FALSE(_intCache.isEmpty());
	EXPECT_TRUE(_emptyIntCache.isEmpty());

	EXPECT_FALSE(_strCache.isEmpty());
	EXPECT_TRUE(_emptyStrCache.isEmpty());

	EXPECT_FALSE(_strIntCache.isEmpty());
	EXPECT_TRUE(_emptyStrIntCache.isEmpty());
}

TEST_F(CacheManagerTest, getNumberOfItems)
{
	EXPECT_EQ(_intCache.getNumberOfItems(), 3);
	EXPECT_EQ(_emptyIntCache.getNumberOfItems(), 0);

	EXPECT_EQ(_strCache.getNumberOfItems(), 3);
	EXPECT_EQ(_emptyStrCache.getNumberOfItems(), 0);

	EXPECT_EQ(_strIntCache.getNumberOfItems(), 3);
	EXPECT_EQ(_emptyStrIntCache.getNumberOfItems(), 0);
}

TEST_F(CacheManagerTest, getItem)
{
	EXPECT_EQ(*_intCache.getItem(1), 0);
	EXPECT_EQ(*_intCache.getItem(2), 1);
	EXPECT_EQ(*_intCache.getItem(3), 3);
	EXPECT_FALSE(_emptyIntCache.getItem(1).has_value());
	EXPECT_FALSE(_emptyIntCache.getItem(2).has_value());
	EXPECT_FALSE(_emptyIntCache.getItem(3).has_value());

	EXPECT_EQ(*_strCache.getItem("key"), "value");
	EXPECT_EQ(*_strCache.getItem("k"), "v");
	EXPECT_EQ(*_strCache.getItem("same"), "same");
	EXPECT_FALSE(_emptyStrCache.getItem("key").has_value());
	EXPECT_FALSE(_emptyStrCache.getItem("k").has_value());
	EXPECT_FALSE(_emptyStrCache.getItem("same").has_value());

	EXPECT_EQ(*_strIntCache.getItem("key"), 4);
	EXPECT_EQ(*_strIntCache.getItem("k"), 5);
	EXPECT_EQ(*_strIntCache.getItem("Very long string with spaces and mixed
case."), 0x7fffffff);
	EXPECT_FALSE(_emptyStrIntCache.getItem("key").has_value());
	EXPECT_FALSE(_emptyStrIntCache.getItem("k").has_value());
	EXPECT_FALSE(_emptyStrIntCache.getItem("Very long string with spaces and
mixed case.").has_value());
}

TEST_F(CacheManagerTest, contains)
{
	EXPECT_TRUE(_intCache.contains(1));
	EXPECT_TRUE(_intCache.contains(2));
	EXPECT_TRUE(_intCache.contains(3));
	EXPECT_FALSE(_emptyIntCache.contains(1));
	EXPECT_FALSE(_emptyIntCache.contains(2));
	EXPECT_FALSE(_emptyIntCache.contains(3));

	EXPECT_TRUE(_strCache.contains("key"));
	EXPECT_TRUE(_strCache.contains("k"));
	EXPECT_TRUE(_strCache.contains("same"));
	EXPECT_FALSE(_emptyStrCache.contains("key"));
	EXPECT_FALSE(_emptyStrCache.contains("k"));
	EXPECT_FALSE(_emptyStrCache.contains("same"));

	EXPECT_TRUE(_strIntCache.contains("key"));
	EXPECT_TRUE(_strIntCache.contains("k"));
	EXPECT_TRUE(_strIntCache.contains("Very long string with spaces and mixed
case.")); EXPECT_FALSE(_emptyStrIntCache.contains("key"));
	EXPECT_FALSE(_emptyStrIntCache.contains("k"));
	EXPECT_FALSE(_emptyStrIntCache.contains("Very long string with spaces and
mixed case."));
}

/**
*
* printList
*
* Method to print out the contents of a linked list
*
* param: DoublyLinkedList myList - list to print out
*
* returns: nothing, but output is sent to console
void printList(const CachePtr& myList) {
	if (!myList) {
		std::cout << "\nList is empty.\n";
		return;
	}

	// while there are nodes to process
	std::cout << "List contents in order:" << std::endl;
	std::cout << *myList << "\n" << std::endl;
}
*/
} // namespace

/**
 *
 * main
 *
 * Processing starts and ends with this method
 *
 * param: none
 *
 * returns: nothing, but output is sent to console
 */
int main(int argc, char **argv) {
	// Run test suite.
	std::cout << "Running test suite:\n";
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
