CXX = g++
CXXFLAGS = -std=c++20 -Wall -O2
LIBS = -lpthread

OBJ = cache-manager.cpp.o libtbb.so.12.12

# Your application
app: main.cpp $(CACHE_MANAGER_OBJ)
	$(CXX) $(CXXFLAGS) main.cpp $(OBJ) $(LIBS) -o app

clean:
	rm -f app

.PHONY: clean
