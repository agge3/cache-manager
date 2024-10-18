#include "bst.h"

#include <iostream>

void BST::insert(Node* node)
{
    _root = insert_helper(_root, node);
}

void BST::display() const
{
    display_helper(_root);
}

void BST::rdisplay() const
{
    rdisplay_helper(_root);
}

bool BST::search(int data) const
{
    return search_helper(_root, data);
}

void BST::remove(int data)
{
    if (search(data)) {
        remove_helper(_root, data);
    } else {
        std::cout << data << " could not be found!\n";
    }
}

Node* BST::insert_helper(Node* root, Node* node)
{
    if (root == nullptr) {
        root = node;
        return root;
    } else if (node->get_data() < root->get_data()) {
        root->set_left(insert_helper(root->get_left(), node));
    } else {
        root->set_right(insert_helper(root->get_right(), node));
    }

    return root;
}

void BST::display_helper(Node* root) const
{
    if (root != nullptr) {
        // inorder traversal -- recursion
        // displays least value first, then ascending (non-decreasing) order
        display_helper(root->get_left());
        std::cout << root->get_data() << "\n";
        display_helper(root->get_right());
    }
}

void BST::rdisplay_helper(Node* root) const
{
    if (root != nullptr) {
        // displays highest value first, then descending (non-increasing) order
        rdisplay_helper(root->get_right());
        std::cout << root->get_data() << "\n";
        rdisplay_helper(root->get_left());
    }
}

bool BST::search_helper(Node* root, int data) const
{
    // tree is empty, can't search for anything!
    if (root == nullptr) {
        return false;
    // found the data we're looking for
    } else if (root->get_data() == data) {
        return true;
    // if root data is greater than data we're looking for, go left
    } else if (root->get_data() > data) {
        return search_helper(root->get_left(), data);
    // else, go right
    } else {
        return search_helper(root->get_right(), data);
    }
}

Node* BST::remove_helper(Node* root, int data)
{
    if (root == nullptr) {
        return root;
    } else if (data < root->get_data()) {
        root->set_left(remove_helper(root->get_left(), data));
    } else if (data > root->get_data()) {
        root->set_right(remove_helper(root->get_right(), data));
    // node found
    } else {
        // leaf node
        if (root->get_left() == nullptr && root->get_right() == nullptr) {
            root = nullptr;
        // right child
        } else if (root->get_right() != nullptr) {
            // find a successor to replace this node
            root->set_data(successor(root));
            root->set_right(remove_helper(root->get_right(), root->get_data()));
        // left child
        } else {
            // find a predecessor to replace this node
            root->set_data(predecessor(root));
            root->set_left(remove_helper(root->get_left(), root->get_data()));
        }
    }

    return root;
}

/**
* Find the least value below the right child of this root node.
*/
int BST::successor(Node* root) const
{
    root = root->get_right();
    while (root->get_left() != nullptr) {
        root = root->get_left();
    }
    return root->get_data();
}

/**
* Find the greatest value below the left child of this root node.
*/
int BST::predecessor(Node* root) const
{
    root = root->get_left();
    while (root->get_right() != nullptr) {
        root = root->get_right();
    }
    return root->get_data();
}

BST::~BST() {}
