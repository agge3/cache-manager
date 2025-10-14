# cache-manager
Database LRU cache with B-tree indexes.

## Description
The expected outcomes of cache-manager are to reimplement data structures and 
methods of the C++ STL and Java SE API. Those "from scratch" implementations 
will be used to implement a ground-up database LRU cache with B-tree indexes.

## Building
Build uses CMake.  
#### Linux
```bash
mkdir build
cd build
cmake ../
make all

# to run test suite
./cache-manager
```
#### Visual Studio
Install CMake plugin.

## Organization
`include`
 * `cache-manager.hpp` - CacheManager template header and implementation.
 * `concurrent-list.hpp` - ConcurrentList interface and specializations.
 * `concurrent-list-impl.hpp` - ConcurrentList interface concrete
                                implementation.
 * `coarse-concurrent-list-impl.hpp` - CoarseConcurrentList implementation.
 * `fine-concurrent-list-impl.hpp` - FineConcurrentList implementation (WARNING:
                                     has races).

`src`
 * `main.cpp` - Main driver and test runner.

`single` - Single-threaded CacheManager.
 * `include`
   * `singly-linked-list.hpp` - SinglyLinkedList share header.
   * `doubly-linked-list.hpp` - DoublyLinkedList share header.
   * `hash-map.hpp` - HashMap share header.
   * `cache-manager.hpp` - CacheManager share header.
   * `test` - Hand-rolled test suite (not GTest).
   * `impl` - Template implementations.
     * `singly-linked-list-impl.hpp` - SinglyLinkedList template implementation.
     * `doubly-linked-list-impl.hpp` - DoublyLinkedList template implementation.
     * `hash-map.hpp-impl` - HashMap template implementation.
     * `cache-manager-impl.hpp` - CacheManager template implementation.

 * `src`
   * `main.cpp` - Main driver with GTest test cases, to run CacheManager and
                all test suites.
   * `test.cpp` - Hand-rolled test suite (not GTest) implementation.

 * `doc` - Class and sequence diagrams, and Doxygen build target.  

 * `config` - Project configuration files (e.g., Doxyfile).  

 * `tools` - Utility tools and scripts (bash).  

 * `external` - External libraries and resources.
   * `lib`
   * `milestoneX` - Milestone JSON configuration (for main driver test cases).  

## TODO
 * B-tree unimplemented

## Credit
**Inspirations**
 * CPP STL
 * cppreference.com
 * cplusplus.com
 * Java SE API - docs.oracle.com
 * https://www.feabhas.com/sites/default/files/2016-06/Rule%20of%20the%20Big%20Five.pdf
 * https://aozturk.medium.com/simple-hash-map-hash-table-implementation-in-c-931965904250
 * https://www.geeksforgeeks.org/introduction-of-b-tree-2/

**Reference for hash functions**
 * http://www.cse.yorku.ca/~oz/hash.html#djb2

**Reference for b-tree:**
 * https://algs4.cs.princeton.edu/code/edu/princeton/cs/algs4/BTree.java.html
 * https://benjamincongdon.me/blog/2021/08/17/B-Trees-More-Than-I-Thought-Id-Want-to-Know/
 * https://www.cs.yale.edu/homes/aspnes/pinewiki/BTrees.html
 * https://courses.cs.washington.edu/courses/cse373/15wi/lectures/lecture15.pdf

**Reference for LRU cache:**
 * https://medium.com/@sarvadaman.singh/solving-cache-conundrums-a-deep-dive-into-singleton-pattern-in-action-1df52a4b088b
 * https://redis.io/glossary/lru-cache/
 * https://www.usenix.org/system/files/conference/nsdi13/nsdi13-final197.pdf
 * https://priorart.ip.com/IPCOM/000196714/High-Performance-Cache-With-LRU-Replacement-Policy
