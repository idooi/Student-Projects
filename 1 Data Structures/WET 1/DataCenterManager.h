//
// Created by IdoSarel on 12/06/19.
//

#ifndef DATASTRUCTURES1_DATACENTERMANAGER_H
#define DATASTRUCTURES1_DATACENTERMANAGER_H

#include "dataCenter.h"
#include "tree.h"
#include "DSexceptions.h"
using namespace std;

enum Method{
    REQUEST, FREE
};
enum TreeType{
    LINUX = 0, WINDOWS = 1
};

class OsKey{
public:
    int _osCount;
    int _id;
    OsKey(int osCount, int id): _osCount(osCount), _id(id){}
    ~OsKey() = default;
    bool operator>(OsKey key2){
        if (_osCount == key2._osCount) return _id < key2._id ;
        return _osCount > key2._osCount ;
    }
    bool operator<(OsKey key2){
        if (_osCount == key2._osCount) return _id > key2._id ;
        return _osCount < key2._osCount ;
    }

};

class DataCenterManager {
private:
    Root<dataCenter, int>   idTree ;
    Root<dataCenter, OsKey> linuxTree;
    Root<dataCenter, OsKey> windowsTree;
public:
     DataCenterManager() = default;
    ~DataCenterManager() = default;

    void addDataCenter(int dataCenterID, int numOfServers){
        if (dataCenterID <= 0 || numOfServers <= 0){
            throw InvalidInputException();
        }
        int * idKey = new int();
        *idKey = dataCenterID;
        if (idTree.exists(idKey)) {
            delete idKey;
            throw FailureException();
        }
        dataCenter* dC = new dataCenter(dataCenterID, numOfServers);
        idTree     .insert(idKey, dC);
        OsKey * keyLinux   = new OsKey(numOfServers, dataCenterID);
        OsKey * keyWindows = new OsKey(0,            dataCenterID);
        linuxTree  .insert(keyLinux,   dC);
        windowsTree.insert(keyWindows, dC);
    }
    void removeDataCenter(int dataCenterID){
        if (dataCenterID <= 0) throw InvalidInputException();
        int * idKey = new int();
        *idKey = dataCenterID;
        if (!idTree.exists(idKey)) {
            delete idKey;
            throw FailureException();
        }
        Tree<dataCenter, int> * dCNode = idTree.detach(idKey);
        delete idKey;
        dataCenter * dC = dCNode->getData();
        int numLinux   = dC->  getNumOfLinuxServers();
        int numWindows = dC->getNumOfWindowsServers();
        Tree<dataCenter, OsKey> * dCLinux   = detachDataCenterOfTree(dataCenterID, numLinux  , LINUX  );
        Tree<dataCenter, OsKey> * dCWindows = detachDataCenterOfTree(dataCenterID, numWindows, WINDOWS);
        idTree.deleteTree(dCNode);
        dCLinux->  setData(nullptr);
        dCWindows->setData(nullptr);
        linuxTree.  deleteTree(dCLinux  );
        windowsTree.deleteTree(dCWindows);
    }
    void serverManager(int dataCenterID, int serverID, int os, int* assignedID, Method method){
        int * idKey = new int();
        *idKey = dataCenterID;
        dataCenter* dC = idTree.findData(idKey);
        delete idKey;
        if (!dC) throw FailureException(); //dataCenter doesnt exist
        if (serverID >= dC->getNumOfServers()) throw InvalidInputException();
        int oldNumLinux   = dC->  getNumOfLinuxServers();
        int oldNumWindows = dC->getNumOfWindowsServers();

        if (method == REQUEST){
        dC->requestServer(serverID, os, assignedID);
        } else {//FREE
            dC->freeServer(serverID);
        }
        //getting the os tree nodes using the old key parameters
        Tree<dataCenter, OsKey> * dCLinux   = detachDataCenterOfTree(dataCenterID, oldNumLinux,   LINUX  );
        Tree<dataCenter, OsKey> * dCWindows = detachDataCenterOfTree(dataCenterID, oldNumWindows, WINDOWS);
        //update the linux and windows keys
        dCLinux  ->getKey()->_osCount = dC->  getNumOfLinuxServers();
        dCWindows->getKey()->_osCount = dC->getNumOfWindowsServers();
        //insert the updated nodes to the corresponding trees
        linuxTree.insertTree(dCLinux);
        windowsTree.insertTree(dCWindows);
    }

    Tree<dataCenter, OsKey> * detachDataCenterOfTree(int dataCenterID, int numOfServers, TreeType type){
        Tree<dataCenter, OsKey> * detached;
        OsKey * key;
        switch (type){
            case LINUX:
                key = new OsKey(numOfServers, dataCenterID);
                detached = linuxTree.detach(key);
                break;
            default: //WINDOWS
                key = new OsKey(numOfServers, dataCenterID);
                detached = windowsTree.detach(key);
                break;
        }
        delete key;
        return  detached;
    }

    void getDataCentersByOS(int os, int **dataCenters, int* numOfDataCenters) {
        if (!dataCenters || os > 1 || os < 0 || !numOfDataCenters){
            throw InvalidInputException();
        }
        int countDC = idTree.getLength();
        *numOfDataCenters = countDC;
        if (countDC == 0) throw FailureException();
        *dataCenters = (int *) malloc(countDC * sizeof(int));
        dataCenter **dCArr;
        switch (os) {
            case LINUX:
                dCArr = linuxTree.getDataArrayDesc();
                break;
            default:
                dCArr = windowsTree.getDataArrayDesc();
                break;
        }
        for (int i = 0; i < countDC; i++){
            (*dataCenters)[i] = dCArr[i]->getDataCenterID();
        }
        free(dCArr);
    }

    void quitPrep(){
        linuxTree.detachAllRootData();
        windowsTree.detachAllRootData();
    }

};




#endif //DATASTRUCTURES1_DATACENTERMANAGER_H
