#include "block_list.h"

size_t block_get_size(Block *block) { return block->size; }
void *block_get_start(Block *block) { return block->start; }

void list_initialize(BlockList *list, size_t (*get_sort_key)(Block *)) {
	list->count = 0;
	list->get_sort_key = get_sort_key;
}

int list_find_by_key(BlockList *list, size_t key) {
	int i = list_find_by_key_or_higher(list, key);
	Block found_block = list_get_block(list, i);
	if (i != -1 && list->get_sort_key(&found_block) == key) {
		return i;
	}
	return -1;
}

int list_find_by_key_or_higher(BlockList *list, size_t key) {
	int start = 0, end = list->count - 1;
	while (start < end) {
		size_t middle = (start + end) / 2;
		Block middle_block = list_get_block(list, middle);
		size_t middle_key = list->get_sort_key(&middle_block);
		if (middle_key < key) {
			start = middle + 1;
		} else {
			end = middle;
		}
	}
	if (start == list->count) {
		return -1;
	}
	return start;
}

Block list_get_block(BlockList *list, size_t index) {
	return list->blocks[index];
}

void list_move_slice(BlockList *list, size_t dest, size_t source, size_t size) {
	if (source > dest)
		for (int i = 0; i < size; i++)
			list->blocks[dest + i] = list->blocks[source + i];
	else if (source < dest)
		for (int i = size - 1; i >= 0; i--)
			list->blocks[dest + i] = list->blocks[source + i];
}

void list_add_block(BlockList *list, Block block) {
	int insert_index =
	    list_find_by_key_or_higher(list, list->get_sort_key(&block));
	if (insert_index == -1) {
		insert_index = list->count;
	}
	list_move_slice(list, insert_index + 1, insert_index,
	                list->count - insert_index);
	list->blocks[insert_index] = block;
	list->count++;
}

void list_remove(BlockList *list, size_t index) {
}
