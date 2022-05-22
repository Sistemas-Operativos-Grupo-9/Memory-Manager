#include "test_list.h"
#include "block_list.h"

// Basic Test Template:
// ---------------------------------
// void testSomething(CuTest *tc){
// 	BlockList list;
// 	BlockList *listPtr = &list;
// 	listInitialize(listPtr, blockGetSize);

// 	Block *freeList[50];
// 	size_t freeListCount = 0;

// 	for (size_t i = 0; i < freeListCount; i++) {
// 		free(freeList[i]);
// 	}
// }

// Utils
// -----------------------------------------------------------------------
Block *newBlock(size_t size) {
	Block *block = malloc(sizeof(Block) * size);
	block->size = size;
	block->segmentStart = (size_t)(block + 1);
	return block;
}

Block *addBlockWithSize(BlockList *list, size_t size) {
	Block *block = newBlock(size);
	listAddBlock(list, block);
	return block;
}

// Tests
// -----------------------------------------------------------------------
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

void testRemove(CuTest *tc) {
	BlockList list;
	BlockList *listPtr = &list;
	listInitialize(listPtr, blockGetSize);

	Block *freeList[50];
	size_t freeListCount = 0;

	int itemCount;

	freeList[freeListCount++] = addBlockWithSize(listPtr, 1);
	freeList[freeListCount++] = addBlockWithSize(listPtr, 2);
	freeList[freeListCount++] = addBlockWithSize(listPtr, 3);
	freeList[freeListCount++] = addBlockWithSize(listPtr, 4);

	itemCount = freeListCount;

	CuAssertIntEquals(tc, itemCount, listGetSize(listPtr));

	int index;
	int expectedIndexForError = -1;

	index = listFindByKey(listPtr, 1);
	CuAssertTrue(tc, index != expectedIndexForError);

	listRemove(listPtr, index);
	itemCount--;
	CuAssertIntEquals(tc, itemCount, listGetSize(listPtr));

	index = listFindByKey(listPtr, 1);
	CuAssertIntEquals(tc, expectedIndexForError, index);

	// removing an invalid index should keep the list intact
	listRemove(listPtr, -1);
	listRemove(listPtr, -10);
	listRemove(listPtr, 100);
	listRemove(listPtr, itemCount);
	CuAssertIntEquals(tc, itemCount, listGetSize(listPtr));

	for (int i = 0; i < itemCount; i++) {
		listRemove(listPtr, 0); // there will always be a first item
	}
	itemCount = 0;
	CuAssertIntEquals(tc, 0, listGetSize(listPtr));

	// removing from an empty list should do nothing
	listRemove(listPtr, 0);
	listRemove(listPtr, -1);
	listRemove(listPtr, 1);
	listRemove(listPtr, 10);
	CuAssertIntEquals(tc, 0, listGetSize(listPtr));

	for (size_t i = 0; i < freeListCount; i++) {
		free(freeList[i]);
	}
}

void testBlockIsValid(CuTest *tc) {
	BlockList list;
	BlockList *listPtr = &list;
	listInitialize(listPtr, blockGetSize);

	Block *freeList[50];
	size_t freeListCount = 0;

	freeList[freeListCount++] = newBlock(2);
	freeList[freeListCount++] = newBlock(100);
	freeList[freeListCount++] = newBlock(53);

	CuAssertTrue(tc, blockIsValid(freeList[0]));
	CuAssertTrue(tc, blockIsValid(freeList[1]));
	CuAssertTrue(tc, blockIsValid(freeList[2]));

	// negative-size block
	Block *invalidBlock_1 = malloc(sizeof(Block) * 2);
	freeList[freeListCount++] = invalidBlock_1;
	invalidBlock_1->size = -100;

	// uninitialized block
	Block *invalidBlock_2 = malloc(sizeof(Block) * 2);
	freeList[freeListCount++] = invalidBlock_2;

	// purposefully wron segmentStart address
	Block *invalidBlock_3 = malloc(sizeof(Block) * 2);
	freeList[freeListCount++] = invalidBlock_3;
	invalidBlock_3->size = 2;
	invalidBlock_3->segmentStart = (size_t)NULL;

	CuAssertTrue(tc, !blockIsValid(invalidBlock_1));
	CuAssertTrue(tc, !blockIsValid(invalidBlock_2));
	CuAssertTrue(tc, !blockIsValid(invalidBlock_3));

	for (size_t i = 0; i < freeListCount; i++) {
		free(freeList[i]);
	}
}

void testBlockDivide(CuTest *tc) {
	BlockList list;
	BlockList *listPtr = &list;
	listInitialize(listPtr, blockGetSize);

	Block *block = newBlock(100);
	Block *dividedBlock = blockDivide(block, 50);
	CuAssertIntEquals(tc, 50, blockGetSize(block));
	CuAssertIntEquals(tc, 50, blockGetSize(dividedBlock));

	Block *evenMoreDividedBlock = blockDivide(block, 10);
	CuAssertIntEquals(tc, 40, blockGetSize(block));
	CuAssertIntEquals(tc, 10, blockGetSize(evenMoreDividedBlock));
	CuAssertTrue(tc, dividedBlock != evenMoreDividedBlock);

	Block *anotherEvenMoreDividedBlock = blockDivide(dividedBlock, 20);
	CuAssertIntEquals(tc, 30, blockGetSize(dividedBlock));
	CuAssertIntEquals(tc, 20, blockGetSize(anotherEvenMoreDividedBlock));

	// There are no test for invalid values because the function doesn't need to
	// check for them since it is always used with already checked arguments

	free(block);
}

// Suite
// -----------------------------------------------------------------------
CuSuite *getListSuite() {
	CuSuite *suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, testSimpleAdd);
	SUITE_ADD_TEST(suite, testFindBySize);
	SUITE_ADD_TEST(suite, testFindByKeyWithNoValidElements);
	SUITE_ADD_TEST(suite, testRemove);
	SUITE_ADD_TEST(suite, testBlockIsValid);
	SUITE_ADD_TEST(suite, testBlockDivide);

	return suite;
}
