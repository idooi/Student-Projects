//
// Created by 97254 on 02/01/2020.
//

#ifndef MIVNIRATUV2_DATACENTER_H
#define MIVNIRATUV2_DATACENTER_H
#include "Server.h"
#include "root.h"

#define INVALID_K -1

class DataCenter {
    Root<int,Server>* serversTree;
    int numOfServers;
public:

    DataCenter(): serversTree(new Root<int,Server>()), numOfServers(0){}
    DataCenter(int i): serversTree(nullptr), numOfServers(0){}
    ~DataCenter(){
        delete serversTree;
    }
    friend DataCenter* operator+(DataCenter& dataCenter1, DataCenter& dataCenter2);
    int getNumOfServers() const{
        return numOfServers;
    }
    void addIdleServer(){
        numOfServers++;
    }
    void detachServer(Server* server){
        numOfServers--;
        if (!serversTree->exists(server)){//traffic is 0
            return;
        }
        Tree<int, Server>* serverNode = serversTree->detach(server);
        serverNode->setKey(nullptr);
        serversTree->deleteTree(serverNode);
    }
    void updateServerTraffic(Server * server, int newTraffic, int oldTraffic){
        Tree<int,Server>* serverNode;
        if (oldTraffic == 0){//not in tree
            int * newData = new int(newTraffic);
            serverNode = new Tree<int,Server>(server,newData, nullptr);
        }
        else {//already in tree
            server->changeTraffic(oldTraffic);//to find the server in the tree
            serverNode = serversTree->detach(server);
            server->changeTraffic(newTraffic);
            *(serverNode->getData()) = newTraffic;
            *(serverNode->getDataSum()) = 0;
        }
        serversTree->insertTree(serverNode);
    }

    int * getSumHighestTrafficServers(int k) const
    {
        return serversTree->sumGreatestData(k);
    }

    void destructPrep(){
        serversTree->detachAllRootKeys();
    }

};
DataCenter* operator+(DataCenter& dataCenter1, DataCenter& dataCenter2){
    Root<int,Server> * merged = mergeRoots(dataCenter1.serversTree, dataCenter2.serversTree);
    dataCenter1.serversTree->emptyRoot();
    dataCenter2.serversTree->emptyRoot();
    DataCenter * newDataCenter = new DataCenter(1);
    newDataCenter->serversTree = merged;
    newDataCenter->numOfServers = dataCenter1.numOfServers + dataCenter2.numOfServers;
    return newDataCenter;
}

#endif //MIVNIRATUV2_DATACENTER_H