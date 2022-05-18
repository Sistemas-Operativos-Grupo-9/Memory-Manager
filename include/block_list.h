#pragma once
#include <stdlib.h>

#define MAX_LIST_SIZE 64

typedef enum { FALSE = 0, TRUE = 1 } bool; // TODO: move to a stdlib file

// Each block serves as a header for a memory segment, with info about it
typedef union block {
	size_t size; // size of the adyacent memory segment, in blocks
} Block;

typedef struct BlockList {
	size_t count;
	Block *blocks[MAX_LIST_SIZE];
	size_t (*getSortKey)(Block *);
} BlockList;

size_t blockGetSize(Block *block);
void blockSetSize(Block *block, size_t size);
Block *blockDivide(Block *oldBlock, size_t newBlockSize);
// void *blockGetStart(Block *block);

void listInitialize(BlockList *list, size_t (*getSortKey)(Block *));

size_t listFindByKey(BlockList *list, size_t key);
size_t listFindByKeyOrHigher(BlockList *list, size_t key);

bool listIsEmpty(BlockList *list);

size_t listGetSize(BlockList *list);

Block *listGetBlock(BlockList *list, size_t index);
void listAddBlock(BlockList *list, Block *block);
void listRemove(BlockList *list, size_t index);
