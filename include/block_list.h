#pragma once
#include <stdbool.h>
#include <stdlib.h>

#define MAX_LIST_SIZE 64

// Each block serves as a header for a memory segment, with info about it
typedef struct block {
	size_t size;         // size of the adyacent memory segment, in blocks
	size_t segmentStart; // memory address of the memory segment's start. This
	                     // is used to check for validity of the block
} Block;

typedef struct BlockList {
	size_t count;
	Block *blocks[MAX_LIST_SIZE];
	size_t (*getSortKey)(Block *);
} BlockList;

size_t blockGetSize(Block *block);
void blockInitialize(Block *block, size_t size);
Block *blockDivide(Block *oldBlock, size_t newBlockSize);
bool blockIsValid(Block *block);

void listInitialize(BlockList *list, size_t (*getSortKey)(Block *));

size_t listFindByKey(BlockList *list, size_t key);
size_t listFindByKeyOrHigher(BlockList *list, size_t key);

bool listIsEmpty(BlockList *list);

size_t listGetSize(BlockList *list);

Block *listGetBlock(BlockList *list, size_t index);
void listAddBlock(BlockList *list, Block *block);
void listRemove(BlockList *list, size_t index);
