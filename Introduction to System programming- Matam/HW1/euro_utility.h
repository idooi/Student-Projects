//
// Created by IdoSarel on 04/28/19.
//
#include "map.h"
#ifndef EUROVISION_EURO_UTILITY_H
#define EUROVISION_EURO_UTILITY_H

MapKeyElement CopyInt(MapKeyElement n);

void FreeIntOrDouble(MapKeyElement n);

int CompareInts(MapKeyElement n1, MapKeyElement n2);

MapKeyElement CopyDouble(MapKeyElement n);

bool NameIsValid(const char * name );

bool IdIsValid(int id);

const char * CopyName(const char* name);

#endif //EUROVISION_EURO_UTILITY_H

