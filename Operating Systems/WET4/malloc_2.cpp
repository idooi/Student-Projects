//
// Created by student on 6/20/20.
//

#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <assert.h>
#define  META_SIZE sizeof(MallocMetadata)
using namespace std;

typedef enum{
    size, count
}countFlag;


struct MallocMetadata {
    size_t size;
    bool isFree;
    MallocMetadata *next;
    MallocMetadata *prev;
};

MallocMetadata* memList = nullptr;
MallocMetadata* lastMetadata = nullptr;

MallocMetadata* find_free_block(size_t size){
    MallocMetadata* current = memList;
    while(current){
        if(current->isFree && current->size >= size){
            current->isFree = false;
            return current;
        }
        current = current->next;
    }
    return current;
}
size_t countOfMem(countFlag what, bool isFree){
    MallocMetadata* curr = memList;
    if (!curr) return 0;
    size_t count = 0;
    while(curr){
        if (!isFree || curr->isFree){
            count += (what == size ? curr->size : 1);
        }
        curr = curr->next;
    }
    return count;
}

size_t _num_free_blocks(){
    return countOfMem(count, true);
}

size_t _num_free_bytes(){
    return countOfMem(size, true);
}

size_t _num_allocated_blocks(){
    return countOfMem(count, false);
}

size_t _num_allocated_bytes(){
    return countOfMem(size, false);
}

size_t _size_meta_data(){
    return META_SIZE ;
}

size_t _num_meta_data_bytes(){
    return _size_meta_data()*_num_allocated_blocks();
}
/*void printMeta(MallocMetadata* metadata){
    cout<< "addr: "<< (void*)metadata << " allocated: "<< (void*)((char*)metadata + META_SIZE) <<" size: " << metadata->size << " free: " << metadata->isFree << endl;
}
void debug(){
    std::cout<<"\nfree: "<< _num_free_blocks() << " freeSize: " << _num_free_bytes()
             <<"  blocks: " << _num_allocated_blocks() << " bytes: "<< _num_allocated_bytes()<<std::endl;
    cout<< "Heap: " <<endl;
    MallocMetadata* curr = memList;
    MallocMetadata* prev = memList;
    while(curr){
        printMeta(curr);
        prev = curr;
        curr = curr->next;
    }
    if (prev != lastMetadata){
        cout << "last is fucked" <<endl;
    }
    cout << "MMap: " << endl;
}*/

void* smalloc(size_t size) {

    if(size == 0 || size > 100000000) return nullptr;

    /* first try to find a free block in the list of allocated memories: */
    MallocMetadata* foundBlock = find_free_block(size);
    if(foundBlock){
        /* return the address of the first byte in a block excluding metadata: */
        foundBlock->isFree = false;
        return (void*)((char*)foundBlock + META_SIZE);
    }

    /* A free block wasnt found in the list, need to allocate a new block upon the current program break: */
    void* currProgBreak = sbrk(0);
    MallocMetadata* currentMetadata = (MallocMetadata*) currProgBreak;

    /*Allocating space for metadata and requested memory: */
    void* newProgBreak = sbrk(META_SIZE + size);
    if(newProgBreak == (void*)-1){
        return nullptr;
    }

    /* setting relevant values in fields of metadata for current memory allocation: */
    currentMetadata->size = size;
    currentMetadata->isFree = false;
    currentMetadata->next = nullptr;
    currentMetadata->prev = lastMetadata;

    /*Initiating the first node in memory list: */
    if(!memList){
        memList = currentMetadata;
    }
        /* need to set pointer 'next' in metadata of previous allocation */
    else {
        lastMetadata->next = currentMetadata;
    }

    /* updating the pointer to the last metadata: */
    lastMetadata = currentMetadata;

    /* returning an address of the first byte of allocated memory excluding metadata: */
    return (void*)((char*)currProgBreak + META_SIZE);
}

void* scalloc(size_t num, size_t size){
    size_t totalSize = num*size;
    void* allocatedMem = smalloc(totalSize);
    if(!allocatedMem) return nullptr;
    std::memset(allocatedMem, 0, totalSize);
    return allocatedMem;
}

void* srealloc(void* oldp, size_t size){
    void* allocatedMem = nullptr;
    if(!oldp){
        allocatedMem = smalloc(size);
        return allocatedMem;
    }

    MallocMetadata* oldMetadata = (MallocMetadata*)((char*)oldp - META_SIZE);
    size_t oldSize = oldMetadata->size;
    if(size <= oldSize && size > 0 ) return oldp;

    allocatedMem = smalloc(size);
    if(!allocatedMem) return nullptr;

    std::memcpy(allocatedMem, oldp, oldSize);
    oldMetadata->isFree = true;
    return allocatedMem;
}

void sfree(void* p){
    if(!p) return;
    MallocMetadata* metadata = (MallocMetadata*)((char*)p - META_SIZE);
    if(metadata->isFree) return;
    metadata->isFree = true;
}


