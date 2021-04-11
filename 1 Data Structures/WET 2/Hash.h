//
// Created by 97254 on 06/01/2020.
//

#ifndef HASHPROJECT_HASH_H
#define HASHPROJECT_HASH_H
#include "Server.h"
# define INITIAL_TABLE_SIZE 20

class Hash {
    struct node{
        Server* server;
        node* next;
        node():server(nullptr), next(nullptr){}
        ~node(){
            delete server;
        };
    };
    struct list{
        node* next;
        list():next(nullptr){}
        ~list() = default;
    };
    list* table;
    int tableSize;
    int numOfServers;
public:
    Hash():table(new list[INITIAL_TABLE_SIZE]), tableSize(INITIAL_TABLE_SIZE), numOfServers(0){}

    ~Hash(){
        list* head = table;
        for (int i = 0; i < tableSize; i++){
            head = table + i;
            for(node * curr = head->next; curr; /*curr = curr->next*/){
               node * toDelete = curr;
               curr = curr->next;
               delete toDelete;
            }
        }
        delete[] table;
    }

    int hashFunc(int key){
        return key % tableSize;
    }

    Server* search(int serverID){
        int index = hashFunc(serverID);
        for(node* cur = table[index].next; cur; cur = cur->next){
            if((cur->server)->getServerID() == serverID){
                return cur->server;
            }
        }
        return nullptr;
    }




    void allocateNewTable(double sizeMulti){
        int oldSize = tableSize;
        tableSize = tableSize*sizeMulti;
        list* biggerTable = new list[tableSize];
        for(int i = 0; i < oldSize; i++){
            for(node* cur = table[i].next; cur;){
                int index = hashFunc(cur->server->getServerID());
                node* temp = biggerTable[index].next;
                biggerTable[index].next = new node();
                biggerTable[index].next->server = cur->server;
                biggerTable[index].next->next = temp;
                node* nodeToDelete = cur;
                cur = cur->next;
                nodeToDelete->server = nullptr;
                delete nodeToDelete;
            }
        }
        delete[] table;
        table = biggerTable;

    }

    void insert(Server *server) {
        if (search(server->getServerID()) != nullptr) {
            return;
        }
        int index = hashFunc(server->getServerID());
        list * head = table + index;
        node *temp = head->next;
        node * newNode = new node();
        newNode->server = server;
        newNode->next = temp;
        head->next = newNode;
        numOfServers++;
        double factor = double(numOfServers) / tableSize;
        if (factor > 0.75) {
            allocateNewTable(2);
        }
    }

    void remove(int serverID){
        if(!search(serverID)){
            return;
        }
        int index = hashFunc(serverID);
        if(table[index].next->server->getServerID() == serverID){
            node *nodeToDelete = table[index].next;
            table[index].next = table[index].next->next;
            nodeToDelete->server = nullptr;
            delete nodeToDelete;
            numOfServers--;
        }
        else {
            for (node *cur = table[index].next; cur && cur->next; cur = cur->next) {
                if(cur->next->server->getServerID() == serverID){
                    node *nodeToDelete = cur->next;
                    cur->next = cur->next->next;
                    nodeToDelete->server = nullptr;
                    delete nodeToDelete;
                    numOfServers--;
                    break;
                }
            }
        }
        double factor = double(numOfServers)/tableSize;
        if(factor < 0.4){
            allocateNewTable(0.5);
        }
    }
};


#endif //HASHPROJECT_HASH_H
