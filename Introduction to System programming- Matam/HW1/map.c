#include <stdio.h>
#include <stdlib.h>
#include "map.h"


typedef struct node_t {
    MapKeyElement key;
    MapDataElement  data;
    struct node_t * next;//Node technically doesnt exist yet
}*Node;

struct Map_t {
    Node                  head;
    Node                  tail;
    Node                  iterator;
    copyMapDataElements   copyDataElement;
    copyMapKeyElements    copyKeyElement;
    freeMapDataElements   freeDataElement;
    freeMapKeyElements    freeKeyElement;
    compareMapKeyElements compareKeyElements;
} ;


//============= Our Funccs =================

//creat our own iterator
Node MapIter(Map map);

//Advances our own iterator
Node MapIterNext(Node mapIter);

//checking if the map is empty
bool MapIsEmpty(Map map);

//searching a value by its key
Node MapSearchByKey(Map map, MapKeyElement key);

//adding a new node to the map
Node AddNode(Map map, MapKeyElement key, MapDataElement data) ;

Node CopyNode(Map map, Node node);

//freeing a node
void FreeNode(Map map, Node node);

//inserting a node in the middle of the map
Node InsertMidNode(Map map, Node prev, Node newNode, Node next);

//creating a node
Node createNode();

//============================================

Map mapCreate(copyMapDataElements copyDataElement,
              copyMapKeyElements copyKeyElement,
              freeMapDataElements freeDataElement,
              freeMapKeyElements freeKeyElement,
              compareMapKeyElements compareKeyElements) {
    Map map = malloc(sizeof(*map));
    if (map == NULL ||
        copyDataElement == NULL ||
        copyKeyElement == NULL ||
        freeDataElement == NULL ||
        freeKeyElement == NULL ||
        compareKeyElements == NULL
            ) {
        return NULL;
    }
    map->head = NULL;
    map->tail = map->head;
    map->iterator = map->head;
    map->copyDataElement = copyDataElement;
    map->copyKeyElement = copyKeyElement;
    map->freeDataElement = freeDataElement;
    map->freeKeyElement = freeKeyElement;
    map->compareKeyElements = compareKeyElements;
    return map;
}

void mapDestroy(Map map) {
    if (map == NULL)
        return;
    mapClear(map);
    free(map);
}

Map mapCopy(Map map) {
    if (map == NULL) {
        return NULL;
    }
    Map newMap = mapCreate(
            map->copyDataElement,
            map->copyKeyElement,
            map->freeDataElement,
            map->freeKeyElement,
            map->compareKeyElements);
    if (newMap == NULL) return NULL;
    if(MapIsEmpty(map)){//no need to copy elements
        return newMap;
    }
    int map_size = mapGetSize(map);
    mapGetFirst(newMap);
    mapGetFirst(map);
    Node mapIter = MapIter(map);
    //head handling
    if( AddNode(newMap, mapIter->key, mapIter->data) == NULL){//AddNode copies
        mapDestroy(newMap);
        return NULL;
    }
    Node newmapIter = MapIter(newMap);
    mapIter = MapIter(map);
    mapIter = MapIterNext(mapIter);

    // handling the rest
    // now mapIter is pointing to the second node of original
    // now newMapIter is pointing to the first node of copy
    for (int i = 1; i < map_size ; i++) {
        if( AddNode(newMap, mapIter->key, mapIter->data) == NULL){//AddNode copies
            mapDestroy(newMap);
            return NULL;
        }
        if (i == map_size-1){
            break;//end of map - finished copying
        }
        mapIter = MapIterNext(mapIter);
        newmapIter = MapIterNext(newmapIter);
    }
    return newMap;
}


int mapGetSize(Map map) {
    if (map == NULL) return -1;
    Node mapIter = MapIter(map);
    int size = 0;
    while(mapIter != NULL){
        size++;
        mapIter = MapIterNext(mapIter);
    }
    return size;
}

bool mapContains(Map map, MapKeyElement element) {
    if (map == NULL || element == NULL) return false;
    return (MapSearchByKey(map, element) == NULL ? false : true);
}




/**
*	mapPut: Gives a specified key a specific value.
*  Iterator's value is undefined after this operation.
*
* @param map - The map for which to reassign the data element
* @param keyElement - The key element which need to be reassigned
* @param dataElement - The new data element to associate with the given key.
*      A copy of the element will be inserted as supplied by the copying function
*      which is given at initialization and old data memory would be
*      deleted using the free function given at initialization.
* @return
* 	MAP_NULL_ARGUMENT if a NULL was sent as map
* 	MAP_OUT_OF_MEMORY if an allocation failed (Meaning the function for copying
* 	an element failed)
* 	MAP_SUCCESS the paired elements had been inserted successfully
*/
MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement) {
    if (map == NULL) {
        return MAP_NULL_ARGUMENT;
    }
    if (mapContains(map, keyElement)) {
        MapDataElement newdata = map->copyDataElement(dataElement);
        if (newdata == NULL) return MAP_OUT_OF_MEMORY;//will be needed anyway.
        Node existedNode = MapSearchByKey(map, keyElement);
        map->freeDataElement(existedNode->data);
        existedNode->data = newdata;
        map->iterator = NULL;
        return MAP_SUCCESS;
    }
    //not found -> create new and inserts in the right place according to key value
    MapResult addResult = (AddNode(map, keyElement, dataElement)? MAP_SUCCESS : MAP_OUT_OF_MEMORY);
    map->iterator = NULL;
    return addResult;//AddNode copies
}

MapDataElement mapGet(Map map, MapKeyElement keyElement) {
    if (map == NULL || keyElement == NULL) {
        return NULL;
    }
    Node RealKeyNode = MapSearchByKey(map, keyElement);
    return (RealKeyNode == NULL ? NULL : RealKeyNode->data);
}

