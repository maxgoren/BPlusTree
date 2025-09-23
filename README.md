# B+ Tree w/ Order Statistics
An in-memory B+ Tree Symbol Table in C++

Maintains perfectly balanced m-way search tree with all leaf nodes at the same level.
Internal nodes are an Index containing keys which guide the search to the data-storing leaf nodes
which form a doubly linked list of nodes for fast in-order traversal.

Both insertion and deletion are performed bottom-up.

## API

### Mutating Operations
    insert(K key, V value) - Add a Value, Indexed by the provided Key to the tree.
    erase(K key)           - Remove the value indexed by key

### Search Operations
    get(K key)             - Returns an Iterator to the entry indexed by key, if it exists. Returns end() on search failure.
    select(int rank)       - Returns an Iterator to the ith element in sorted order from the entry set.
    rank(K key)            - Returns the rank of supplied key
    keysInRange(K hi, K lo) - Returns a Queue containing the keys which fall within the supplied range.
    
### Population Queries
    size()                 - Returns the number of entries in the tree
    empty()                - Determines if tree is empty.
    
### Iteration
    begin()                - Returns an iterator to the first in-order element
    end()                  - Returns an iterator to the last in-order element + 1.
