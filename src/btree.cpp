#include "btree.h"

using namespace csc;

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

void BTree::insert(int key) {
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
