//
// Created by IdoSarel on 11/29/19.
//
#ifndef DATASTRUCTURES1_TREE_H
#define DATASTRUCTURES1_TREE_H
#define LEFT  0
#define RIGHT 1

#include <iostream>

using namespace std;

template <class T, class K>
        class Tree{
private :
    K*       _key;
    T*       _data;
    int   _height;
    Tree* _father;
    Tree* _left;
    Tree* _right;
    template <class T2, class K2>
    friend class Root;
public:
    Tree(K* key, T* data, Tree* father):
        _key(key), _data(data), _height(1),
        _father(father), _left(nullptr), _right(nullptr)
    {}

    ~Tree(){
        delete _key;
        delete _data;
    };

    T*   getData()  const {
        return _data;
    }

    K* getKey() const{
        return _key;
    }

    void setData(T* newData){
        _data = newData;
    }

    void insertElement(Tree* element){
        if (*_key > *(element->_key)){//the root is greater -> go left
            if (!_left) {//add as left
                return addChild(element, LEFT);
            }
            return _left ->insertElement(element);
        } else { // assuming never equals -> go right
            if (!_right) {//add as right
                return addChild(element, RIGHT);
            }
            return _right->insertElement(element);
        }
    }
    int getMax(int a, int b){
        return ( a < b ? b : a);
    }
    int getHeight(){
        int left  = (_left  ? _left  ->_height : 0);
        int right = (_right ? _right ->_height : 0);
        return  getMax(left ,right) + 1;
    }

    void maintainHeight(){
        if (!this) return;
        _height = getHeight();
        _father->maintainHeight();
    }

    void addChild (Tree* element, int dir){
        if (dir == LEFT){
            _left = element;
        }
        else {//There's so many directions I can't choose...
            _right = element;
        }
        element->_father = this;
        maintainHeight();
        fixTree();
    }

    int getBF(){
        if (!this) return 0;
        int left   = (_left  ? _left ->_height : 0 );
        int right  = (_right ? _right->_height : 0 );
        return left - right;
    }

    void fixTree(){
        if (!this) return;
        _height = getHeight();
        int BF = getBF();
        if  (abs(BF) <= 1){//skip!! go upper
            if(_father){
                _father->fixTree();
            }
            return;
        }
        //now abs(BF) = 2
        int lBF = _left ->getBF();
        int rBF = _right->getBF();

        //start fixing
        if ( BF == 2){
            if (lBF >= 0){//LL
                LL();
            }   else {//assuming lBF = -1 -> LR
                _left->RR();
                LL();

            }

        } else {// assuming BF = -2
            if (rBF <= 0) {//RR
                RR();
            } else {//assuming rBF = 1 -> RL
                _right->LL();
                RR();
            }
        }
        _height = getHeight();
       fixTree();
    }

    void  LL(){
        Tree * b    = this;//
        Tree * a    = b->_left;
        Tree * aR   = a->_right;
        //start moving
        b ->_left   = aR;
        if (aR) {
            aR->_father = b ;
        }
        ////
        a->_right  = b;
        b->rotateRootAndHeightHandling(a);
    }

    void  RR(){
        Tree * b    = this;//
        Tree * a    = b->_right;
        Tree * aL   = a->_left;
        //start moving
        b ->_right   = aL;
        if (aL){
            aL->_father = b ;
        }
        ////
        a->_left  = b;
        b->rotateRootAndHeightHandling(a);
    }

    void  rotateRootAndHeightHandling(Tree *a){
        Tree * b    = this;
        Tree * root = b->_father;
        if (root){// handle the upper root if not null - a is the new child of root
            if (root->_right == b){
                root->_right = a;
            }else {//root->_left == b
                root->_left = a;
            }
        }
        b->_father = a;
        a->_father = root;
        b->_height = b->getHeight();
        a->_height = a->getHeight();
    }

    T* findElement(K* key) const{
        if (*_key > *key){
            return (_left  ? _left ->findElement(key) : nullptr);
        }
        if (*_key < *key) {
            return (_right ? _right->findElement(key) : nullptr);
        }//must be equal
        return _data;

    }

    Tree * detachPrep(K* key) {
        if (*_key > *key) {
            return (_left  ? _left ->detachPrep(key) : nullptr);
        }
        if (*key > *_key) {
            return (_right ? _right->detachPrep(key) : nullptr);
        }
        // assuming *_key == *key
        if (!_left && !_right) {// a leaf
            return this;
        }
        if (_left && _right) {
            //find the element with the closet next key value and swap
            //that means go right once and go left as much as possible
            Tree<T, K> *tr = _right;
            while (tr->_left) {
                tr = tr->_left;
            }
            //now tr is either a leaf or has one child
            K* tempKey  = this->_key ;
            T* tempData = this->_data;

            this->_key = tr->_key;
            tr->_key = tempKey;

            this->_data = tr->_data;
            tr-> _data = tempData;
            if (!tr->_left && !tr->_right){//tr is a leaf -> detach
                return tr;
            }
            //tr has one child
            Tree *trChild = (tr->_left ? tr->_left : tr->_right);
            //tr has a father
            tr->_father->_left  = (tr->_father-> _left == tr ? trChild : tr->_father->_left );
            tr->_father->_right = (tr->_father->_right == tr ? trChild : tr->_father->_right);
            trChild->_father = tr->_father;
            // now tr can be detached
            return tr;
        }
        //now there's only one child
        //just lift the child
        Tree<T, K> * child = (_left ? _left : _right);
        if (_father) {//the detached wasn't the root
            _father->_left  = (_father-> _left == this ? child : _father-> _left);
            _father->_right = (_father->_right == this ? child : _father->_right);
            child->_father  = _father;
        }
        else{//no father -> child is the new root
            child->_father = nullptr;
        }

        return this;
    }

    int getLength() const{
        if (!this) return 0;
        return _left->getLength() + _right ->getLength() + 1 ;
    }

    Tree* getMax() const{
        if (_right) return _right->getMax();
        return this;
    }

    void getDataDesc(T** dataArr, int* i){
     if (_right) _right->getDataDesc(dataArr, i);
     dataArr[*i] = _data;
      *i = *i + 1;
     if (_left ) _left ->getDataDesc(dataArr, i);
    }
};

