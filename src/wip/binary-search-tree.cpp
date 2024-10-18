#include "binary-search-tree.hpp"

BinarySearchTree<T>::BinarySearchTree(const T& element) :
	_root(element, nullptr, nullptr);
{
	// end constructor
}
BinarySearchTree<T>::BinarySearchTree(const T& element, 
	const BinarySearchTree<T> *leftTree, const BinarySearchTree<T> *rightTree) :
	_root(element, copy(leftTree->_root), copy(rightTree->_root))
{
	// end constructor
}

BinarySearchTree<T>::BinarySearchTree(const BinarySearchTree<T>& tree);
	_root(copy(tree._root));
{
	// end constructor
}

bool BinarySearchTree<T>::isEmpty() const;

int BinarySearchTree<T>::getHeight() const
{
	return getHeightHelper(_root);
}

int BinarySearchTree<T>::getNumberOfNodes() const
{
	return getNumberOfNodesHelper(_root);
}
std::optional<T> getRootElement() const
{
	return !empty() ? std::optional<T>(_root->getElement()) : std::nullopt;
}

void BinarySearchTree<T>::setRootElement(const T& element)
{
	if (empty()) {
		_root = new BSTNode<T>(element);
	} else {
		_root->setElement(element);
	}
}

bool BinarySearchTree<T>::add(const T& element)
{
	BSTNode<T> *node = new BSTNode<T>(element);
	_root = balancedAdd(_root, node);
	return true;
}

bool BinarySearchTree<T>::remove(const T& element)
{
	success = false;
	_root = removeHelper(_root, element, success);
	return success;
}

T getElement(const T& element) const;
bool BinarySearchTree<T>::contains(const T& element) const;
void BinarySearchTree<T>::preorderTraverse(void visit(T&)) const;
void BinarySearchTree<T>::inorderTraverse(void visit(T&)) const;
void BinarySearchTree<T>::postTraverse(void visit(T&)) const;

void clear()
{
	destroyTree(_root);
}

int BinarySearchTree<T>::getHeightHelper(BSTNode<T> *subtree) const
{
	if (!subtree) {
		return 0;
	}
	return 1 + std::max(getHeightHelper(subtree->getLeftChild()),
		getHeightHelper(subtree->getRightChild());
}

int BinarySearchTree<T>::getNumberOfNodesHelper(BSTNode<T> *subtree) const
{
	if (!subtree) {
		return 0;
	}
	// xxx maybe +1 for root?
	return 1 + getNumberOfNodesHelper(subtree->getLeftChild()) +
		getNumberOfNodesHelper(subtree->getRightChild());
}

void BinarySearchTree<T>::destroyTree(BSTNode<T> *subtree)
{
	if (!subtree) {
		destroyTree(subtree->getLeftChild());
		destroyTree(subtree->getRightChild());
		delete subtree;
	}
}

BSTNode<T>* BinarySearchTree<T>::balancedAdd(BSTNode<T> *subtree, BSTNode<T> *node)
{
	if (!subtree) {
		return node;
	}
	BSTNode<T> *leftChild = subtree->getLeftChild();
	BSTNode<T> *rightChild = subtree->getRightChild();
	if (getHeightHelper(leftChild) > getHeightHelper(rightChild)) {
		rightChild = balancedAdd(rightChild, node);
		subtree->setRightChild(rightChild);
	} else {
		leftChild = balancedAdd(leftChild, node);
		subtree->setLeftChild(leftChild);
	}
	return subtree;
}

BSTNode<T>* BinarySearchTree<T>::removeHelper(BSTNode<T> *subtree, 
	const T& element, bool &success)
{
	if (!subtree) {
		success = false;
		return nullptr;
	} else if (subtree->getElement() == element) {
		subtree = removeHelper(subtree);
		success = true;
		return subtree;
	} else if (subtree->getElement() > element) {
		// Search the left subtree.
		BSTNode<T> *tmp = removeNode(
			subtree->getLeftChild(), element, success);
		subtree->setLeftChild(tmp);
		return subtree;
	} else {
		// Search the right subtree.
		BSTNode<T> *tmp = removeNode(
			subtree->getLeftChild(), element, success);
		subtree->setRightChild(tmp);
		return subtree;
	}
}

BSTNode<T>* BinarySearchTree<T>::removeNode(BSTNode<T> *node)
{
	if (node->isLeaf()) {
		// Remove the leaf from the tree.
		delete node;
		node = nullptr;
		return nullptr;
	} else if (node->getLeftChild() && !node->getRightChild()) {
		// Node has only a left child.
		BSTNode<T> *leftChild = node->getLeftChild();
		delete node;
		node = nullptr;
		return leftChild;
	} else if (node->getRightChild() && !node->getLeftChild()) {
		// Node has only a right child.
		BSTNode<T> *rightChild = node->getRightChild();
		delete node;
		node = nullptr;
		return rightChild;
	} else {
		// Node has both children.
		T element = node->getElement();
		BSTNode<T> *tmp = removeLeftmostNode(node->getRightChild(), element);
		node->setRightChild(tmp);
		node->setElement(element);
	}
}

BSTNode<T>* BinarySearchTree<T>::removeLeftmostNode(BSTNode<T> *node, 
	T& element)
{
	if (!node->getLeftChild()) {
		// This is the node we want; it has no left child, but it might have a
		// right subtree.
		element = node->getElement();
		return removeNode(node);
	} else {
		return removeLeftmostNode(node->getLeftChild(), element);
	}
}

BSTNode<T>* BinarySearchTree<T>::moveElementUp(BSTNode<T> *subtree);
BSTNode<T>* BinarySearchTree<T>::find(BSTNode<T> *tree, const T& element, bool &success) const);

void BinarySearchTree<T>::traverse(visit(T&))
{
	bool done = false;
	if (empty()) {	// save creating nodes if we don't have to
		done = true;
	} else {
		std::stack<BSTNode<T>*> stack;
		BSTNode<T> *curr = _root;
	}

	while(!done) {
		if (curr) {
			stack.push(curr);
			curr = curr->getLeftChild();
		} else {
			if (!stack.empty()) {
				visit(stack.top()->getElement());
				stack.pop();
				curr = curr->getRightChild();
			} else {
				done = true;
			}
		}
	}
}

BSTNode<T>* BinarySearchTree<T>::insertInorder(BSTNode<T> *subtree, 
	BSTNode<T> *node)
{
	if (!subtree) {
		return node;
	} else if (subtree->getElement() > node->getElement()) {
		BSTNode<T> *tmp = insertInorder(subtree->getLeftChild(), node);
		subtree->setLeftChild(tmp);
	} else {
		BSTNode<T> *tmp = insertInorder(subtree->getRightChild(), node);
		subtree->setRightChild(tmp);
	}
	return subtree;
}

BSTNode<T>* BinarySearchTree<T>::copy(const BSTNode<T> *tree) const
{
	BSTNode<T> *newTree = nullptr;
	if (!tree) {
		// Copy node.
		newTree = new BSTNode<T>(tree->getElement(), nullptr, nullptr);
		// Copy children.
		newTree->setLeftChild(copy(tree->getLeftChild()));
		newTree->setRightChild(copy(tree->getRightChild()));		
	}
	// Else, tree is empty (newTree is nullptr).
	return newTree;
}




void BinarySearchTree<T>::preorderHelper(void visit(T&), BSTNode<T> *tree) const;
void BinarySearchTree<T>::inorderHelper(void visit(T&), BSTNode<T> *tree) const;
void BinarySearchTree<T>::postorderHelper(void visit(T&), BSTNode<T> *tree) const;
