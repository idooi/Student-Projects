//
// Created by 97254 on 03/12/2019.
//

#ifndef MIVNI2_DATACENTER_H
#define MIVNI2_DATACENTER_H
#include "server.h"
#include "node.h"
#include "DSexceptions.h"



class dataCenter {
    int dataCenterID;
    int numOfServers;
    node<server> **listOfServers;
    node<server> *linuxHead;
    node<server> *linuxTail;
    node<server> *windowsHead;
    node<server> *windowsTail;
    int numOfLinuxServers;
    int numOfWindowsServers;
public:
    dataCenter(int dataCenterID, int numOfServers):
            dataCenterID(dataCenterID), numOfServers(numOfServers),
            listOfServers(new node<server>*()), windowsHead(nullptr),
            windowsTail(nullptr), numOfLinuxServers(numOfServers), numOfWindowsServers(0)
    {
        *listOfServers = new node<server>[numOfServers];
        linuxHead = (*listOfServers);
        linuxTail = (*listOfServers) + numOfServers - 1;
        node<server> *temp = linuxHead;
        for(int i = 0; i < numOfServers; i++){
            (*listOfServers)[i].data->serverID = i;
            if(i > 1){
                temp->next = (*listOfServers) + i;
                temp->prev = (*listOfServers) + i - 2;
                temp = temp->next;
                if(i == numOfServers - 1){
                    temp->prev = (*listOfServers) + i - 1;
                }
            }
            else if(i == 1){
                temp->next = (*listOfServers) + i;
                temp = temp->next;
            }
        }
    }
    ~dataCenter(){
        delete[] (*listOfServers);
        delete listOfServers;
    }
    int getNumOfLinuxServers(){
        return numOfLinuxServers;
    }
    int getNumOfWindowsServers(){
        return numOfWindowsServers;
    }
    int getNumOfServers(){
        return numOfServers;
    }
    int getDataCenterID(){
        return dataCenterID;
    }
    bool isServerFree(int serverID){
        return (*listOfServers)[serverID].data->isFree;
    }

    node<server>* removeNode(node<server> *nodeToRemove){
        node<server> *prevNode = nodeToRemove->prev;
        node<server> *nextNode = nodeToRemove->next;
        if(!nextNode && !prevNode){
            if(nodeToRemove->data->os == 0){ // we changed the os before calling the function
                linuxHead = linuxTail = nullptr;
            }
            else{
                windowsHead = windowsTail = nullptr;
            }
            return nodeToRemove;
        }
        if(nextNode){
            nextNode->prev = prevNode;
        }
        if(prevNode){
            prevNode->next = nextNode;
        }
        if(nodeToRemove == linuxHead){
            linuxHead = linuxHead->next;
            linuxHead->prev = nullptr;
        }
        else if(nodeToRemove == linuxTail){
            linuxTail = linuxTail->prev;
            linuxTail->next = nullptr;
        }
        else if(nodeToRemove == windowsHead){
            windowsHead = windowsHead->next;
            windowsHead->prev = nullptr;
        }
        else if(nodeToRemove == windowsTail){
            windowsTail = windowsTail->prev;
            windowsTail->next = nullptr;
        }
        nodeToRemove->next = nullptr;
        nodeToRemove->prev = nullptr;
        return nodeToRemove;
    }

    void requestServer(int serverID, int os, int *assignedID) {
        if (isServerFree(serverID) == true) {
            if((*listOfServers)[serverID].data->os != os){
                if(os == 0){
                    numOfLinuxServers++;
                    numOfWindowsServers--;
                }
                else{
                    numOfLinuxServers--;
                    numOfWindowsServers++;
                }
            }
            (*listOfServers)[serverID].data->isFree = false;
            *assignedID = serverID;
            node<server> *temp = (*listOfServers) + serverID;
            removeNode(temp);
            (*listOfServers)[serverID].data->os = os;
            return;
        }
        if (os == 0) {
            if (linuxHead) {
                linuxHead->data->isFree = false;
                *assignedID = linuxHead->data->serverID;
                removeNode(linuxHead);
                return;
            }
            else if (windowsHead) {
                windowsHead->data->isFree = false;
                *assignedID = windowsHead->data->serverID;
                node<server>* temp = removeNode(windowsHead);
                temp->data->os = os;
                numOfWindowsServers--;
                numOfLinuxServers++;
                return;
            }
        }
        else {
            if (windowsHead) {
                windowsHead->data->isFree = false;
                *assignedID = windowsHead->data->serverID;
                removeNode(windowsHead);
                return;
            }
            else if (linuxHead) {
                linuxHead->data->isFree = false;
                *assignedID = linuxHead->data->serverID;
                node<server>* temp = removeNode(linuxHead);
                temp->data->os = os;
                numOfWindowsServers++;
                numOfLinuxServers--;
                return;
            }
        }
        throw FailureException();
    }

    void freeServer(int serverID){
        if((*listOfServers)[serverID].data->isFree == true){
            throw FailureException();
        }
        if((*listOfServers)[serverID].data->os == 0){
            ((*listOfServers) + serverID)->prev = linuxTail;
            if(linuxTail) {
                linuxTail->next = ((*listOfServers) + serverID);
            }
            else{
                linuxHead = ((*listOfServers) + serverID);
            }
            linuxTail = ((*listOfServers) + serverID);
        }
        else{
            ((*listOfServers) + serverID)->prev = windowsTail;
            if(windowsTail) {
                windowsTail->next = ((*listOfServers) + serverID);
            }
            else{
                windowsHead = ((*listOfServers) + serverID);
            }
            windowsTail = ((*listOfServers) + serverID);
        }
        (*listOfServers)[serverID].data->isFree = true;
    }

};




#endif //MIVNI2_DATACENTER_H