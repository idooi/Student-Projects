//
// Created by 97254 on 03/12/2019.
//

#ifndef MIVNI2_NODE_H
#define MIVNI2_NODE_H
#include <cstddef>


template<class T>
class node {
private:
    T *data;
    node *next;
    node *prev;
    friend class dataCenter;
public:
    node(): data(new T()), next(nullptr), prev(nullptr){};
    ~node(){
        delete data;
    }

};

#endif //MIVNI2_NODE_H