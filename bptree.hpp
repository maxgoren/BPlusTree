#ifndef bplus_hpp
#define bplus_hpp
#include <iostream>
#include <queue>
#include "bpnode.hpp"
#include "bpiterator.hpp"
#include "orderedpair.hpp"
using namespace std;


template <class K, class V, int M>
class BPlusTree {
    private:
        bool NO_DUPLICATES;
        using node = BPNode<K,V,M>;
        using link = node*;
        using entry = Entry<K,V,M>;
        link root;
        int count;
        int height;
        entry nilInfo;
        BPIterator<K,V,M> select(link h, int rank) {
            if (h->isLeaf()) {
                return BPIterator<K,V,M>(h, rank);
            }
            int i = 0, curr_rank = rank;
            while (i < h->n) {
                if (curr_rank < h->page[i].size) 
                    break;
                curr_rank = curr_rank - h->page[i++].size;
            }
            return select(h->page[i].child, curr_rank);
        }
        int rank(link h, K key) {
            if (h->isLeaf()) {
                return h->find(key);
            }
            int j = h->floor(key), rc = 0;
            for (int i = 0; i < j; i++)
                rc +=  h->page[i].size;
            return rank(h->page[j].child, key) + rc;
        }
        int size(link h) {
            if (h->isLeaf())
                return h->n;
            int sz = 0;
            for (int i = 0; i < h->n; i++) {
                sz += size(h->page[i].child);
            }
            return sz;
        }
        void updateSubsize(link& curr, int idx) {
            curr->page[idx].size = size(curr->page[idx].child);
        }
        link merge(link a, link b) {
            for (int i = 0; i < b->n; i++) {
                a->page[a->n] = b->page[i];
                a->n++;
            }
            b->n = 0;
            a->next = b->next;
            if (b->next != nullptr) {
                b->next->prev = a;
            }
            delete b;
            return a;
        }
        link split(link h) {
            link nn = new node(M/2);
            for (int i = 0; i < M/2; i++) {
                nn->page[i] = h->page[i+(M/2)]; 
            }
            h->n = M/2;
            nn->n = M/2;
            nn->next = h->next;
            if (h->next != nullptr) {
                h->next->prev = nn;
            }
            h->next = nn;
            nn->prev = h;
            return nn;
        }
        link rebalance(link curr, K key, int j) {
            if (curr->page[j].child->n < M/2) {
                if (j+1 < curr->n && (curr->page[j].child->n + curr->page[j+1].child->n <= M)) {
                    curr->page[j].child = merge(curr->page[j].child, curr->page[j+1].child);
                    curr->eraseAt(j+1);
                } else if (j-1 >= 0 && (curr->page[j].child->n + curr->page[j-1].child->n <= M)) {
                    curr->page[j-1].child = merge(curr->page[j-1].child, curr->page[j].child);
                    curr->eraseAt(j);
                }
            }
            if (key == curr->page[j].info.key()) {
                curr->page[j] = entry(curr->page[j].child->page[0].info.key(), curr->page[j].child);
            } 
            updateSubsize(curr, j);
            return curr;
        }
        link insert(link curr, K key, V value) {
            int j = curr->find(key);
            entry nent = entry(key, value);
            if (curr->isLeaf()) {
                if (curr->page[j].info.key() == key && NO_DUPLICATES) {
                    curr->page[j].info.setValue(value);
                } else {
                    j = curr->n;
                    while (j > 0 && curr->page[j-1].info.key() > key) {
                        j--;
                    }
                    curr->insertAt(nent, j);
                    count++;
                }
            } else {
                j = curr->floor(key);
                link result = insert(curr->page[j++].child, key, value);
                updateSubsize(curr, j-1);
                if (result == nullptr) return nullptr;
                nent = entry(result->page[0].info.key(), result);
                curr->insertAt(nent, j);
                updateSubsize(curr, j);
            }
            return curr->isFull() ? split(curr):nullptr;
        }
        BPIterator<K,V,M> search(link curr, K key) {
            if (curr->isLeaf()) {
                int idx = curr->find(key);
                if (idx == -1)
                    return end();
                return BPIterator<K,V,M>(curr, idx);
            }
            int idx = curr->floor(key);
            return search(curr->page[idx].child);
        }
        link erase(link curr, K key) {
            int j = curr->find(key);
            if (curr->isLeaf() && j != -1) {
                curr->eraseAt(j);
                count--;
            } else {
                j = curr->floor(key);
                curr->page[j].child = erase(curr->page[j].child, key);
            }
            return curr->isLeaf() ? curr:rebalance(curr, key, j);
        }
        link splitRootAndGrow(link new_node) {
            link tmp = root;
            root = new node(2);
            root->page[0] = entry(tmp->page[0].info.key(), tmp);
            root->page[1] = entry(new_node->page[0].info.key(), new_node);
            updateSubsize(root, 0);
            updateSubsize(root, 1);
            height++;
            return root;
        }
        link chopRootAndShrink(link old_root) {
            link tmp = root;
            root = root->page[0].child;
            delete tmp;
            height--;
            return root;
        }
        
        void printLevel(link x) {
            for (link t = x; t != nullptr; t = t->next) {
                t->print();
                if (t->next == nullptr) break;
            }
            cout<<endl;
        }
        void cleanUp(link h) {
            if (h != nullptr) {
                for (int i = 0; i < h->n; i++)
                    cleanUp(h->page[i].child);
                delete h;
            }
        }
        link clone(link h) {

        }
    public:
        BPlusTree(bool enforce_unique = false) {
            height = 0;
            root = new node(0);
            count = 0;
            NO_DUPLICATES = enforce_unique;
        }
        ~BPlusTree() {
            cleanUp(root);
        }
        BPlusTree(const BPlusTree& bpt) {

        }
        int size() {
            return count;
        }
        bool empty() {
            return count == 0;
        }
        void insert(K key, V value) {
            link nn = insert(root, key, value);
            if (nn != nullptr) {
                root = splitRootAndGrow(nn);
            }
        }
        void erase(K key) {
            root = erase(root, key);
            if (root->n == 1 && height > 0) {
                root = chopRootAndShrink(root);
            }
        }
        BPIterator<K,V,M> select(int rank) {
            return select(root, rank);
        }
        int rank(K key) {
            return rank(root, key);
        }
        int rangeCount(K lo, K hi) {
            return rank(hi) - rank(lo);
        }
        queue<K> keysInRange(K lo, K hi) {
            queue<K> keys;
            auto itr = bpt.select(bpt.rank('h'));
            auto endIt = bpt.select(bpt.rank('s'));
            while ( itr != endIt) {
                keys.push((*itr).key());
                itr++;
            }
            return keys;
        }
        BPIterator<K,V,M> get(K key) {
            return search(root, key).info;
        }
        void bfs() {
            link x = root;
            while (x != nullptr) {
                printLevel(x);
                x = x->page[0].child;
            }
        }
        BPIterator<K,V,M> begin() {
            link x = root; int ht = height;
            while (ht > 0) {
                x = x->page[0].child;
                ht--;
            }
            return BPIterator<K,V,M>(x, 0);
        }
        BPIterator<K,V,M> end() {
            return BPIterator<K,V,M>(nullptr, 0);
        }
};

#endif