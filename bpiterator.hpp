#ifndef bpiterator_hpp
#define bpiterator_hpp
#include <iostream>
#include "bpnode.hpp"
using namespace std;

template <class K, class V>
class BPIterator {
    private:
        using bpnode = BPNode<K,V>;
        bpnode* node;
        int pos;
    public:
        BPIterator(bpnode* ptr, int idx) : node(ptr), pos(idx) { }
        pair<K,V> operator*() {
            return make_pair(node->at(pos).key, node->at(pos).value);
        }
        BPIterator operator++() {
            if (node != nullptr) {
                if (pos+1 < node->size())
                    pos++;
                else {
                    pos = 0;
                    node = node->rightSibling();
                }
            }
            return *this;
        }
        BPIterator operator--() {
            if (node != nullptr) {
                if (pos-1 < 0) {
                    node = node->leftSibling();
                    if (node) pos = node->size()-1;
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