/**
*
* HashNode.hpp : This is the header file for HashNode.
*
* 09/23/24 - Created by ChatGPT
* 09/23/24 - Modified by jhui
* 11/07/24 - Modified by jhui
*/

#ifndef HASH_NODE
#define HASH_NODE

#include "Node.hpp"
#include <string>

/**
* HashNode
*
* This class is the node used by the HashTable.
*/
class HashNode : public Node {
private:
    Node* cacheNode;

public:
     HashNode(int value);
};



#endif // HASH_NODE
