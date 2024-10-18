#pragma once

template <typename T>
class BSTNode {
public:
	BSTNode(const T& element) :
		_element(element), _leftChild(nullptr), _rightChild(nullptr) {}
	BSTNode(const T& element, BSTNode<T> *leftChild, BSTNode<T> *rightChild) :
		_element(element), _leftChild(left), _rightChild(right) {}
	~BSTNode() {}

	void setElement(const T& element) { _element = element; }
	T getElement() { return _element; }
	bool isLeaf() const { return !_leftChild && !_rightChild; }
	BSTNode<T>* getLeftChild() const { return _leftChild; }
	BSTNode<T>* getRightChild() const { return _rightChild; }
	void setLeftChild(BSTNode<T> *leftChild) { _leftChild = leftChild; }
	void setRightChild(BSTNode<T> *rightChild) { _rightChild = rightChild; }
private:
T _element;
BSTNode<T> *_leftChild;
BSTNode<T> *_rightChild;
};

template <typename T>
class BinarySearchTree {
public:
	BinarySearchTree() : _root(nullptr) {}
	BinarySearchTree(const T& element);
	BinarySearchTree(const T& element, const BinarySearchTree<T> *leftTree, 
		const BinarySearchTree<T> *rightTree);
	BinarySearchTree(const BinarySearchTree<T> *tree);
	~BinarySearchTree() { clear(); }

	bool isEmpty() const;
	int getHeight() const;
	int getNumberOfNodes() const;
	std::optional<T> getRootElement() const;
	void setRootElement(const T& element);
	bool add(const T& element);
	bool remove(const T& element);
	bool contains(const T& element) const;
	void preorderTraverse(void visit(T&)) const;
	void inorderTraverse(void visit(T&)) const;
	void postTraverse(void visit(T&)) const;
	void clear();
protected:
	BSTNode<T>* insertHelper(BSTNode<T> *subtree, BSTNode<T> *node);
	BSTNode<T>* removeHelper(BSTNode<T> *subtree, const T& element, 
		bool& success);
	BSTNode<T>* removeNode(BSTNode<T> *node);
	BSTNode<T>* removeLeftmostNode(BSTNode<T> *node, T& successor);
	BSTNode<T>* findNode(BSTNode<T> *node) const;

	int getHeightHelper(BSTNode<T> *subtree) const;
	int getNumberOfNodesHelper(BSTNode<T> *subtree) const;
	void destroyTree(BSTNode<T> *subtree);	

	BSTNode<T>* findNode(BSTNode<T> *tree, const T& element, bool& success)
		const;
	BSTNode<T>* copy(const BSTNode<T> *tree) const;	

	void preorderHelper(void visit(T&), BSTNode<T> *tree) const;
	void inorderHelper(void visit(T&), BSTNode<T> *tree) const;
	void postorderHelper(void visit(T&), BSTNode<T> *tree) const

	BSTNode<T>* balancedAdd(BSTNode<T> *subtree, BSTNode<T> *node);
	BSTNode<T>* moveElementUp(BSTNode<T> *subtree);
private:
BSTNode<T> *_root;
};
#include "binary-search-tree.cpp"
