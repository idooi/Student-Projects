//
// Created by IdoSarel on 04/28/19.
//
#include "stdlib.h"
#include "string.h"
#include "euro_utility.h"
MapKeyElement CopyInt(MapKeyElement n) {
    if (!n) {
        return NULL;
    }
    int *copy = malloc(sizeof(*copy));
    if (!copy) {
        return NULL;
    }
    *copy = *(int *) n;
    return copy;
}

void FreeIntOrDouble(MapKeyElement n) {
    free(n);
}
int CompareInts(MapKeyElement n1, MapKeyElement n2) {
    return (*(int *) n1 - *(int *) n2);
}

MapKeyElement CopyDouble(MapKeyElement n) {
        if (!n) {
            return NULL;
        }
        double *copy = malloc(sizeof(*copy));
        if (!copy) {
            return NULL;
        }
        *copy = *(double *) n;
        return copy;
}

bool NameIsValid(const char * name ){
    if (!name) return false;
    for (int i = 0; i< strlen(name); i++){
        if (name[i] == ' '){
            continue;
        }
           if(name[i] > 'z' || name[i] <  'a' ){ // not lowercase
            return false;
        }
    }
    return true;
}

bool IdIsValid(int id){
    return id >= 0;
}

const char* CopyName(const char* name)
{
    if (!name) return NULL;
    int length = (int)strlen(name);
    char* temp = malloc(sizeof(char)*(length+1));
    if (!temp) return NULL;
    strcpy(temp, name);
    return (const char *)temp;
}
