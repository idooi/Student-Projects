//
// Created by IdoSarel on 01/13/20.
//

#ifndef WET2_DATACENTERMANAGER_H
#define WET2_DATACENTERMANAGER_H

#include "Hash.h"
#include "UnionFind.h"
#include "DataCenter.h"
#include "DSexceptions.h"

class DataCenterManager {
private:
    Hash hTable;
    UnionFind<DataCenter> dataCenterUnion;
    Root<int,Server> allServersTree;
    int numOfAllServers;
    int _n;
public:
    DataCenterManager(int n):
            hTable(Hash()), dataCenterUnion(UnionFind<DataCenter>(n)),
            allServersTree(Root<int,Server>()), numOfAllServers(0), _n(n)
            {
    }
    ~DataCenterManager() = default;
    void mergeDataCenters(int dataCenterId1, int dataCenterId2){
        if (InvalidDataCenterId(dataCenterId1) ||
            InvalidDataCenterId(dataCenterId2)){
            throw InvalidInputException();
        }
        Node<DataCenter>* dataCenter1 = dataCenterUnion.find(dataCenterId1 - 1);
        Node<DataCenter>* dataCenter2 = dataCenterUnion.find(dataCenterId2 - 1);
        dataCenterUnion.unionGroups(dataCenter1, dataCenter2);
    }
    void addServer(int dataCenterID, int serverID){
        if (InvalidDataCenterId(dataCenterID) || serverID <= 0 ){
            throw InvalidInputException();
        }
        if (hTable.search(serverID)) {
            throw FailureException();
        }
        Server * newServer = new Server(serverID, dataCenterID, 0);
        hTable.insert(newServer);
        DataCenter* dataCenter = getDataCenterByID(dataCenterID);
        dataCenter->addIdleServer();
        numOfAllServers++;
    }
    void RemoveServer(int serverID){
        if (serverID <= 0){
            throw InvalidInputException();
        }
        Server * server = hTable.search(serverID);
        if (!server){
            throw FailureException();
        }
        //remove from main tree
        Tree<int,Server> * detached = allServersTree.detach(server);
        detached->setKey(nullptr);
        allServersTree.deleteTree(detached);
        //remove from the parent dataCenter
        DataCenter* dataCenter = getDataCenterByID(server->getDataCenterID());
        dataCenter->detachServer(server);
        //remove from the hash table
        hTable.remove(server->getServerID());
        //actual remove
        delete server;
        numOfAllServers--;
    }
    void setTraffic(int serverID, int newTraffic){
        if (newTraffic < 0 || serverID <= 0){
            throw  InvalidInputException();
        }
        Server * server = hTable.search(serverID);
        if (!server){
            throw FailureException();
        }
        DataCenter* dataCenter = getDataCenterByID(server->getDataCenterID());
        int oldTraffic = server->getTraffic();
        server->changeTraffic(newTraffic);
        //update the servers tree in dataCenter
        dataCenter->updateServerTraffic(server, newTraffic, oldTraffic);
        //update allServers tree
        updateAllServerTraffic(server, newTraffic, oldTraffic);
    }
    int sumHighestTrafficServers(int dataCenterID, int k){
        if ( dataCenterID < 0 || dataCenterID > _n || k < 0 ){
            throw InvalidInputException();
        }
        int * sum;
        int sumValue;
        if (dataCenterID == 0){
            if (k > numOfAllServers){
                k = numOfAllServers ;//I think its invalid but whateva
            }
            sum = allServersTree.sumGreatestData(k);
        }else{
            DataCenter* dataCenter = getDataCenterByID(dataCenterID);
            if (k > dataCenter->getNumOfServers()){//I think its invalid but whateva
                 k = dataCenter->getNumOfServers();
            }
            sum = dataCenter->getSumHighestTrafficServers(k);
        }
        sumValue = *sum;
        delete sum;
        return sumValue;
    }

    void quitPrep(){
        allServersTree.detachAllRootKeys();
        DataCenter * dataCenter;
        for (int i = 0; i < _n; i++){
            dataCenter = getDataCenterByID(i + 1);
            dataCenter->destructPrep();
        }
    }

    bool InvalidDataCenterId(int id) {
        return (id <= 0 || id > _n);
    }
    DataCenter* getDataCenterByID(int dataCenterID) {
        Node<DataCenter>* dataCenterNode = dataCenterUnion.find(dataCenterID - 1);
        DataCenter* dataCenter = dataCenterNode->getData();
        return dataCenter;
    }
    void updateAllServerTraffic(Server * server, int newTraffic, int oldTraffic){
        Tree<int,Server>* serverNode;
        if (oldTraffic == 0){//not in tree, so traffic is 0
            int * newData = new int(newTraffic);
            serverNode = new Tree<int,Server>(server,newData, nullptr);
        }
        else {//already in tree
            server->changeTraffic(oldTraffic);//to find the server in the tree
            serverNode = allServersTree.detach(server);
            server->changeTraffic(newTraffic);
            *(serverNode->getData()) = newTraffic;
            *(serverNode->getDataSum()) = 0;
        }
        allServersTree.insertTree(serverNode);
    }

};


#endif //WET2_DATACENTERMANAGER_H
