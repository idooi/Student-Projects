//
// Created by IdoSarel on 12/09/19.
//

#include"DataCenterManager.h"
#include "library1.h"


void *Init() {
    DataCenterManager *DS = new DataCenterManager();
    return (void*)DS;
}
StatusType AddDataCenter(void *DS, int dataCenterID, int numOfServers) {
    if (!DS) return INVALID_INPUT;
    try {
        ((DataCenterManager *) DS)->addDataCenter(dataCenterID, numOfServers);
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
StatusType RemoveDataCenter(void *DS, int dataCenterID){
    if (!DS) return INVALID_INPUT;
    try{
        ((DataCenterManager *) DS)->removeDataCenter(dataCenterID);
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
StatusType RequestServer(void *DS, int dataCenterID, int serverID, int os, int *assignedID){
    if (!DS || serverID < 0 || !assignedID || os > 1 || os < 0 ||dataCenterID <= 0) {
        return INVALID_INPUT;
    }
    try {
        ((DataCenterManager *) DS)->serverManager(dataCenterID, serverID, os, assignedID, REQUEST);
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

StatusType FreeServer(void *DS, int dataCenterID, int serverID){
    if  (!DS || serverID < 0 || dataCenterID <= 0){
        return INVALID_INPUT;
    }
    try{
        ((DataCenterManager *) DS)->serverManager(dataCenterID, serverID, -1, nullptr, FREE);
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

StatusType GetDataCentersByOS(void *DS, int os, int **dataCenters, int* numOfDataCenters){
    if(!DS){
        return INVALID_INPUT;
    }
    try{
    ((DataCenterManager *) DS)->getDataCentersByOS(os, dataCenters, numOfDataCenters);
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


