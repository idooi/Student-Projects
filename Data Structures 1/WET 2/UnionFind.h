//
// Created by IdoSarel on 12/31/19.
//

#ifndef WET2_UNIONFIND_H
#define WET2_UNIONFIND_H
using namespace std;
#include "Node.h"
#include <iostream>

template <class T>
class UnionFind {
    Node<T>** elements;
    int maxElements;
 //   int *freeIndex;
public:
    UnionFind(int maxElements):
    elements(new Node<T>* [maxElements]), maxElements(maxElements){
            init();
    }
    ~UnionFind(){
        Node<T> *node;
        for (int i = 0; i< maxElements; i++){
            node = elements[i];
            delete node;
        }
        delete[] elements;
    }
    void init(){
        T* data;
        for (int i = 0; i < maxElements; i++){
            data = new T();
            makeSet(data, i);
        }
    }
    Node<T>* makeSet(T* element, int index){
        Node<T> * newSet = new Node<T>(element);
        elements[index] = newSet;
        return newSet;
    }
    Node<T>* find(int i){
        Node<T> * treeNode = elements[i];
        while (treeNode->up){//find header
            treeNode = treeNode->up;
        }
        Node<T>* header = treeNode;
        if (!header->data) cout<<"no header data"<< endl;
        treeNode = elements[i];
        Node<T>* temp = treeNode;
        while (treeNode != header){//shorten routes
            temp = treeNode->up;
            treeNode->up = header;
            treeNode = temp;
        }
        return header;
    }
    Node<T>* unionGroups(Node<T>* header1, Node<T>* header2) {
        if (header1 == header2) {//same group
            return header1;
        }
        Node<T>* newHeader = (header1->size > header2->size ? header1 : header2);
        Node<T>* oldHeader = (newHeader == header1 ? header2 : header1);
        T * newData = (*(newHeader->data) + *(oldHeader->data));
        delete oldHeader->data;
        delete newHeader->data;
        oldHeader->data = nullptr;
        newHeader->data = newData;
        oldHeader->up = newHeader;
        newHeader->size += oldHeader->size;
        oldHeader->size = 1;
        return newHeader;
    }
};




#endif //WET2_UNIONFIND_H

