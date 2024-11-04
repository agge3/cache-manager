/**
 *
 * milestone3.cpp : This file contains the 'main' function. Program execution begins and ends there.
 *
 * 09/23/24 - Created by ChatGPT with prompt "write C++ program reads and parses the file: milestone3.json"
 *            The file: "milestones3.json" is in the following format:
 * 
 * 	{
 * 	    "hashTable": [
 * 	        {"testCase1": [
 * 	                {"insertAtHead": 100},
 * 	                {"insertAtTail": 10},
 * 	                {"insertAtTail": 20}
 * 	            ],
 * 	        {"testCase6": [
 * 	                {"insertAtHead": 30},
 * 	                {"insertAtTail": 40},
 * 	                {"insertAtTail": 50},
 * 	                {"insertAtTail": 60},
 * 	                {"insertAtHead": 1000},
 * 	                {"deleteTailNode": 0}
 * 	            ]
 * 	        }
 * 	    ]
 * 	}
 * 
 * @modified 2024.11.3 Tyler Baxter
 */

#define _CRT_SECURE_NO_WARNINGS

#include "hash-map.hpp"

#include <nlohmann/json.hpp>

#include <iostream>
#include <fstream>
#include <string>

using json = nlohmann::json;

namespace {
	struct Person {
		std::string fullName;
		std::string address;
		std::string city;
		std::string state;
		std::string zip;
	};

	using MapPtr = std::unique_ptr<csc::HashMap<int, int>>;
	#define DEFINE_MAP_PTR() std::make_unique<csc::HashMap<int, int>>()

	void processTestCase(MapPtr& map, const std::string& testCaseName, const json& testCaseArray) {
	    std::cout << "Processing " << testCaseName << ":\n\n";
	
	    for (size_t i = 0; i < testCaseArray.size(); ++i) {
	        const json& entry = testCaseArray[i];
	
	        for (json::const_iterator it = entry.begin(); it != entry.end(); ++it) {
	            const std::string& actionName = it.key();
	            const json& details = it.value();
	
	            if (actionName == "isEmpty") {
	                bool result = map->isEmpty();
	                std::cout << "isEmpty: " << result << std::endl;
	            }
	            else if (actionName == "contains") {
	                int key = details["key"];
	                bool result = map->contains(key);
	                std::cout << "contains(" << key << "): " << result << std::endl;
	            }
				// xxx if item (was) a HashNode containing only strings, then
				// what are we returning for getItem()? Are we returning the
				// unhashed key? Making the assumption that we should return
				// the strings.
	            else if (actionName == "getItem") {
	                int key = details["key"];
					auto optresult = map->getItem(key);
					int result = 0;
					optresult.has_value() ? result = *optresult : result = -1;
	                std::cout << "getItem(" << key << "): " << result << std::endl;
	            }
	            else if (actionName == "getNumberOfItems") {
	                int result = map->getNumberOfItems();
	                std::cout << "getNumberOfItems: " << result << std::endl;
	            }
	            else if (actionName == "add") {
					Person person{
	                	.fullName = details["fullName"],
						.address = details["address"],
						.city = details["city"],
						.state = details["state"],
						.zip = details["zip"]
					};
					// xxx But we're not looking for a person when we print the
					// HashMap, we're looking for the unhashed key. Going to
					// make value the unhashed key, under that assumption.
	                map->add(details["key"], details["key"]);
	            }
	            else if (actionName == "remove") {
	                int key = details["key"];
	                map->remove(details["key"]);
	            }
	            else if (actionName == "clear") {
	                map->clear();
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
	void printTable(const MapPtr& map) {
	    std::cout << "\nTable contents " << "(" << map->getNumberOfItems() <<
			" entries):\n\n";
		std::cout << *map;
	    std::cout << "\nEnd of table\n";
	}
}

int main() {
    // create the hash table
	MapPtr map = DEFINE_MAP_PTR();

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
	bool last = false;
    for (size_t i = 0; i < data["hashTable"].size(); ++i) {
        const json& testCase = data["hashTable"][i];
        for (json::const_iterator it = testCase.begin(); it != testCase.end(); ++it) {
            const std::string& testCaseName = it.key();
            const json& testCaseArray = it.value();
            processTestCase(map, testCaseName, testCaseArray);

            // print out the table
			if (++it != testCase.end()) {
            	printTable(map);
				// If not at end, push a newline after this test case.
				std::cout << "\n";
			} else {
				printTable(map);
			}
			--it;

            // clear hashTable out for the next test case
            map->clear();
        }
    }
  
    return 0;
}
