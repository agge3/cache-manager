#pragma once

class Node {
public:
    Node(int data) : _data(data), _left(nullptr), _right(nullptr) {}
    int get_data() const            { return _data; }
    void set_data(int data)         { _data = data; }
    Node* get_left() const          { return _left; }
    void set_left(Node* left)       { _left = left; }
    Node* get_right() const         { return _right; }  
    void set_right(Node* right)     { _right = right; }
private:
    int _data;
    Node* _left;
    Node* _right;
};

class BST {
public:
    BST() : _root(nullptr) {}
    ~BST();

    void insert(Node* node);
    void display() const;
    void rdisplay() const;
    bool search(int data) const;
    void remove(int data);
private:
    Node* insert_helper(Node* root, Node* node);
    void display_helper(Node* root) const;
    void rdisplay_helper(Node* root) const;
    bool search_helper(Node* root, int data) const;
    Node* remove_helper(Node* root, int data);
    int successor(Node* root) const;
    int predecessor(Node* root) const;

    Node* _root;
};
