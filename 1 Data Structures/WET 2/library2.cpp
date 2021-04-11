//
// Created by IdoSarel on 01/14/20.
//
#include "library2.h"
#include "DataCenterManager.h"

void * Init(int n){
    DataCenterManager *DS = new DataCenterManager(n);
    return (void*)DS;
}

StatusType MergeDataCenters(void *DS, int dataCenter1, int dataCenter2){
    if (!DS) return INVALID_INPUT;
    try {
        ((DataCenterManager *) DS)->mergeDataCenters(dataCenter1, dataCenter2);
        return SUCCESS;
    }
    catch (InvalidInputException& invalidExc){
        return INVALID_INPUT;
    }
    catch (bad_alloc& ba){
        return ALLOCATION_ERROR;
    }
    catch (...){
        return FAILURE;
    }
}

StatusType AddServer(void *DS, int dataCenterID, int serverID){
    if (!DS) return INVALID_INPUT;
    try {
        ((DataCenterManager *) DS)->addServer(dataCenterID, serverID);
        return SUCCESS;
    }
    catch (InvalidInputException& invalidExc){
        return INVALID_INPUT;
    }
    catch (bad_alloc& ba){
        return ALLOCATION_ERROR;
    }
    catch (...){
        return FAILURE;
    }
}

StatusType RemoveServer(void *DS, int serverID){
    if (!DS) return INVALID_INPUT;
    try {
        ((DataCenterManager *) DS)->RemoveServer(serverID);
        return SUCCESS;
    }
    catch (InvalidInputException& invalidExc){
        return INVALID_INPUT;
    }
    catch (bad_alloc& ba){
        return ALLOCATION_ERROR;
    }
    catch (...){
        return FAILURE;
    }
}

StatusType SetTraffic(void *DS, int serverID, int traffic){
    if (!DS) return INVALID_INPUT;
    try {
        ((DataCenterManager *) DS)->setTraffic(serverID, traffic);
        return SUCCESS;
    }
    catch (InvalidInputException& invalidExc){
        return INVALID_INPUT;
    }
    catch (bad_alloc& ba){
        return ALLOCATION_ERROR;
    }
    catch (...){
        return FAILURE;
    }
}

StatusType SumHighestTrafficServers(void *DS, int dataCenterID, int k, int *traffic){
    if (!DS || !traffic) return INVALID_INPUT;
    try {
        *traffic = ((DataCenterManager *) DS)->sumHighestTrafficServers(dataCenterID, k);
        return SUCCESS;
    }
    catch (InvalidInputException& invalidExc){
        return INVALID_INPUT;
    }
    catch (bad_alloc& ba){
        return ALLOCATION_ERROR;
    }
    catch (...){
        return FAILURE;
    }
}

void Quit(void **DS){
    if (!DS) return;
    ((DataCenterManager *) *DS)->quitPrep();
    delete ((DataCenterManager *) *DS);
    *DS = nullptr;
}
