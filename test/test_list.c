#include "test_list.h"
#include "block_list.h"

Block *addBlockWithSize(BlockList *list, size_t size) {
	Block *block = malloc(sizeof(Block) * size);
	block->size = size;
	block->segmentStart = (size_t)(block + 1);
	listAddBlock(list, block);
	return block;
}

void testSimpleAdd(CuTest *tc) {
	BlockList list;
	listInitialize(&list, blockGetSize);
	Block *expected_block = addBlockWithSize(&list, 3);

	Block *b = listGetBlock(&list, 0);
	CuAssertTrue(tc, b->segmentStart == expected_block->segmentStart);
	CuAssertTrue(tc, b->size == expected_block->size);
	free(expected_block);
}

void testFindBySize(CuTest *tc) {
	BlockList list;
	listInitialize(&list, blockGetSize);

	Block *freeList[50];
	size_t freeListCount = 0;

	freeList[freeListCount++] = addBlockWithSize(&list, 10);
	freeList[freeListCount++] = addBlockWithSize(&list, 12);
	freeList[freeListCount++] = addBlockWithSize(&list, 15);

	Block *expected_block = addBlockWithSize(&list, 13);

	freeList[freeListCount++] = addBlockWithSize(&list, 20);
	freeList[freeListCount++] = addBlockWithSize(&list, 23);
	freeList[freeListCount++] = addBlockWithSize(&list, 30);

	int index;

	index = listFindByKey(&list, 13);
	CuAssertTrue(tc, index >= 0);
	// freeListCount contains the amount of elements minus one
	CuAssertTrue(tc, freeListCount >= index);

	Block *b = listGetBlock(&list, index);
	CuAssertPtrEquals(tc, (int *)expected_block->segmentStart,
	                  (int *)b->segmentStart);
	CuAssertIntEquals(tc, (int)expected_block->size, (int)b->size);

	index = listFindByKeyOrHigher(&list, 5);
	CuAssertTrue(tc, index >= 0);
	// freeListCount contains the amount of elements minus one
	CuAssertTrue(tc, freeListCount >= index);

	b = listGetBlock(&list, index);
	CuAssertIntEquals(tc, 10, (int)b->size);

	for (size_t i = 0; i < freeListCount; i++) {
		free(freeList[i]);
	}
	free(expected_block);
}

void testFindByKeyWithNoValidElements(CuTest *tc) {
	BlockList list;
	BlockList *listPtr = &list;
	listInitialize(listPtr, blockGetSize);

	Block *freeList[50];
	size_t freeListCount = 0;

	freeList[freeListCount++] = addBlockWithSize(listPtr, 10);
	freeList[freeListCount++] = addBlockWithSize(listPtr, 20);
	freeList[freeListCount++] = addBlockWithSize(listPtr, 30);

	int expectedIndexForError = -1;
	int index;

	index = listFindByKey(listPtr, 5);
	CuAssertIntEquals(tc, expectedIndexForError, index);

	index = listFindByKey(listPtr, 35);
	CuAssertIntEquals(tc, expectedIndexForError, index);

	index = listFindByKeyOrHigher(listPtr, 35);
	CuAssertIntEquals(tc, expectedIndexForError, index);

	for (size_t i = 0; i < freeListCount; i++) {
		free(freeList[i]);
	}
}

CuSuite *getListSuite() {
	CuSuite *suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, testSimpleAdd);
	SUITE_ADD_TEST(suite, testFindBySize);
	SUITE_ADD_TEST(suite, testFindByKeyWithNoValidElements);
	return suite;
}