//a framing class to hold the tree and handle non - recursive functions
//as well as an interface
template <class T, class K>
class Root{
private:
    Tree<T, K> * _root;
public:
    Root():
    _root(nullptr){}
    ~Root(){
        deleteTree(_root);
    }
    void insert(K* key, T* data){
        Tree<T, K> * element  = new Tree<T,K>(key, data, nullptr);
        insertTree(element);

    }

    void insertTree(Tree<T, K>* element){
        if (!_root){
            _root = element;
            return;
        }
        _root->insertElement(element);//adds the element and
        setRoot();
    }

    Tree<T, K> * detach(K* key){
        Tree<T, K> * detached  = _root->detachPrep(key);
        Tree<T, K> * detachedFather = detached->_father;
        if (!detachedFather){//new root
            _root = (detached->_left ? detached->_left : detached->_right);
            _root->maintainHeight();
            _root->fixTree();
        }
        else{
        detachedFather->_left  = (detachedFather-> _left  == detached ? nullptr : detachedFather-> _left );
        detachedFather->_right = (detachedFather-> _right == detached ? nullptr : detachedFather-> _right);
        detachedFather->maintainHeight();
        detachedFather->fixTree();
        }
        detached->_height = 1      ;
        detached->_father = nullptr;
        detached->_left   = nullptr;
        detached->_right  = nullptr;
        if (_root) {
            setRoot();
        }
        return detached;
    }

    void setRoot(){
        if(_root->_father){
            _root =_root->_father;
            setRoot();
        }
    }

    int getLength() const{
        return _root->getLength();
    }

    T* findData(K * key) const{
        if (!_root) return nullptr;
        return _root->findElement(key);
    }

    bool exists(K* key){
        if (!_root) return false;
        return (_root->findElement(key));
    }

    void deleteTree(Tree<T, K>* tr){
        if (!this) return;
        if (!tr) return;
        deleteTree(tr-> _left);
        deleteTree(tr->_right);
        delete tr;
    }
    void detachAllData(Tree<T, K>* tr){
        if (!this) return;
        if (!tr) return;
        detachAllData(tr-> _left);
        detachAllData(tr->_right);
        tr->_data = nullptr;
    }

    T** getDataArrayDesc() const{
        int treeLength = getLength();
        T **dataArr =  (T**)malloc(treeLength * sizeof(T *));
        int * i = new int();
        *i = 0;
         _root->getDataDesc(dataArr, i);
         delete i;
         return dataArr;
    }
    void detachAllRootData(){
        detachAllData(_root);
    }
};
#endif //DATASTRUCTURES1_TREE_H
