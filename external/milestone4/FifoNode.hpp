/**
*
* FifoNode.hpp : This is the header file for FifoNode.
*
* 09/23/24 - Created by ChatGPT
* 09/23/24 - Modified by jhui
* 11/07/24 - Modified by jhui
*/

#ifndef FIFO_NODE
#define FIFO_NODE

#include "Node.hpp"
#include <string>


/**
* Data
*
* This struct is used to contain the data to be stored in the FIFO list.
*/
struct Data {
	int key;
	std::string fullName;
	std::string address;
	std::string city;
	std::string state;
	std::string zip;

	Data(int keyValue, std::string fullName, std::string address, std::string city, std::string state, std::string zip);
};

/**
* FifoNode
*
* This class is the node used by the FIFO cache.
*/

class FifoNode : public Node {
private:
	Data* dataValues;

public:
    FifoNode(int value);
};

#endif // FIFO_NODE
