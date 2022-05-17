#include "block_list.h"

size_t blockGetSize(Block *block) { return block->size; }
void *blockGetStart(Block *block) { return block->start; }

void listInitialize(BlockList *list, size_t (*getSortKey)(Block *)) {
	list->count = 0;
	list->getSortKey = getSortKey;
}

int listFindByKey(BlockList *list, size_t key) {
	int i = listFindByKeyOrHigher(list, key);
	Block foundBlock = listGetBlock(list, i);
	if (i != -1 && list->getSortKey(&foundBlock) == key) {
		return i;
	}
	return -1;
}

int listFindByKeyOrHigher(BlockList *list, size_t key) {
	int start = 0, end = list->count - 1;
	while (start < end) {
		size_t middle = (start + end) / 2;
		Block middleBlock = listGetBlock(list, middle);
		size_t middleKey = list->getSortKey(&middleBlock);
		if (middleKey < key) {
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

Block listGetBlock(BlockList *list, size_t index) {
	return list->blocks[index];
}

void listMoveSlice(BlockList *list, size_t dest, size_t source, size_t size) {
	if (source > dest)
		for (int i = 0; i < size; i++)
			list->blocks[dest + i] = list->blocks[source + i];
	else if (source < dest)
		for (int i = size - 1; i >= 0; i--)
			list->blocks[dest + i] = list->blocks[source + i];
}

void listAddBlock(BlockList *list, Block block) {
	int insertIndex = listFindByKeyOrHigher(list, list->getSortKey(&block));
	if (insertIndex == -1) {
		insertIndex = list->count;
	}
	listMoveSlice(list, insertIndex + 1, insertIndex,
	              list->count - insertIndex);
	list->blocks[insertIndex] = block;
	list->count++;
}

void listRemove(BlockList *list, size_t index) {}
