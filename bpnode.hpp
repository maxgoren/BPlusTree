#ifndef bpnode_hpp
#define bpnode_hpp
#include <iostream>
#include "orderedpair.hpp"
using namespace std;


template <class K, typename V, int M>
struct BPNode;

template <class K, typename V, int M>
struct Entry {
    KVPair<K,V> info;
    BPNode<K,V,M>* child;
    int size;
    Entry(K key, V value) : info(key, value), size(0), child(nullptr) { }
    Entry(K key, BPNode<K,V,M>* n) : info(key, V()), size(0), child(n) { }
    Entry() : size(0), child(nullptr) { }
};

template <class K, class V, int M>
struct BPNode {
    Entry<K,V,M> page[M];
    int n;
    BPNode* next;
    BPNode* prev;
    BPNode(int m) : n(m), next(nullptr), prev(nullptr) {

    }
    bool isFull() const {
        return n == M;
    }
    bool isEmpty() const {
        return n == 0;
    }
    bool isLeaf() {
        return page[0].child == nullptr;
    }
    int size() const {
        return n;
    }
    BPNode* insertAt(Entry<K,V,M>& entry, int index) {
        for (int j = n; j > index; j--) {
            page[j] = page[j-1];
        }
        page[index] = entry;
        n++;
        return this;
    }
    void eraseAt(int idx) {
        for (int j = idx; j < n-1; j++) {
            page[j] = page[j+1];
        }
        n--;
    }
    int floor(K key) {
        int i;
        for (i = 0; i < n; i++) {
            if (i+1 == n || key < page[i+1].info.key())
                break;
        }
        return i;
    }
    int find(K key) {
        int i = 0;
        while (i < n) {
            if (key == page[i].info.key())
                return i;
            i++;
        }
        return -1;
    }
    void print() {
        cout<<"[ ";
        for (int i = 0; i < n; i++) {
            cout<<page[i].info.key()<<" ("<<page[i].size<<") ";
        }
        cout<<"] ";
    }
};

#endif