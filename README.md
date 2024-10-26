# cache-manager
Database LRU cache with B-tree indexes

# Description
The expected outcomes of cache-manager are to reimplement data structures and 
methods of the C++ STL and Java SE API. Those "from scratch" implementations 
will be used to implement a ground-up database LRU cache with B-tree indexes.

# Discussion (WIP)
Iterator pattern was chosen because of the consequences listed in Design Patterns:
*
*
*
internal iterator to support find and abstract the iteration away from the client.
a robust iterator as to not invalidate iterators and pre-emptively prepare for thread-safe operations
(a cache manager can be accessed from many threads)
polymorphic vs. concrete iterator? it's fine, clients know their data structure, so concrete is preferred due to its stack allocation

# HashMap Description

# Credit
Inspirations:
<br>
CPP STL
<br>
cppreference.com
<br>
cplusplus.com
<br>
Java SE API
<br>
docs.oracle.com
<br>
https://www.feabhas.com/sites/default/files/2016-06/Rule%20of%20the%20Big%20Five.pdf
<br>
https://aozturk.medium.com/simple-hash-map-hash-table-implementation-in-c-931965904250
<br>
https://www.geeksforgeeks.org/introduction-of-b-tree-2/
<br>
Reference for hash functions:
<br>
http://www.cse.yorku.ca/~oz/hash.html#djb2
<br>
Reference for b-tree:
<br>
https://algs4.cs.princeton.edu/code/edu/princeton/cs/algs4/BTree.java.html
<br>
https://benjamincongdon.me/blog/2021/08/17/B-Trees-More-Than-I-Thought-Id-Want-to-Know/
<br>
https://www.cs.yale.edu/homes/aspnes/pinewiki/BTrees.html
<br>
https://courses.cs.washington.edu/courses/cse373/15wi/lectures/lecture15.pdf
<br>
Reference for LRU cache:
<br>
https://medium.com/@sarvadaman.singh/solving-cache-conundrums-a-deep-dive-into-singleton-pattern-in-action-1df52a4b088b
<br>
https://redis.io/glossary/lru-cache/
<br>
https://www.usenix.org/system/files/conference/nsdi13/nsdi13-final197.pdf
<br>
https://priorart.ip.com/IPCOM/000196714/High-Performance-Cache-With-LRU-Replacement-Policy
