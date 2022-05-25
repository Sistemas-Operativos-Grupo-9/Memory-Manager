#ifdef OUR_MM

#include "memory_manager.h"
#include "block_list.h"

static BlockList freeList = {0};

static MemoryState internalMemState = {0};

size_t listGetSortKey(Block *block) { return block->size; }

/**
 * @param start first memory address of the heap
 * @param end last memory address of the heap
 */
void memoryManagerInitialize(size_t start, size_t end) {
	Block *firstBlock = (Block *)start;
	size_t heapBytes = end - start;

	blockInitialize(firstBlock, heapBytes / sizeof(Block));

	listInitialize(&freeList, listGetSortKey);
	listAddBlock(&freeList, firstBlock);

	/*
	TODO:
	* should the total memory be the heap size or the
	* effective memory available for use ( ceil(heapBytes /
	* sizeof(Block)) * sizeof(Block) ) ????
	*/
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
	if (bytes == 0 || bytes > internalMemState.totalMemory) {
		return NULL;
	}

	size_t mUnits = computeMemoryUnits(bytes);

	size_t blockIndex = listFindByKeyOrHigher(&freeList, mUnits);
	if (blockIndex == -1) {
		// No fitting free blocks
		return NULL;
	}

	Block *header = listGetBlock(&freeList, blockIndex);
	if (header == NULL) {
		return NULL;
	}
	if (blockGetSize(header) > mUnits) {
		header = blockDivide(header, mUnits);
	} else {
		listRemove(&freeList, blockIndex);
	}

	header->isAllocated = true;
	internalMemState.usedMemory += blockGetSize(header) * sizeof(Block);
	return header + 1;
}

void ourFree(void *memPtr) {
	// TODO: should free throw errors? how?
	Block *header = ((Block *)memPtr) - 1;

	if (!blockIsValid(header)) {
		//? Error
		return;
	}
	if (!header->isAllocated) {
		//? Error
		return;
	}

	//? Maybe coallesce memory

	listAddBlock(&freeList, header);
	header->isAllocated = false;

	internalMemState.usedMemory -= blockGetSize(header) * sizeof(Block);
}

void getMemoryState(MemoryState *mState) {
	mState->fragmentsAmount = listGetSize(&freeList);
	mState->heapStart = internalMemState.heapStart;
	mState->totalMemory = internalMemState.totalMemory;
	mState->usedMemory = internalMemState.usedMemory;
}

#endif