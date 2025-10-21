# Cache Manager
Concurrent LRU cache with `O(logn)` sorted index. Single-threaded
implementation with initial design documentation is included in `single`.

---

## Description
The expected outcomes of cache-manager are to reimplement data structures and 
methods of the C++ STL and Java SE API. Those "from scratch" implementations 
will be used to implement a ground-up database LRU cache with `O(logn)` sorted
index.

---

## Tech Stack

- **Language:** C++20  
- **Concurrency:** [Intel Threading Building Blocks (TBB)](https://github.com/oneapi-src/oneTBB)  
- **Testing:** GoogleTest / GoogleMock / Custom 
- **Containerization:** Podman  
- **Build System:** CMake (3.14–3.28)

---

## Key Components

Concurrent cache-manager:
- **LRU Cache Core:**  
  Implements least-recently-used eviction policy with atomic synchronization and TBB concurrent data structures.

Single-threaded cache-manager implements hand-rolled data structures (hashmap, SLL, DLL)

---

## Dependencies

| Dependency | Minimum Version | Notes |
|-------------|-----------------|-------|
| Podman | Latest stable | For containerized build & test |
| C++ Compiler | C++20-compliant | GCC ≥ 10.2 or Clang ≥ 12 recommended |
| CMake | 3.14 – 3.28 | For project configuration |
| Intel TBB | — | Used for concurrent containers |
| GTest / GMock | — | Unit testing framework |

---

## Organization
On branch `concurrency`

`include`
 * `cache-manager.hpp` - CacheManager template header and implementation.
 * `test-runner.hpp` - Handrolled threaded test runner.
 * `macros.hpp` - Utility macros.
    `list` 
        * `concurrent-list.hpp` - ConcurrentList interface and specializations.
        * `concurrent-list-impl.hpp` - ConcurrentList interface concrete
                                implementation.
        * `coarse-concurrent-list-impl.hpp` - CoarseConcurrentList implementation. (WARNING: implementation has races).
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

---

## Running
Run script assumes `podman`. See `run-podman.sh` for specific `Docker` run
configuration (NOTE: there's no specifics).
```bash
./run-podman.sh
```

---

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

---

## Credit 

**Inspirations**
* CPP STL
* cppreference.com
* cplusplus.com
* Java SE API - docs.oracle.com
* https://www.feabhas.com/sites/default/files/2016-06/Rule%20of%20the%20Big%20Five.pdf
* https://aozturk.medium.com/simple-hash-map-hash-table-implementation-in-c-931965904250
* https://www.geeksforgeeks.org/introduction-of-b-tree-2/ **Reference for hash functions** * http://www.cse.yorku.ca/~oz/hash.html#djb2

**Reference for LRU cache:** 
* https://medium.com/@sarvadaman.singh/solving-cache-conundrums-a-deep-dive-into-singleton-pattern-in-action-1df52a4b088b
* https://redis.io/glossary/lru-cache/
* https://www.usenix.org/system/files/conference/nsdi13/nsdi13-final197.pdf
* https://priorart.ip.com/IPCOM/000196714/High-Performance-Cache-With-LRU-Replacement-Policy

---

Authors

[@agge3](github.com/agge3)  
[@kpowkitty](github.com/kpowkitty)

