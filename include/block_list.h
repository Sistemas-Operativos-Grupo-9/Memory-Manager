#pragma once
#include <stdlib.h>

#define MAX_LIST_SIZE 64

typedef struct Block {
	void *start;
	size_t size;
} Block;

typedef struct BlockList {
	size_t count;
	Block blocks[MAX_LIST_SIZE];
	size_t (*getSortKey)(Block *);
} BlockList;

size_t blockGetSize(Block *block);
void *blockGetStart(Block *block);

void listInitialize(BlockList *list, size_t (*getSortKey)(Block *));

int listFindByKey(BlockList *list, size_t key);
int listFindByKeyOrHigher(BlockList *list, size_t key);

Block listGetBlock(BlockList *list, size_t index);
void listAddBlock(BlockList *list, Block block);
void listRemove(BlockList *list, size_t index);
