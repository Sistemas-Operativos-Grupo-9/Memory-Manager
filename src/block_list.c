#include "block_list.h"

size_t blockGetSize(Block *block) { return block->size; }
void blockSetSize(Block *block, size_t size) { block->size = size; }
// void *blockGetStart(Block *block) { return block->start; }

Block *blockDivide(Block *oldBlock, size_t newBlockSize) {
	oldBlock->size -= newBlockSize;
	Block *newBlock = oldBlock + blockGetSize(oldBlock);
	newBlock->size = newBlockSize;
	return newBlock;
}

void listInitialize(BlockList *list, size_t (*getSortKey)(Block *)) {
	list->count = 0;
	list->getSortKey = getSortKey;
}

size_t listFindByKey(BlockList *list, size_t key) {
	size_t i = listFindByKeyOrHigher(list, key);
	Block *foundBlock = listGetBlock(list, i);
	if (i != -1 && list->getSortKey(foundBlock) == key) {
		return i;
	}
	return -1;
}

/**
 * @brief
 *
 * @return index of the found element, -1 if no appropiate element was found
 */
size_t listFindByKeyOrHigher(BlockList *list, size_t key) {
	size_t start = 0, end = list->count - 1;
	while (start < end) {
		size_t middle = (start + end) / 2;
		Block *middleBlock = listGetBlock(list, middle);
		size_t middleKey = list->getSortKey(middleBlock);
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

Block *listGetBlock(BlockList *list, size_t index) {
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

void listAddBlock(BlockList *list, Block *block) {
	int insertIndex = listFindByKeyOrHigher(list, list->getSortKey(block));
	if (insertIndex == -1) {
		insertIndex = list->count;
	}
	listMoveSlice(list, insertIndex + 1, insertIndex,
	              list->count - insertIndex);
	list->blocks[insertIndex] = block;
	list->count++;
}

void listRemove(BlockList *list, size_t index) {}

bool listIsEmpty(BlockList *list) { return list->count == 0; }

size_t listGetSize(BlockList *list) { return list->count; }
