/**
 * @file btree/unit-tests.cpp
 *
 * @author Tyler Baxter
 * @version 1.0
 * @data 2024.11.13
 *
 * BTree unit tests.
 */

#include "btree.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <memory>

TEST(BTreeTest, Constructor)
{
	const std::size_t MAX_KEYS = 3;
	BTree<int, int> *bt = new BTree<int, int>(MAX_KEYS);
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
