#include "block_list.h"


size_t block_get_size(Block *block) {
	return block->size;
}
void *block_get_start(Block *block) {
	return block->start;
}

void list_initialize(BlockList *list, size_t (*get_sort_key)(Block *)) {
	list->count = 0;
	list->get_sort_key = get_sort_key;
}

int list_find_by_key(BlockList *list, size_t key) {
	return 0;
}

int list_find_by_key_or_higher(BlockList *list, size_t key) {
	return 0;
}

Block list_get_block(BlockList *list, size_t index) {
	return list->blocks[index];
}

void list_add_block(BlockList *list, Block block) {
}

void list_remove(BlockList *list, size_t index) {
}
