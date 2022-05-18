#pragma once
#include <stdlib.h>

typedef struct memState
{
    size_t totalMemory; // in bytes
    size_t heapStart; // address
    size_t usedMemory; // in bytes
    size_t fragmentsAmount;
} MemoryState;


// Allocates 'byteCount' bytes and returns it's memory location.
void *ourMalloc(size_t byteCount);

// Frees a previously allocated buffer.
void ourFree(void *memPtr);

// Copies into mState the relevan info about the current state of the heap
void getMemoryState(MemoryState *mState);
