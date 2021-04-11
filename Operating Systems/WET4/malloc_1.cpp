//
// Created by student on 6/20/20.
//

#include <unistd.h>
#include <iostream>
void* smalloc(size_t size){
    if(size == 0 || size > 100000000) return nullptr;

    void* addr = sbrk(size);
    if(addr == (void*)-1){
        return nullptr;
    }
    return addr;
}