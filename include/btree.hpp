/**
 * A "page" is a contiguous block of data and a "probe" is the first access to 
 * a page.
 */

#include "btree.h"

#include <optional>
#include <iostream>
#include <cassert>

namespace csc {

template <typename K, typename V>
class BTree;

/**
* BTreeEntry is the key entry for each BTreeNode. Value can either be the value
* in case of being a leaf node, or nullptr for internal nodes.
*/
template <typename K, typename V>
class BTreeEntry {
public:
	BTreeEntry(const K& key) : _key(key), _value(nullptr) {}

	BTreeEntry(const K& key, const V& value) :
		_key(key), _value(std::make_unique<V>(value)) {}

	V *getValue() const
	{
		return value;
	}

	bool operator<(const BTreeEntry<K, V>& e1, const BTreeEntry<K, V> e2)
	{
		return e1.key < e2.key;
	}
	bool operator>(const BTreeEntry<K, V>& e1, const BTreeEntry<K, V> e2)
	{
		return e1.key > e2.key;
	}
	bool operator==(const BTreeEntry<K, V>& e1, const BTreeEntry<K, V> e2)
	{
		return e1.key == e2.key;
	}
	bool operator!=(const BTreeEntry<K, V>& e1, const BTreeEntry<K, V> e2)
	{
		return !(e1.key == e2.key);
	}
private:
	const K _key;
	std::unique_ptr<V> _value;
};

template <typename K, typename V>
class BTreeNode {
public:

BTreeNode(std::size_t maxKeys, bool leaf) :
	MAX_KEYS(maxKeys), _leaf(leaf), _numKeys(0)
{
	_keys = new BTreeEntry<K, V>[MAX_KEYS];
	_children = new BTreeNode<K, V>*[MAX_KEYS + 1];
}

~BTreeNode()
{
	for (std::size_t i = 0; i <= _numKeys; ++i) {
		delete _children[i];
	}
	delete[] _keys;
	delete[] _children;
}

private:
friend class BTree<K, V>;



const std::size_t MAX_KEYS;
BTreeEntry<K, V> *keys;
BTreeNode **_children;
bool _leaf;
std::size_t _numKeys;
};

// every node has between [n/2] and [n] children
// split at MAX_KEYS + 1 (degree). OR MAX_KEYS (order M) - 1 (like this more)
// leaves are all at the same height
template <typename K, typename V>
class BTree {
public:
BTree(std::size_t maxKeys)
{
	MAX_KEYS = maxKeys;
	_root = new BTreeNode(MAX_KEYS, true);
	assert(_root);
	_size = 0;
}

~BTree()
{
	clear(_root);
}

bool contains(const K& key)
{
	return search(_root, key) != nullptr;
}

void clear(BTreeNode *node)
{
	if (node->_leaf) {
		for (int i = 0; i < node->numKeys + 1; ++i) {
			clear(node->children[i]);
		}
	}
	delete node;
}

std::optional<V> get(const K& key)
{
	if (_root == nullptr) {
		return std::nullopt;
	}
	BTreeNode *node = search(_root, key);
	if (node != nullptr) {
		for (std::size_t i = 0; i < node->_numKeys; ++i) {
			if (node->_keys[i] == key) {
				return std::optional<V>(_node->_keys[i].value);
			}
		}
	}
	return std::nullopt;
}

bool empty() const
{
	return _root == nullptr && _size == 0;
}

bool size() const
{
	return _size;
}

bool add(const K& key, const V& value)
{
	// If the tree is empty:
	if (_root == nullptr) {
		// Allocate memory for _root.
		_root = new BTreeNode<K, V>(MAX_KEYS, true);
		_root->keys[0] = { key, value }; // insert key-value pair
		_root->_numKeys++;
		++_size;
		return true;
	}

	// The tree is not empty:	
	// If _root is full, the tree grows in height.
	if (_root->_numKeys == MAX_KEYS) {
		BTreeNode<K, V> *newRoot = new BTreeNode<K, V>(MAX_KEYS, false);
		// Make old _root a child of new _root.
		newRoot->_children[0] = _root;
		splitChild(newRoot, 0);
		_root = newRoot;
	} else {
    	// Insert key into the non-full root.
    	addHelper(_root, key, value);
	}
	return true;
}

bool remove(const K& key)
{
	if (empty()) {
		return false;
	}
	bool removed = removeHelper(_root, key);
	if (_root->_numKeys == 0) {
		if (_root->_leaf) {
			_root = nullptr;	// tree is now empty
		} else {
			_root = _root->children[0];
		}
	}
	return removed;
}

bool removeHelper(BTreeNode<K, V> *root, const K& key)
{
	std::size_t i = 0;
	BTreeNode<K, V> *node = search(root, key);
	if (node == nullptr) {
		return false;
	}

	if (node->_leaf) {
		// xxx handle delete
		// null both of them?
		node->_numKeys--;
		--_size;
		return true;
	} else {
		// do nothing, keep index keys intact - they don't need to be modified.
		return false;
	}
}

private:
BTreeNode *search(BTreeNode<K, V> *root, const K& key)
{
	std::size_t = 0;
	while (i < root->_numKeys && key > root->_keys[i]) {
		++i;
	}
	if (i < root->_numKeys && key == root->_keys[i]) {
		return node;
	}
	if (root->_leaf) {
		return nullptr;	// searched all leaves and key was not found
	}
	return search(root->_children[i], key);
}

void addHelper(BTreeNode<K, V> *root, const K& key, const V& value)
{
	// Initialize index as index of the right-most element.
	std::size_t i = root->_numKeys - 1;
	if (root->_leaf) {
		while (i >= 0 && key < root->_keys[i]) {
			root->_keys[i + 1] = root->_keys[i];
			--i;
		}
		root->_keys[i + 1] = new BTreeEntry(key, *value);
		root->_numKeys++;
		++_size;
	} else {
		// Find the child which is going to have the new key.
		while (i >= 0 && key <= node->_keys[i]) {
			--i;
		}
		// See if the found child is full.
		if (node->_children[i]->_numKeys == MAX_KEYS) {
			splitChild(node, i);
		}
       	// After the split, the middle key of children[i] goes up and 
		// children[i] is split into two. See which of the two is going to have
		// the new key.
		if (_key > node->_keys[i + 1]) {
			++i;	// adjust index if new key is greater
		}
		// Recursive call for the child.
		addHelper(node->_children[i + 1], key, value);
	}
}

void splitChild(BTreeNode<K, V> *parent, std::size_t index)
{
	const int SPLIT = MAX_KEYS / 2;
	BTreeNode<K, V> *fullChild = parent->_children[index];
	BTreeNode<K, V> *newChild = new BTreeNode(SPLIT, fullChild->_leaf);
	for (std::size_t i = 0; i < SPLIT; ++i) {
		newChild->_keys[i] = fullChild->_keys[i + SPLIT + 1];
	}
	fullChild->_numKeys = SPLIT + 1;
	newChild->_numKeys = SPLIT;
	for (std::size_t i = parent->_numKeys; i > index; --i) {
		parent->_children[i + 1] = parent->_children[i];
	}
	parent->_children[index + 1] = newChild;

	// Promote the median key to the parent.
	parent->_keys[index] = fullChild->_keys[SPLIT];
	parent->_numKeys++;
}

BTreeNode *_root;
std::size_t _size;
const std::size_t MAX_KEYS;
};


// xxx
BTreeNode* addHelper(BTreeNode *node, const K& key, const V& value)
{
	int i = 0;
	while (i < node->_numKeys && key >= node->children[i]->_key) {
		++i;
	}
	if (i < node->_numKeys && node->_children[i] == key) {
		return nullptr;	// key already exists, do nothing
	}

	if (!node->_leaf && (i + 1 == node->_numKeys ||
		key < node->_children[i + 1]->_key)) {
		BTreeNode *myNode = addHelper(node->_children[++i], key, value);
		if (myNode == nullptr) {
			return nullptr;
		}

	} else {
		for (; idx < node->_numKeys; ++idx) {
			if (idx + 1 == node->_numKeys ||
				key < node->_children[idx + 1]->_key) {
				BTreeNode *otherNode = addHelper(
					node->children[idx++]->_next, key, value, height - 1);
				if (otherNode == nullptr) {
	   				return nullptr;
				}	
				entry->_key = otherNode->_children[0]-?key;
				entry->_value = nullptr;
				entry->_next = otherNode;
				break;
			}
		}
	}

	for (int i = node->_numKeys; i > idx; --i) {
		node->_children[i] = node->_children[i - 1];
		node->_children[idx] = entry;
		node->_numKeys++;
	}
	if (node->_numKeys < MAX_KEYS) {
		return nullptr;
	} else {
		return split(node);
	}
}
