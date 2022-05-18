#include "malloc.h"
#include "block_list.h"

static BlockList freeList;

static MemoryState internalMemState;

size_t listGetSortKey(Block *block) { return block->size; }

/**
 * @param start first memory address of the heap
 * @param end last memory address of the heap
 */
void memoryManagerInitialize(size_t start, size_t end) {
	Block *firstBlock = start;
	size_t heapBytes = end - start;

	blockInitialize(firstBlock, heapBytes / sizeof(Block));

	listInitialize(&freeList, listGetSortKey);
	listAddBlock(&freeList, firstBlock);

	internalMemState.totalMemory = heapBytes;
	internalMemState.heapStart = start;
	internalMemState.usedMemory = 0;
	internalMemState.fragmentsAmount = 1;
}

size_t computeMemoryUnits(size_t bytes) {
	size_t blockSize = sizeof(Block);
	size_t segmenSize = bytes + blockSize;
	/*
	 * here ((segmenSize - 1) / blockSize) + 1 will always return an amount of
	 * blocks which is the smallest number above segmentSize/blockSize
	 */
	return ((segmenSize - 1) / blockSize) + 1;
}

void *ourMalloc(size_t bytes) {
	if (bytes <= 0) { //? // TODO: check == 0 case
		return NULL;
	}

	size_t mUnits = computeMemoryUnits(bytes);

	size_t blockIndex = listFindByKeyOrHigher(&freeList, mUnits);
	if (blockIndex == -1) {
		// No fitting free blocks
		return NULL;
	}

	Block *header = listGetBlock(&freeList, blockIndex);
	if (blockGetSize(header) > mUnits) {
		header = blockDivide(header, mUnits);
	} else {
		listRemove(&freeList, blockIndex);
	}

	internalMemState.usedMemory += blockGetSize(header) * sizeof(Block);
	return header + 1;
}

void ourFree(void *memPtr) {
	Block *header = ((Block *)memPtr) - 1;

	if (!blockIsValid(header)) { // TODO: implement blockIsValid
		//? Error
		return;
	}

	//? Maybe coallesce memory

	listAddBlock(&freeList, header);

	internalMemState.fragmentsAmount++; // no coalescence
	internalMemState.usedMemory -= blockGetSize(header) * sizeof(Block);
}

void getMemoryState(MemoryState *mState) {
	mState->fragmentsAmount = internalMemState.fragmentsAmount;
	mState->heapStart = internalMemState.heapStart;
	mState->totalMemory = internalMemState.totalMemory;
	mState->usedMemory = internalMemState.usedMemory;
}
