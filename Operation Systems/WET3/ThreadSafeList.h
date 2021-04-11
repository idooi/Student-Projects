#ifndef THREAD_SAFE_LIST_H_
#define THREAD_SAFE_LIST_H_

#include <pthread.h>
#include <iostream>
#include <iomanip> // std::setw

using namespace std;

template <typename T>
class List 
{
    public:


        class Node {
         public:
          T data;
          Node *next;
          pthread_mutex_t lock;

          Node(T data):data(data), next(nullptr){
              if (pthread_mutex_init(&lock, nullptr) == -1) {
                  cerr << "pthread_mutex_init: failed"<< endl;
                  exit(-1);
              }
          }
          Node(): next(nullptr){
              if (pthread_mutex_init(&lock, nullptr) == -1) {
                    cerr << "pthread_mutex_init: failed"<< endl;
                  exit(-1);
              }
          }
          ~Node() {
              pthread_mutex_destroy(&lock);
          }
        };


        /**
         * Insert new node to list while keeping the list ordered in an ascending order
         * If there is already a node has the same data as @param data then return false (without adding it again)
         * @param data the new data to be added to the list
         * @return true if a new node was added and false otherwise
         */
        bool insert(const T& data) {
			Node* newNode;
            pthread_mutex_lock(&dummy->lock);
            Node* current = dummy;
            Node* next = current->next;
            while(next){
                pthread_mutex_lock(&next->lock);
                if(current != dummy && current->data == data) {
                    __insert_test_hook();
                    pthread_mutex_unlock(&current->lock);
                    pthread_mutex_unlock(&next->lock);
                    return false;

                } else if( (current == dummy || current->data < data) && next->data > data) {
                    newNode = new Node(data);
                    current->next = newNode;
                    newNode->next = next;
                    __insert_test_hook();
                    pthread_mutex_unlock(&current->lock);
                    pthread_mutex_unlock(&next->lock);
                    changeSize(1);
                    return true;
                }
                pthread_mutex_unlock(&current->lock);
                current = next;
                next = current->next;
            }
            //last node
            if (current != dummy && current->data == data){
                __insert_test_hook();
                pthread_mutex_unlock(&current->lock);
                return false;
            }
            //if(current->data < data) {
            newNode = new Node(data);
            current->next = newNode;
            __insert_test_hook();
            pthread_mutex_unlock(&current->lock);
            changeSize(1);
            return true;
        }
        /**
         * Remove the node that its data equals to @param value
         * @param value the data to lookup a node that has the same data to be removed
         * @return true if a matched node was found and removed and false otherwise
         */
        bool remove(const T& value) {
            pthread_mutex_lock(&dummy->lock);
            Node* prev = dummy;
            Node* current = prev->next;
            while(current){
                pthread_mutex_lock(&current->lock);
                if(current->data == value) {
                    prev->next = current->next;
                    pthread_mutex_unlock(&current->lock);
                    delete current;
                    __remove_test_hook();
                    pthread_mutex_unlock(&prev->lock);
                    changeSize(-1);
                    return true;
                }
                pthread_mutex_unlock(&prev->lock);
                prev = current;
                current = prev->next;
            }
            __remove_test_hook();
            pthread_mutex_unlock(&prev->lock);
            return false;
        }

        /**
         * Returns the current size of the list
         * @return current size of the list
         */
        unsigned int getSize() {
            pthread_mutex_lock(&sizeLock);
			int len = this->size;
            pthread_mutex_unlock(&sizeLock);
			return len;
        }

        void changeSize(int delta){
            pthread_mutex_lock(&sizeLock);
            size += delta;
            pthread_mutex_unlock(&sizeLock);
        }

		// Don't remove
        void print() {
          Node* temp = dummy->next;
          if (temp == NULL)
          {
            cout << "";
          }
          else if (temp->next == NULL)
          {
            cout << temp->data;
          }
          else
          {
            while (temp != NULL)
            {
              cout << right << setw(3) << temp->data;
              temp = temp->next;
              cout << " ";
            }
          }
          cout << endl;
        }

		// Don't remove
        virtual void __insert_test_hook() {}
		// Don't remove
        virtual void __remove_test_hook() {}
    bool isSorted(){
        pthread_mutex_lock(&dummy->lock);
        if(!dummy->next) {
            pthread_mutex_unlock(&dummy->lock);
            return true;
        }else{
            pthread_mutex_lock(&dummy->next->lock);
            pthread_mutex_unlock(&dummy->lock);
        }
        Node* prev = dummy->next;
        Node* curr = prev->next;
        while(curr) {
            pthread_mutex_lock(&curr->lock);
            if(prev->data >= curr->data) {
                pthread_mutex_unlock(&curr->lock);
                pthread_mutex_unlock(&prev->lock);
                return false;
            }
            pthread_mutex_unlock(&prev->lock);
            prev = curr;
            curr = curr->next;
        }
        pthread_mutex_unlock(&prev->lock);
        return true;
    }


private:
        Node* dummy;
        int size;
        pthread_mutex_t sizeLock;
        public:
    /**
         * Constructor
         */
    List(): dummy(new Node()), size(0)
    {
        if (pthread_mutex_init(&sizeLock, nullptr) == -1){
            cerr << "pthread_mutex_init: failed"<< endl;
            exit(-1);
        }
    }
    /**
     * Destructor
     */
    virtual ~List(){
        pthread_mutex_destroy(&sizeLock);
        Node* current = dummy;
        Node * next = current->next;
        while (next){
            delete current;
            current = next;
            next = current->next;
        }
        delete current;
    }

    // TODO: Add your own methods and data members
};

#endif //THREAD_SAFE_LIST_H_