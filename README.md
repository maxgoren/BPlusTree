# BPlusTree
An in-memory B+ Tree in C++

Maintains perfectly balanced m-way search tree with all leaf nodes at the same level.
Internal nodes are an Index containing keys which guide the search to the data-storing leaf nodes
which form a doubly linked list of nodes for fast in-order traversal.

Both insertion and deletion are performed bottom-up.

## API

### Mutating Operations
    insert(K key, V value) - Add a Value, Indexed by the provided Key to the tree.
    erase(K key)           - Remove the value indexed by key

### Search Operations
    get(K key)             - Return the entry indexed by key, if it exists. Returns empty KVPair on search failure.
    select(int rank)       - Returns the ith element in sorted order from the entry set.

### Population Queries
    size()                 - Returns the number of entries in the tree
    empty()                - Determines if tree is empty.
    
### Iteration
    begin()                - Returns an iterator to the first in-order element
    end()                  - Returns an iterator to the last in-order element + 1.
