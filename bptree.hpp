#ifndef bptree_hpp
#define bptree_hpp
#include <iostream>
#include <queue>
#include "bpnode.hpp"
#include "bpiterator.hpp"
using std::cout;
using std::endl;
using std::queue;

template <class K, class V>
class BPTree {
    private:
        using bpnode = BPNode<K,V>;
        using bpentry = BPEntry<K,V>;
        using iterator = BPIterator<K,V>;
        bpnode* root;
        int height;
        K nullKey;
        V nullItem;
        bpnode* insert(bpnode* node, K key, V value, int ht) {
            int i, j;
            bpentry nent = bpentry(key, value, nullptr);
            if (ht == 0) {
                j = node->floor(key);
            } else {
                for (j = 0; j < node->size(); j++) {
                    if (j+1 == node->size() || key < node->at(j+1).key) {
                        bpnode* result = insert(node->at(j++).child, key, value, ht-1);
                        if (result == nullptr) {
                            return nullptr;
                        } else {
                            nent = bpentry(result->at(0).key, nullItem, result);
                            break;
                        }
                    }
                }
            }
            return node->insertAt(nent, j);
        }
        bpnode* min(bpnode* node, int ht) {
            if (ht == 0)
                return node;
            return min(node->at(0).child, ht-1);
        }
        bpnode* min(bpnode* node, int ht) const {
            if (ht == 0)
                return node;
            return min(node->at(0).child, ht-1);
        }
    public:
        BPTree() {
            height = 0;
            root = new bpnode(0);
        }
        void insert(K key, V value) {
            bpnode* tmp = insert(root, key, value, height);
            if (tmp != nullptr) {
                bpnode* oldroot = root;
                root = new bpnode(2);
                root->at(0) = bpentry(oldroot->at(0).key, nullItem, oldroot);
                root->at(1) = bpentry(tmp->at(0).key, nullItem, tmp);
                height++;
            }
        }
        void sorted() {
            for(bpnode* it = min(root, height); it != nullptr; it = it->rightSibling()) {
                for (int i = 0; i < it->size(); i++) {
                    cout<<it->at(i).key<<" ";
                }
            }
            cout<<endl;
        }
        void levelorder() {
            queue<bpnode*> fq;
            fq.push(root);
            while (!fq.empty()) {
                int nc = fq.size();
                cout<<"[";
                while (nc > 0) {
                    bpnode* curr = fq.front();
                    fq.pop();
                    nc--;
                    if (curr != nullptr) {
                        cout<<"(";
                        int i;
                        for (i = 0; i < curr->size()-1; i++) {
                            cout<<curr->at(i).key<<", ";
                            fq.push(curr->at(i).child);
                        }
                        cout<<curr->at(i).key;
                        fq.push(curr->at(i).child);
                        cout<<")";
                    }
                }
                cout<<"]"<<endl;
            }
        }
        iterator begin() {
            return iterator(min(root, height), 0);
        }
        iterator begin() const {
            return iterator(min(root, height), 0);
        }
        iterator end() {
            return iterator(nullptr, 0);
        }
        iterator end() const {
            return iterator(nullptr, 0);
        }
};


#endif