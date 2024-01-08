#ifndef bptree_hpp
#define bptree_hpp
#include <iostream>
#include <queue>
#include "bpnode.hpp"
#include "bpiterator.hpp"
using namespace std;

template <class K, class V>
class BPTree {
    private:
        using bpnode = BPNode<K,V>;
        using bpentry = BPEntry<K,V>;
        using iterator = BPIterator<K,V>;
        bpnode* root;
        int count;
        int height;
        K nullKey;
        V nullItem;
        bpnode* insert(bpnode* node, K key, V value, int ht) {
            int i, j;
            bpentry nent = bpentry(key, value, nullptr);
            if (ht == 0) {
                return node->insert(nent);
            } else {
                j = node->floor(key);
                bpnode* result = insert(node->at(j++).child, key, value, ht-1);
                if (result == nullptr) return nullptr;
                nent = bpentry(result->at(0).key, nullItem, result);
            }
            return node->insertAt(nent, j);
        }
        iterator search(bpnode* node, K key, int ht) {
            int i, j;
            if (ht == 0) {
                j = node->search(key);
                if (j > -1) return iterator(node, j);
                else return end();
            }
            return search(node->at(node->floor(key)).child, key, ht-1);
        }
        bpnode* erase(bpnode* node, K key, int ht) {
            int i, j;
            if (ht == 0) {
                j = node->search(key);
                if (j > -1) {
                    node->removeAt(j);
                    count--;
                }
            } else {
                j = node->floor(key);
                node->at(j).child = erase(node->at(j).child, key, ht-1);
            }
            return node;
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
        void split_root(bpnode* new_node) {
            bpnode* oldroot = root;
            root = new bpnode(2);
            root->at(0) = bpentry(oldroot->at(0).key, nullItem, oldroot);
            root->at(1) = bpentry(new_node->at(0).key, nullItem, new_node);
            height++;
        }
        void cleanup(bpnode* node) {
            if (node != nullptr) {
                for (int i = 0; i < node->size(); i++)
                    cleanup(node->at(i).child);
                delete node;
            }
        }
    public:
        BPTree() {
            height = 0;
            count = 0;
            root = new bpnode(0);
        }
        BPTree(const BPTree& tree) {
            height = 0;
            count = 0;
            root = new bpnode(0);
            for (auto p : tree)
                insert(p.first, p.second);
        }
        ~BPTree() {
            cleanup(root);
        }
        void erase(K key) {
            root = erase(root, key, height);
        }
        void insert(K key, V value) {
            bpnode* tmp = insert(root, key, value, height);
            if (tmp != nullptr) {
                split_root(tmp);
            }
            count++;
        }
        int size() { return count; }
        int depth() { return height; }
        iterator find(K key) {
            return search(root, key, height);
        }
        void levelorder(int method) {
            int ht = height;
            bpnode* x = root;
            while (ht >= 0) {
                cout<<"[ ";
                for (int g = 0; g < x->size(); g++) cout<<"   ";
                for(bpnode* it = x; it != nullptr; it = it->rightSibling()) {
                    cout<<"(";
                    if (method == 0) {
                        for (int i = 0; i < it->size()-1; i++) {
                            cout<<it->at(i).key<<" | ";
                        }
                        cout<<it->at(it->size()-1).key<<") ";
                        
                    } else {
                        double pct = (double)it->size()/(double)M;
                        int usg = pct * 100;
                        cout<<usg<<"%) ";
                    }
                }
                cout<<"]"<<endl;
                x = x->at(0).child;
                ht--;
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
        BPTree& operator=(const BPTree& tree) {
            height = 0;
            count = 0;
            root = new bpnode(0);
            for (auto p : tree)
                insert(p.first, p.second);
            return *this;
        }
};


#endif