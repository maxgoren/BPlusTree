#ifndef bplus_hpp
#define bplus_hpp
#include <iostream>
#include "bpnode.hpp"
#include "bpiterator.hpp"
#include "orderedpair.hpp"
using namespace std;


template <class K, class V, int M>
class BPlusTree {
    private:
        using node = BPNode<K,V,M>;
        using link = node*;
        using entry = Entry<K,V,M>;
        link root;
        int count;
        int height;
        entry nilInfo;
        KVPair<K,V> select(link h, int rank, int ht) {
            if (ht == 0) {
                return h->page[rank].info;
            }
            int curr_rank = rank;
            for (int i = 0; i < h->n; i++) {    
                if ( curr_rank < h->subsize[i]) {
                    return select(h->page[i].child, curr_rank, ht-1);
                }
                curr_rank -= h->subsize[i];
            }
            return nilInfo.info;
        }
        int size(link h, int ht) {
            if (ht == 0)
                return h->n;
            int sz = h->n;
            for (int i = 0; i < h->n; i++) {
                sz += size(h->page[i].child, ht-1);
            }
            return sz;
        }
        link merge(link a, link b) {
            for (int i = 0; i < b->n; i++) {
                a->page[a->n] = b->page[i];
                a->subsize[a->n] = b->subsize[i];
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
                nn->subsize[i] = h->subsize[i+(M/2)];
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
            return curr;
        }
        link insert(link curr, K key, V value, int ht) {
            int j;
            entry nent = entry(key, value);
            if (ht == 0) {
                j = curr->n;
                while (j > 0 && curr->page[j-1].info.key() > key) {
                    j--;
                }
            } else {
                j = curr->floor(key);
                link result = insert(curr->page[j++].child, key, value, ht-1);
                if (ht > 0)
                    curr->subsize[j-1] = size(curr->page[j-1].child, ht-1);
                if (result == nullptr) return nullptr;
                nent = entry(result->page[0].info.key(), result);
            }
            curr = curr->insertAt(nent, j);
            return curr->isFull() ? split(curr):nullptr;
            
        }
        entry& search(link curr, K key, int ht) {
            if (ht == 0) {
                int idx = curr->find(key);
                if (idx == -1)
                    return nilInfo;
                return curr->page[idx];
            }
            int idx = curr->floor(key);
            return search(curr->page[idx].child, key, ht-1);
        }
        link erase(link curr, K key, int ht) {
            int j = curr->find(key);
            if (ht == 0 && j != -1) {
                curr->eraseAt(j);
                count--;
            } else {
                j = curr->floor(key);
                curr->page[j].child = erase(curr->page[j].child, key, ht-1);
            }
            return ht == 0 ? curr:rebalance(curr, key, j);
        }
        link splitRootAndGrow(link new_node) {
            link tmp = root;
            root = new node(2);
            root->page[0] = entry(tmp->page[0].info.key(), tmp);
            root->page[1] = entry(new_node->page[0].info.key(), new_node);
            root->subsize[0] = size(tmp, height) + 1;
            root->subsize[1] = size(new_node, height) + 1;
            height++;
            return root;
        }
        link chopRootAndShrink(link old_root) {
            link tmp = root;
            root = root->page[0].child;
            delete tmp;
            height--;
            //cout<<"Chopping the head off."<<endl;
            return root;
        }
    public:
        BPlusTree() {
            height = 0;
            root = new node(0);
            count = 0;
        }
        int size() {
            return count;
        }
        void insert(K key, V value) {
            count++;
            link nn = insert(root, key, value, height);
            if (nn != nullptr) {
                root = splitRootAndGrow(nn);
            }
        }
        void erase(K key) {
            root = erase(root, key, height);
            if (root->n == 1 && height > 0) {
                root = chopRootAndShrink(root);
            }
        }
        KVPair<K,V> select(int rank) {
            return select(root, rank, height);
        }
        KVPair<K,V>& get(K key) {
            return search(root, key, height).info;
        }
        void bfs() {
            link x = root;
            while (x != nullptr) {
                link t;
                for (t = x; t != nullptr; t = t->next) {
                    cout<<"[ ";
                    for (int i = 0; i < t->n; i++) {
                        cout<<t->page[i].info.key()<<" "; //"("<<t->subsize[i]<<") ";
                    }
                    cout<<"] ";
                    if (t->next == nullptr) break;
                }
                cout<<endl;
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