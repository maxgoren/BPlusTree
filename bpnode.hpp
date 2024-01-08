#ifndef bpnode_hpp
#define bpnode_hpp
#include <iostream>
using namespace std;

const int M = 4;

template <class K, class V>
class BPNode;

template <class K, class V>
struct BPEntry {
    K key;
    V value;
    BPNode<K,V>* child;
    BPEntry(K k = K(), V val = V(), BPNode<K,V>* c = nullptr) 
    : key(k), value(val), child(c) { }
};

template <class K, class V>
class BPNode {
    private:
        using bpentry = BPEntry<K,V>;
        bpentry page[M];
        int n;
        BPNode *next, *prev;
    public:
        BPNode(int k = 0) : n(k) { }
        ~BPNode() {        }
        int size() const {
            return n;
        }
        int isFull() {
            return !(n < M);
        }
        int floor(K key) {
            int j;
            for (j = 0; j < n; j++)
                if (j+1 == n || key < page[j+1].key)
                    break;
            return j;
        }
        int search(K key) {
            int j;
            for (j = 0; j < n; j++)
                if (key == page[j].key)
                    return j;
            return -1;
        }
        BPNode* split() {
            BPNode<K,V>* nn = new BPNode<K,V>(M/2);
            for (int i = 0; i < M/2; i++) {
                nn->page[i] = page[(M/2)+i];
            }
            n = M/2;
            nn->next = next;
            if (next) next->prev = nn;
            nn->prev = this;
            next = nn;
            return nn;
        }
        BPNode* insertAt(bpentry& entry, int j) {
            for (int i = n; i > j; i--)
                page[i] = page[i-1];
            page[j] = entry;
            n++;
            if (isFull()) return split();
            else return nullptr;
        }
        BPNode* removeAt(int j) {
            for (int i = j; i < n; i++)
                page[j] = page[j+1];
            n--;
            return this;
        }
        BPNode* insert(bpentry& entry) {
            int j = n;
            while (j > 0 && page[j-1].key >= entry.key) {
                page[j] = page[j - 1];
                j--;
            }
            page[j] = entry;
            n++;
            if (isFull()) return split();
            else return nullptr;
        }
        bpentry& at(int j) {
            return page[j];
        }
        BPNode* rightSibling() {
            return next;
        }
        BPNode* leftSibling() {
            return prev;
        }
};

#endif