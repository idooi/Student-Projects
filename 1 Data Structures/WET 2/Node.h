//
// Created by IdoSarel on 12/31/19.
//

#ifndef WET2_NODE_H
#define WET2_NODE_H

/*template <class K>
class Header{
    K* data;
    Header(K* data): data(data){}
    ~Header()= default;
    template <class T2, class K2>
            friend class Node;

};*/

template<class T/*,class K*/>
class Node {
    T* data;
    Node * up;
    //Header<K>* header;
    int size;
    Node(T* data): data(data), up(nullptr), size(1)/*, header(nullptr)*/{}
    ~Node(){
        delete data;
    }
    template <class T2>
    friend class UnionFind;
public:
    T* getData(){
        return data;
    }
};



#endif //WET2_NODE_H
