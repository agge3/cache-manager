/**
 * A "page" is a contiguous block of data and a "probe" is the first access to 
 * a page.
 */

#include "btree.h"

#include <optional>

using namespace csc;

namespace csc {



/* implementation of a B-tree */
typedef struct btNode *bTree;

/* create a new empty tree */
bTree btCreate(void);

/* free a tree */
void btDestroy(bTree t);

/* return nonzero if key is present in tree */
int btSearch(bTree t, int key);

/* insert a new element into a tree */
void btInsert(bTree t, int key);

/* print all keys of the tree in order */
void btPrintKeys(bTree t);


BTree::BTree()
{
	_root = new BTreeNode();
	_leaf = true;
	_num = 0;
}

int BTree:search_key(int n, const int *a, int key)
{
	int lo;
	int hi;
	int mid;

	lo = -1;
	hi = n;

	while(++lo < hi) {
		mid = (lo + hi) / 2;
		if (a[mid] == key) {
			return mid;
		} else if (a[mid] < key) {
			low = mid;
		} else {
			hi = mid;
		}
	}

	return hi;
}



std::size_t BTree::size() const
{
	return _num;
}

V BTree::get(const K& key) const
{
	return search(_root, key, height);
}

BTreeNode::BTreeNode(int degree, bool leaf) :
	_degree(degree), _leaf(leaf)
{
	// Allocate memory for maximum number of possible keys and child pointers.	
	_keys = new int[2 * degree - 1];
	_C = new BTreeNode*[2 * degree];

	// Initialize the number of keys as zero.
	_num = 0;
}

void BTreeNode::traverse()
{
	for (int i = 0; i < _num; ++i) {
	  if (_leaf == false) {
	  	_C[i]->traverse();
	  }
	}
}

BTreeNode* BTreeNode::search(int key)
{
	// Find the first key greater than or equal to key.
	int i = 0;
	while (i < _num && key > _keys[i]) {
		++i;
	}
	// If the key found is equal to key, return this node.
	if (_keys[i] == key) {
		return this;
	}
	// If the key is not found here and this is a leaf node, return nullptr.
	if (_leaf == true) {
		return nullptr;
	}	
	// Go to the appropriate child.
	return _C[i]->search(key);
}



void BTreeNode::insert_non_full(int key)
{
	// Initialize index as index of the right-most element.
	int i = _num - 1;

	if (_leaf == true) {
    	// The following loop does two things
    	// a) Finds the location of new key to be inserted
    	// b) Moves all greater keys to one place ahead
    	while (i >= 0 && _keys[i] > key) {
    	    _keys[i + 1] = _keys[i];
			--i;
    	}
		
		// Insert the new key at the found location.
		_keys[i + 1] = key;
		++_num;
	} else {
		// Find the child which is going to have the new key.
		while (i >= 0 && _keys[i] > key) {
			--i;
		}
		// See if the found child is full.
		if (_C[i + 1]->_num == (2 * degree - 1)) {
			split_child(i + 1, C[i + 1];
		}
        // After the split, the middle key of _C[i] goes up and
        // _C[i] is splitted into two.  See which of the two
        // is going to have the new key.
        if (_keys[i + 1] < key) {
            ++i;
        }
        _C[i + 1]->insert_non_full(key);
	}
}



// XXX
// A utility function to split the child y of this node
// Note that y must be full when this function is called
void BTreeNode::split_child(int i, BTreeNode *y)
{
    // Create a new node which is going to store (t-1) keys
    // of y
    BTreeNode *z = new BTreeNode(y->t, y->leaf);
    z->n = t - 1;

    // Copy the last (t-1) keys of y to z
    for (int j = 0; j < t-1; j++)
        z->keys[j] = y->keys[j+t];

    // Copy the last t children of y to z
    if (y->leaf == false)
    {
        for (int j = 0; j < t; j++)
            z->C[j] = y->C[j+t];
    }

    // Reduce the number of keys in y
    y->n = t - 1;

    // Since this node is going to have a new child,
    // create space of new child
    for (int j = n; j >= i+1; j--)
        C[j+1] = C[j];

    // Link the new child to this node
    C[i+1] = z;

    // A key of y will move to this node. Find the location of
    // new key and move all greater keys one space ahead
    for (int j = n-1; j >= i; j--)
        keys[j+1] = keys[j];

    // Copy the middle key of y to this node
    keys[i] = y->keys[t-1];

    // Increment count of keys in this node
    n = n + 1;
}

template <typename K, typename V>
class BTree {
public:
void add(const K& key, const V& value)
{
	// If the tree is empty:
	if (_root == nullptr) {
		// Allocate memory for _root.
		_root = new BTreenode(_degree, true);
		_root->keys[0] = key; // insert key
		_root->_num = 1; // update the number of keys in root
	// The tree is not empty:
	} else {
		// If _root is full, the tree grows in height.
		if (_root->_num == (2 * degree - 1)) {
			// Allocate memory for new _root.
			BTreeNode* s = new BTreeNode(_degree, false);
			// Make old _root a child of new _root.
			s->_C[0] = _root;
			// Split the old _root and move one key to the new _root.
			s->split_child(0, _root);
			// New _root has two children now. Decide which of the two children
			// is going to have the new key.
			int i = 0;
			if (s->_keys[0] < key) {
				++i;
			}
			s->_C[i]->insert_non_full(key);
			// Change _root.
			_root = s;
		// If root is not full, call insert_non_full helper method for _root.
		} else {
			_root->insert_non_full(key);	
		}
	}
}

bool contains(const K& key)
{
}

int search_key(int n, const int *a, int key)
{
	int lo;
	int hi;
	int mid;

	lo = -1;
	hi = n;

	while(++lo < hi) {
		mid = (lo + hi) / 2;
		if (a[mid] == key) {
			return mid;
		} else if (a[mid] < key) {
			low = mid;
		} else {
			hi = mid;
		}
	}

	return hi;
}

BTree()
{
	_root = new BTreeNode(0, true);
	assert(_root);
}

~BTree()
{
	clear(_root);
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
	return search(_root, key, _height);
}

std::optional<V> search(BTreeNode *node, const K& key, int height)
{
	// external node
	if (height == 0) {
		for (int i = 0; i < node->_numKeys; ++i) {
			if (key == children[i]->key) {
				return std::optional<V>(children[i]->value);
			}
		}
	// internal node
	} else {
		for (int i = 0; i < node->_numKeys; ++i) {
			if (i + 1 == node->_numKeys || key < children[i + 1]->key) {
				return search(children[i]->next, key, height - 1)
			}
		}
	}
	return std::nullopt;
}

bool empty() const
{
	return _root == nullptr;
}

bool size() const
{
	return _size;
}

bool add(const K& key, const V& value)
{
	BTreeNode *myNode = addHelper(_root, key, value, height);
	++_size;
	if (myNode == nullptr) {
		return true;
	}

	// need to split root
	BTreeNode *newRoot = new BTreeNode(2);	// two children
	newRoot->_children[0] = new BTreeNode(_root->_children[0]->_key, nullptr, _root);
	newRoot->_children[1] = new BTreeNode(myNode->_children[0]->_key, nullptr, otherNode);
	_root = myNode;
	++height;
}

private:
	BTreeNode *_root;
	std::size_t _capacity;
	std::size_t _size;
	const std::size_t MAX_KEYS = 5;
};

class BTreeNode {
BTreeNode(std::size_t numKeys, bool leaf)
{
	_numKeys = numKeys;
	_leaf = leaf;
	_children = new BTreeNode*[MAX_KEYS + 1];
}


private:

BTreeNode* addHelper(BTreeNode *node, const K& key, const V& value, int height)
{
	int idx = 0;
	BTreeNode *entry = new BTreeNode(key, value, nullptr);

	// external node
	if (height == 0) {
		for (; idx < node->_numKeys; ++idx) {
			if (key < node->_children[idx]->key) {
				break;
			}
		}
	// internal node
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

BTreeNode* split(BTreeNode* node)
{
	const int SPLIT = MAX_KEYS / 2;
	BTreeNode *otherNode = new BTreeNode(SPLIT);
	node->numberOfChildren = SPLIT;
	for (int i = 0; i < SPLIT; ++i) {
		otherNode->_children[i] = otherNode->_children[SPLIT + i];
	}
	return otherNode;
}





	BTreeNode **_children;
	BTreeNode *_next;
	std::size_t _numberOfChildren;
	bool _leaf;
	const K _key;
	V _value;
	std::size_t _numKeys;
};

class BTreeNode {
public:
	BTreeNode(int degree, bool leaf) : _degree(degree), _leaf(leaf) {}
	void traverse();
	BTreeNode* search(int key)
	friend class BTree;
private:
	int* _keys;
	int _degree;
	BTreeNode** _children;
	int _num;
	bool _leaf;
};

class BTree {
public:
	BTree(int degree) : _degree(degree) {}
	void traverse();
private:
	BTreeNode* _root;
	int _degree;
	int _count;
};

class BTreeNode {
private:
	_leaf;
	int *_keys;
	BTreeNode **_children;
	int _num;
	int MAX_KEYS = 6
}
