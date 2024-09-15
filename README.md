# cache-manager
The expected outcome of cache-manager is to re-implement data structures and 
methods of the C++ STL and Java SE API. Those classes will then be used to
implement a cache and cache manager.

# Description
Iterator pattern was chosen because of the consequences listed in Design Patterns:
*
*
*

internal iterator to support find and abstract the iteration away from the client.
a robust iterator as to not invalidate iterators and pre-emptively prepare for thread-safe operations
(a cache manager can be accessed from many threads)
polymorphic vs. concrete iterator? it's fine, clients know their data structure, so concrete is preferred due to its stack allocation

# Credit
Inspirations and sources of code (not sources of implementations):
CPP STL
cppreference.com
cplusplus.com
Java SE API
docs.oracle.com
https://www.feabhas.com/sites/default/files/2016-06/Rule%20of%20the%20Big%20Five.pdf
https://aozturk.medium.com/simple-hash-map-hash-table-implementation-in-c-931965904250
https://www.geeksforgeeks.org/introduction-of-b-tree-2/
Reference for hash functions:
http://www.cse.yorku.ca/~oz/hash.html#djb2
Reference for b-tree:
https://algs4.cs.princeton.edu/code/edu/princeton/cs/algs4/BTree.java.html
https://benjamincongdon.me/blog/2021/08/17/B-Trees-More-Than-I-Thought-Id-Want-to-Know/
https://www.cs.yale.edu/homes/aspnes/pinewiki/BTrees.html
https://courses.cs.washington.edu/courses/cse373/15wi/lectures/lecture15.pdf
https://medium.com/@sarvadaman.singh/solving-cache-conundrums-a-deep-dive-into-singleton-pattern-in-action-1df52a4b088b
https://redis.io/glossary/lru-cache/
https://www.usenix.org/system/files/conference/nsdi13/nsdi13-final197.pdf
