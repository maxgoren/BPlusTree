#ifndef bpiterator_hpp
#define bpiterator_hpp
#include <iostream>
#include "bpnode.hpp"
using namespace std;

template <class K, class V, int M>
class BPIterator {
    private:
        using bpnode = BPNode<K,V,M>;
        bpnode* node;
        int pos;
    public:
        BPIterator(bpnode* ptr, int idx) : node(ptr), pos(idx) { }
        KVPair<K,V> operator*() {
            return node->page[pos].info;
        }
        BPIterator operator++() {
            if (node != nullptr) {
                if (pos+1 < node->n)
                    pos++;
                else {
                    pos = 0;
                    node = node->next;
                }
            }
            return *this;
        }
        BPIterator operator--() {
            if (node != nullptr) {
                if (pos-1 < 0) {
                    node = node->prev;
                    if (node) pos = node->n-1;
                } else {
                    pos--;
                }
            }
            return *this;
        }
        BPIterator operator++(int) {
            BPIterator it = *this;
            ++*this;
            return it;
        }
        BPIterator operator--(int) {
            BPIterator it = *this;
            --*this;
            return it;
        }
        bool operator==(const BPIterator& it) const {
            return node == it.node && pos == it.pos;
        }
        bool operator!=(const BPIterator& it) const {
            return !(*this == it);
        }
};

#endif