MapResult mapRemove(Map map, MapKeyElement keyElement) {
    if (map == NULL || keyElement == NULL) {
        return MAP_NULL_ARGUMENT;
    }
    if (!mapContains(map, keyElement)) return MAP_ITEM_DOES_NOT_EXIST;
    //searching for the node right before the one to remove
    mapGetFirst(map);
    if (map->compareKeyElements(map->iterator->key, keyElement) == 0){//Cut his head off!!
        Node headNode = map->head;
        map->head = map->head->next;
        FreeNode(map, headNode);
        map->iterator = NULL;
        return  MAP_SUCCESS;

    }
    mapGetFirst(map);
    while (map->compareKeyElements(map->iterator->next->key, keyElement) != 0) {
        mapGetNext(map);
    }
    Node nodeToRemove = map->iterator->next;
    map->iterator->next = nodeToRemove->next;
    FreeNode(map, nodeToRemove);
    map->iterator = NULL;
    return MAP_SUCCESS;

}

MapKeyElement mapGetFirst(Map map) {
    if (map == NULL || MapIsEmpty(map))
        return NULL;
    map->iterator = map->head;
    return map->head->key;
}

MapKeyElement mapGetNext(Map map) {
    if ( !map || !(map->iterator) || !map->iterator->next) return NULL;
    map->iterator = map->iterator->next;
    return map->iterator->key;
}


MapResult mapClear(Map map) {
    if (map == NULL) return MAP_NULL_ARGUMENT;
    int originalSize = mapGetSize(map);
    for(int i = 0; i < originalSize; i++){
        mapRemove(map,map->head->key);
    }
    return MAP_SUCCESS;
}

//implementation
//============ Our Funcsss ===================

Node MapIter(Map map) {
    Node mapIter = map->head;
    return mapIter;
}
Node MapIterNext(Node mapIter) {
    mapIter = mapIter->next;
    return mapIter;
}

bool MapIsEmpty(Map map) {
    return (map->head == NULL ? true : false);
}
Node MapSearchByKey(Map map, MapKeyElement key) {
    if (map == NULL || key == NULL) return false;//NULL argument check
    Node mapIter = MapIter(map);
    int map_size = mapGetSize(map);
    for (int i = 0; i < map_size; i++) {
        if (map->compareKeyElements(key, mapIter->key) == 0)
            return mapIter;
        mapIter = MapIterNext(mapIter);
    }
    return NULL;//key not found
}
/**
* AddNode: Copies and links a new node to the map.
*
* @param map - Target map.
* @return
*	NULL if failed.
*   the copied node if succeeded.
*/
Node AddNode(Map map, MapKeyElement key, MapDataElement data) {
    if ( map == NULL || key == NULL || data == NULL) return NULL;
    Node newNode = createNode();
    MapKeyElement keyCopy  = map->copyKeyElement (key);
    MapKeyElement dataCopy = map->copyDataElement(data);
    if (keyCopy == NULL || dataCopy == NULL || newNode == NULL) {
        map->freeKeyElement(keyCopy);
        map->freeDataElement(dataCopy);
        free(newNode);
        return NULL;
    }
    //coping successful
    newNode->key  = keyCopy;
    newNode->data = dataCopy;
    if (MapIsEmpty(map)){//no head
        InsertMidNode(map, NULL, newNode, NULL);
        return newNode;
    }
    else{//search for the right place
        if (mapGetSize(map) == 1) {//map is head or tail only
            if (map->compareKeyElements(newNode->key, map->head->key) > 0) {
                InsertMidNode(map, map->head, newNode, NULL);
                return newNode;
            }
            InsertMidNode(map, NULL, newNode, map->head);// assuming not in map, so it must be < than head
            return newNode;
        }
        //map is at list 2 nodes long
        if(map->compareKeyElements(newNode->key,map->head->key) < 0){// < then head
            InsertMidNode(map, NULL, newNode, map->head);
            return  newNode;
        }
        Node next = map->head;
        next = next->next;
        Node prev = map->head;
        for (int i = 1; i < mapGetSize(map) ; i++) {
            if (map->compareKeyElements(newNode->key, next->key) < 0
                && map->compareKeyElements(newNode->key, prev->key) > 0) {// assuming not in map
                InsertMidNode(map, prev, newNode, next);
                return newNode;
            }
            next = next->next;
            prev = prev->next;
        }
        // assuming not in map, so it must be > than tail
        InsertMidNode(map, map->tail, newNode, NULL);
        return newNode;
    }

}

void FreeNode(Map map, Node node) {
    map->freeDataElement(node->data);
    map->freeKeyElement(node->key);
    free(node);
}
Node InsertMidNode(Map map, Node prev, Node newNode, Node next){
    if (newNode == NULL || map == NULL) return NULL;
    if (MapIsEmpty(map)) {
        map->head = newNode  ;
        map->tail = map->head;
        map->iterator = map->head;
    }
    else if (prev == NULL && next != NULL) {//Insert as head
        map->head = newNode;
        newNode->next = next;
    }
    else if (prev != NULL && next == NULL) {//Insert as tail
        prev->next = newNode;
        map->tail = newNode;
    }
    else {//Insert between two nodes. Please don't complain about duplication
        prev->next = newNode;
        newNode->next = next;
    }
    map->iterator = NULL;
    return newNode;
}

Node createNode() {
    Node ptr = malloc(sizeof(*ptr));
    if (!ptr) {
        return NULL;
    }
    ptr->key = NULL;
    ptr->data = NULL;
    ptr->next = NULL;
    return ptr;
}

//============================================
