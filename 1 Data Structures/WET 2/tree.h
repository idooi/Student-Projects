//
// Created by IdoSarel on 01/01/20.
//

#ifndef WET2_RANKTREE_H
#define WET2_RANKTREE_H

#define LEFT  0
#define RIGHT 1

#include <iostream>

using namespace std;

enum epart{
    KEY,DATA
};

template <class T, class K>
class Tree{
private :
    K*       _key ;
    T*       _data;
    int   _height ;
    Tree* _father ;
    Tree* _left   ;
    Tree* _right  ;
    int   _rank   ;
    T*    _dataSum;
    template <class T2, class K2>
    friend class Root;
public:
    Tree(K* key, T* data, Tree* father):
            _key(key), _data(data), _height(0),
            _father(father), _left(nullptr), _right(nullptr), _rank(1), _dataSum(new T())
    {
        *_dataSum = *_data;
    }
    Tree(Tree* father):
            _key(nullptr), _data(nullptr), _height(0),
            _father(father), _left(nullptr), _right(nullptr), _rank(1), _dataSum(nullptr){}


    ~Tree(){
        delete _key;
        delete _data;
        delete _dataSum;
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

    void setKey(K* newKey){
        _key = newKey;
    }

    T* getDataSum() const {
        return _dataSum;
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

    int getHeight(){
        int left  = (_left  ? _left  ->_height : -1);
        int right = (_right ? _right ->_height : -1);
        return  max(left ,right) + 1;
    }
    int getRank(){
        int left  = (_left  ? _left  ->_rank : 0);
        int right = (_right ? _right ->_rank : 0);
        return  left + right + 1;
    }
    T getSum(){
        if (!_data){
            return 0;
        }
        T sum = *_data;
        if (_left){
            sum += *_left->_dataSum;
        }
        if (_right){
            sum += *_right->_dataSum;
        }
        return sum;
    }
    void maintainMembers(){
        _height   = getHeight();
        _rank     = getRank()  ;
        if (_dataSum) {
            *_dataSum = getSum() ;
        }
    }

    void addChild (Tree* element, int dir){
        if (dir == LEFT){
            _left = element;
        }
        else {//There's so many directions I can't choose...
            _right = element;
        }
        element->_father = this;
        element->maintainMembers();
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
        maintainMembers();
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
        maintainMembers();
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
        b->maintainMembers();
        a->maintainMembers();
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
    void almostFull(int size){//size is number of trees to add
        if (!this) return;
        if (size == 0) return;//nothing to build
        Tree * left = new Tree(this);
        _left = left;
        if (size == 1){
            left = nullptr;
            return;//I just wanted the left one
        }
        Tree * right = new Tree(this);
        _right = right;
        if (size == 2) {
            left  = nullptr;
            right = nullptr;
            return;//just wanted two children
        }
        size = size - 2;//already built to new children
        if (size != 0) {
            int leftSize = 0, rightSize = 0;
            leftSize = (size % 2 == 0 ? size / 2 : size / 2 + 1);//I'm left wing (just kidding)
            rightSize = size - leftSize;
            left  = nullptr;
            right = nullptr;
            _left->almostFull(leftSize);
            _right->almostFull(rightSize);
        }
        left  = nullptr;
        right = nullptr;
        return;
    }
    void emptyTree(){//use with caution! memory leak warning
        if (!this) return;
        _key  = nullptr;
        _data = nullptr;
        _left->emptyTree();
        _right->emptyTree();
    }
    void fillTree(K** keys, T** datas, int * i){
        if (!this) return;
        _left ->fillTree(keys, datas, i);
        _key  =  keys[*i];
        _data = datas[*i];
        *i = *i + 1;
        _right->fillTree(keys, datas, i);
        _height  = getHeight();
        _rank    = getRank()  ;
        _dataSum = new T(getSum());
    }

    void detachAllData(){
        if (!this) return;
        _left-> detachAllData();
        _right->detachAllData();
        _data = nullptr;
    }

    void detachAllKeys(){
        if (!this) return;
        _left-> detachAllKeys();
        _right->detachAllKeys();
        _key = nullptr;
    }

    int getLength(){
        if (!this) return 0;
        return _left->getLength() + _right ->getLength() + 1 ;
    }

    Tree* getMax() const{
        if (!this) return nullptr;
        if (_right) return _right->getMax();
        return this;
    }

    Tree* getMin() const{
        if (_left) return _left->getMin();
        return this;
    }

    template<class A>
    void getInorder(A** dataArr, int* i, epart e){
        if (_left ) _left ->getInorder(dataArr, i, e);
        dataArr[*i] = (e == DATA ? (A*)_data : (A*)_key);
        *i = *i + 1;
        if (_right) _right->getInorder(dataArr, i, e);
    }

    int getRealRank(){
        if(!this) return 0;
        return (_left->getRealRank()+_right->getRealRank()+1);
    }
    T checkSum(){
        T sum = *_data;
        if (_left){
            sum += _left->checkSum();
        }
        if (_right){
            sum += _right->checkSum();
        }
        return sum;
    }

    void indexSum(int i, T* sum){
        if (i == 0){
            return;
        }
        int leftRank = (_left ? _left->_rank : 0);
        if (leftRank == i - 1){// our journey is done
            *sum = *sum + *_data;
            *sum = (_left ? *_left->_dataSum + *sum : *sum);
            return;
        }
        else {//keep going down
            if (leftRank > i - 1 && _left){
                _left->indexSum(i, sum);
                return;
            }
            else if (_right) {// going right
                *sum = *sum + *_data;
                *sum = (_left ? *sum + *(_left->_dataSum) : *sum);
                _right->indexSum(i - leftRank - 1, sum);
                return;
            }
        }
    }

};

#endif //WET2_RANKTREE_H
