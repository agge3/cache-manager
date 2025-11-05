CXX = g++
CXXFLAGS = -std=c++20 -Wall -O2
INCLUDES = -Iinclude
LIBS = -lpthread

OBJ = $(wildcard lib/*)

# Your application
milestone3: src/main.cpp $(OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDES) src/main.cpp $(OBJ) $(LIBS) -o milestone3

clean:
	rm -f milestone3

.PHONY: clean
