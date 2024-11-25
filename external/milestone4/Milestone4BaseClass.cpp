/**
*
* milestone4.cpp : This file contains the 'main' function. Program execution begins and ends there.
*
* 09/23/24 - Created by ChatGPT with prompt "write C++ program reads and parses the file: milestone4.json"
*            The file: "milestones4.json" is in the following format:
*

{
    "cacheManager": [
        {"testCase1": [
                {"add": 100},
                {"add": 10},
                {"add": 20}
            ],
        {"testCase2": [
                {"add": 30},
                {"add": 40},
                {"add": 50},
                {"add": 60},
                {"add": 1000},
                {"remove": 0}
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
#include "CacheManager.hpp"

using json = nlohmann::json;

/**
*
* processTestCase
*
* Method to process incoming json testcase file
*
* param: 
*
* returns: nothing
*/
void processTestCase(CacheManager* cacheManager, const std::string& testCaseName, const json& testCaseArray) {
    std::cout << "Processing " << testCaseName << ":\n\n";

    for (size_t i = 0; i < testCaseArray.size(); ++i) {
        const json& entry = testCaseArray[i];

        for (json::const_iterator it = entry.begin(); it != entry.end(); ++it) {
            const std::string& actionName = it.key();
            const json& details = it.value();

            if (actionName == "isEmpty") {
                bool result = cacheManager->isEmpty();
                std::cout << "isEmpty: " << result << std::endl;
            }
            else if (actionName == "contains") {
                int key = details["key"];
                bool result = cacheManager->contains(key);
                std::cout << "contains(" << key << "): " << result << std::endl;
            }
            else if (actionName == "getItem") {
                int key = details["key"];
                Node* result = cacheManager->getItem(key);
                std::cout << "getItem(" << key << "): " << result << std::endl;
            }
            else if (actionName == "getNumberOfItems") {
                size_t result = cacheManager->getNumberOfItems();
                std::cout << "getNumberOfItems: " << result << std::endl;
            }
            else if (actionName == "add") {
                Data* newData = new Data(details["key"], details["fullName"], details["address"], details["city"], details["state"], details["zip"]);
                cacheManager->add(details["key"], newData);
            }
            else if (actionName == "remove") {
                int key = details["key"];
                cacheManager->remove(details["key"]);
            }
            else if (actionName == "clear") {
                cacheManager->clear();
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
* param: HashTable inputTable - pointer to hash table to print out
*
* returns: nothing, but output is sent to console
*/
void printTable(HashTable* inputTable) {
    if (!inputTable) {
        std::cout << "\nTable is empty.\n";
        return;
    }

    std::cout << "\nTable contents " << "(" << inputTable->getNumberOfItems() << " entries):";
    bool isFirst = true;
    HashNode** internalTable = inputTable->getTable();

    for (size_t i = 0; i < inputTable->getSize(); ++i) {
        Node* curEntry = internalTable[i];
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

/**
*
* printList
*
* Method to print out the contents of a linked list
*
* param: DoublyLinkedList myList - list to print out
*
* returns: nothing, but output is sent to console
*/
void printList(DoublyLinkedList* myList) {
    if (!myList) {
        std::cout << "\nList is empty.\n";
        return;
    }

    Node* current = myList->getHead();

    // while there are nodes to process
    std::cout << "List contents in order:" << std::endl;
    while (current) {
        std::cout << current->key << " ";
        current = current->next;
    }
    std::cout << std::endl;

    return;
}

/**
*
* main
*
* Processing starts and ends with this method
*
* param: none
*
* returns: nothing, but output is sent to console
*/
int main() {
    // create the hash table
    CacheManager* cacheManager = new CacheManager(101);

    // Load the JSON file
    std::ifstream inputFile("milestone4.json");
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open the file.\n";
        return 1;
    }

    json data;
    inputFile >> data;
    inputFile.close();

    // Process the test cases in the json file
    for (size_t i = 0; i < data["cacheManager"].size(); ++i) {
        const json& testCase = data["cacheManager"][i];
        for (json::const_iterator it = testCase.begin(); it != testCase.end(); ++it) {
            const std::string& testCaseName = it.key();
            const json& testCaseArray = it.value();
            processTestCase(cacheManager, testCaseName, testCaseArray);

            // print out the table
            printTable(cacheManager->getTable());

            printList(cacheManager->getFifoList());

            // clear cacheManager out for the next test case
            cacheManager->clear();
        }
    }

    return 0;
}
