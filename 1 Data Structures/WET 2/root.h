//
// Created by IdoSarel on 01/14/20.
//

#ifndef WET2_ROOT_H
#define WET2_ROOT_H

#include "tree.h"

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
        //element->resetElement();
        if (!_root){
            _root = element;
            _root->maintainMembers();
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
            //  _root->maintainHeight();
            _root->fixTree();
        }
        else{
            detachedFather->_left  = (detachedFather-> _left  == detached ? nullptr : detachedFather-> _left );
            detachedFather->_right = (detachedFather-> _right == detached ? nullptr : detachedFather-> _right);
            // detachedFather->maintainHeight();
            detachedFather->fixTree();
        }
        detached->_height = 0      ;
        detached->_father = nullptr;
        detached->_left   = nullptr;
        detached->_right  = nullptr;
        detached->_rank   = 1;
        *detached ->_dataSum = *detached->_data;
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

    void deleteRoot(){
        deleteTree(_root);
    }

    template<class A>
    A** getArrayInorder(epart e) const{
        int treeLength = getLength();
        if (treeLength == 0){
            return nullptr;
        }
        A** dataArr =  new A*[treeLength];
        int * i = new int();
        *i = 0;
        _root->template getInorder<A>(dataArr, i, e);
        delete i;
        return dataArr;
    }
    void detachAllRootData(){
        _root->detachAllData;
    }
    void detachAllRootKeys(){
        _root->detachAllKeys();
    }
    void rootAlmostFull(int size){
        _root->almostFull(size);
    }
    void fillRoot(K** keys, T** datas, int * i){
        _root->fillTree(keys, datas, i);
    }
    void printRoot(){
        _root->printTree();
    }
    void emptyRoot(){
        _root->emptyTree();
    }

    T* sumGreatestData(int k) const{
        int length = _root->getLength();
        T* maxSum = getIndexSum(length    );
        T* midSum;
        T* greatestDataSum = new T();
        *greatestDataSum = *maxSum;
        if (k < length) {
            midSum = getIndexSum(length - k);
            *greatestDataSum = *greatestDataSum - *midSum;
            delete midSum;
        }
        delete maxSum;
        return greatestDataSum;
    }

    T* getIndexSum(int i) const{
        T* indexSum = new T();
        *indexSum = *(_root->_dataSum) - *(_root->_dataSum);//Init
        _root->indexSum(i, indexSum);
        return  indexSum;

    }
};


template <class T, class K>
T** merge(K** k1, T** a1, int length1, K** k2, T** a2, int length2){
    int length = length1 + length2;
    if (length == 0){
        return nullptr;
    }
    T** merged = new T*[length];
    int i1 = 0, i2 = 0;
    for(int i = 0; i < length; i++){
        if (i2 >= length2 ||
            (i1 < length1 && *k1[i1] < *k2[i2] )){
            merged[i] = a1[i1];
            i1++;
            continue;
        }
        else if(i1 >= length1 || i2 < length2){
            merged[i] = a2[i2];
            i2++;
            continue;
        }
    }
    return merged;
}

int max(int a, int b){
    return ( a < b ? b : a);
}

template <class T>
void printArray(T** a, int length) {
    for (int i = 0; i < length; i++) {
        cout << *a[i] << " ,";
    }
    cout << endl;
}

template <class T, class K>
Root<T,K>* mergeRoots(Root<T,K> * root1, Root<T,K> * root2){
    T** dataArr1 = (T**)root1->template getArrayInorder<T>(DATA);
    T** dataArr2 = (T**)root2->template getArrayInorder<T>(DATA);
    //O(m)
    K** keyArr1  = (K**)root1->template getArrayInorder<K>(KEY );
    K** keyArr2  = (K**)root2->template getArrayInorder<K>(KEY );
    //O(m)
    int length1  = root1->getLength();
    int length2  = root2->getLength();
    //O(m)
    T** mergedDataArray = merge(keyArr1, dataArr1, length1, keyArr2, dataArr2, length2);
    //O(m)
    K** mergedKeyArray  = merge(keyArr1, keyArr1,  length1, keyArr2, keyArr2,  length2);
/*    printArray<K>(mergedKeyArray,length1+length2);
    printArray<T>(mergedDataArray,length1+length2);
    */
    //O(m)
    for (int i = 0; i < length1; i++){
        dataArr1[i] = nullptr;
        keyArr1[i]  = nullptr;
    }
    for (int i = 0; i < length2; i++){
        dataArr2[i] = nullptr;
        keyArr2[i]  = nullptr;
    }
    //O(m)
    delete[] dataArr1;
    delete[] dataArr2;
    //O(m)
    delete[]  keyArr1;
    delete[]  keyArr2;
    //O(m)
    int size = length1 + length2;
    Root<T,K> * mergedRoot = new Root<T,K>() ;
    if (size == 0){
        delete[] mergedKeyArray;
        //O(m)
        delete[] mergedDataArray;
        //O(m)
        return mergedRoot;
    }
    Tree<T,K>* treeRoot   = new Tree<T,K>(nullptr);
    mergedRoot->insertTree(treeRoot);
    int* j = new int();
    *j = 0;
    //O(1)
    mergedRoot->rootAlmostFull(size - 1);//root tree is already built
    //O(m)
    mergedRoot->fillRoot(mergedKeyArray, mergedDataArray, j);
    //O(m)
    delete j;
    //O(1)
    for (int i = 0; i < length1 + length2; i++){
        mergedDataArray[i] = nullptr;
        mergedKeyArray[i]  = nullptr;
    }
    //O(m)
    delete[] mergedKeyArray;
    //O(m)
    delete[] mergedDataArray;
    //O(m)
    return mergedRoot;//TODO: empty and delete the former roots outside of function
}



#endif //WET2_ROOT_H
