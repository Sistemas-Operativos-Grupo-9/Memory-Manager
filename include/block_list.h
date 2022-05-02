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
	size_t (*get_sort_key)(Block *);
} BlockList;

size_t block_get_size(Block *block);
void *block_get_start(Block *block);

void list_initialize(BlockList *list, size_t (*get_sort_key)(Block *));

int list_find_by_key(BlockList *list, size_t key);
int list_find_by_key_or_higher(BlockList *list, size_t key);

Block list_get_block(BlockList *list, size_t index);
void list_add_block(BlockList *list, Block block);
void list_remove(BlockList *list, size_t index);
