/**
*
* milestone3.cpp : This file contains the 'main' function. Program execution begins and ends there.
*
* 09/23/24 - Created by ChatGPT with prompt "write C++ program reads and parses the file: milestone3.json"
*            The file: "milestones3.json" is in the following format:
* 

{
    "hashTable": [
        {"testCase1": [
                {"insertAtHead": 100},
                {"insertAtTail": 10},
                {"insertAtTail": 20}
            ],
        {"testCase6": [
                {"insertAtHead": 30},
                {"insertAtTail": 40},
                {"insertAtTail": 50},
                {"insertAtTail": 60},
                {"insertAtHead": 1000},
                {"deleteTailNode": 0}
            ]
        }
    ]
}
*/
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <string>

#include "json.hpp"
#include "HashTable.hpp"

using json = nlohmann::json;

void processTestCase(HashTable* hashTable, const std::string& testCaseName, const json& testCaseArray) {
    std::cout << "Processing " << testCaseName << ":\n\n";

    for (size_t i = 0; i < testCaseArray.size(); ++i) {
        const json& entry = testCaseArray[i];

        for (json::const_iterator it = entry.begin(); it != entry.end(); ++it) {
            const std::string& actionName = it.key();
            const json& details = it.value();

            if (actionName == "add") {
                HashNode* newNode = new HashNode(details["key"], details["fullName"], details["address"], details["city"], details["state"], details["zip"]);
                hashTable->add(details["key"], newNode);
            }
            else if (actionName == "remove") {
                int key = details["key"];
                hashTable->remove(details["key"]);
            }
            else if (actionName == "clear") {
                hashTable->clear();
            }
        }
    }
}

/**
*
* printTable
*
* Method to print out the contents of table
*
* param: none
*
* returns: nothing, but output is sent to console
*/
void printTable(HashTable *inputTable) {
    std::cout << "\nTable contents " << "(" << inputTable->getNumberOfItems() << " entries):";
    bool isFirst = true;
    HashNode** internalTable = inputTable->getTable();

    for (size_t i = 0; i < inputTable->getSize(); ++i) {
        HashNode* curEntry = internalTable[i];
        if (curEntry == nullptr) {
            if (isFirst) {
                std::cout << "\n\nEmpty: " << i;
                isFirst = false;
            }
            else {
                std::cout << ", " << i;
            }

            continue;
        }

        // have a valid entry in the table
        isFirst = true;
        while (curEntry) {
            // curEntry is pointing to the next node
            if (isFirst) {
                std::cout << "\n\nIndex: " << i << ": " << curEntry->key;
                isFirst = false;
            }
            else {
                std::cout << ", " << curEntry->key;
            }

            curEntry = curEntry->next;
        }

        isFirst = true;
    }

    std::cout << "\n\nEnd of table\n\n";
}

int main() {
    // create the hash table
    HashTable* hashTable = new HashTable();

    // Load the JSON file
    std::ifstream inputFile("milestone3.json");
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open the file.\n";
        return 1;
    }

    json data;
    inputFile >> data;
    inputFile.close();

    // Process the test cases in the json file
    for (size_t i = 0; i < data["hashTable"].size(); ++i) {
        const json& testCase = data["hashTable"][i];
        for (json::const_iterator it = testCase.begin(); it != testCase.end(); ++it) {
            const std::string& testCaseName = it.key();
            const json& testCaseArray = it.value();
            processTestCase(hashTable, testCaseName, testCaseArray);

            // print out the table
            printTable(hashTable);

            // clear hashTable out for the next test case
            hashTable->clear();
        }
    }
  
    return 0;
}
