namespace csc {

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
	int MAX_KEYS = 1024;
}

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
