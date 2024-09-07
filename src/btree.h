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
	BTreeNode** _C;
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
};
