#include "memory_manager.h"
#include <stdio.h>
#include <stdlib.h>

#include "memory_manager_test.h"

#define DEFAULT_HEAP_SIZE 1001

// Basic Test Template:
// ---------------------------------
// void testSomething(CuTest *tc) {
// 	size_t heapSize = DEFAULT_HEAP_SIZE;
// 	void *heap = malloc(heapSize);
// 	size_t heapStart = (size_t)heap;
// 	size_t heapEnd = (size_t)heap + heapSize;
// 	memoryManagerInitialize(heapStart, heapEnd);

// 	free(heap);
// }

// Utils
// -----------------------------------------------------------------------

// Tests
// -----------------------------------------------------------------------
void basicMallocTest(CuTest *tc) {
	void *heap = malloc(DEFAULT_HEAP_SIZE);
	size_t heapStart = (size_t)heap;
	size_t heapEnd = (size_t)heap + DEFAULT_HEAP_SIZE;
	memoryManagerInitialize(heapStart, heapEnd);

	MemoryState memState;
	getMemoryState(&memState);

	CuAssertTrue(tc, memState.usedMemory == 0);

	void *ptr = ourMalloc(10);

	getMemoryState(&memState);

	size_t mallocSize = memState.usedMemory;

	CuAssertTrue(tc, mallocSize != 0);

	CuAssertPtrNotNullMsg(tc, "pointer to first malloc is null", ptr);

	// testing that data is not overriden
	char *ptr1 = ourMalloc(13);

	char msg[13] = "hello world!";

	for (int i = 0; msg[i] != '\0'; i++) {
		ptr1[i] = msg[i];
	}

	CuAssertStrEquals(tc, msg, ptr1);

	char *ptr2 = ourMalloc(10);
	CuAssertStrEquals(tc, msg, ptr1);

	// malloc a few more times
	int const PTRS_AMOUNT = DEFAULT_HEAP_SIZE / mallocSize - 5;
	void *pointers[PTRS_AMOUNT];
	for (int i = 0; i < PTRS_AMOUNT; i++) {
		pointers[i] = ourMalloc(10);
		CuAssertPtrNotNullMsg(tc, "pointer to malloc is null", pointers[i]);
		CuAssertTrue(tc, heapStart <= (size_t)pointers[i] &&
		                     (size_t)pointers[i] <= heapEnd);
		for (int j = 0; j < i; j++) {
			CuAssertTrue(tc, pointers[i] != pointers[j]);
			// check that 10 bytes were reserved for each ptr
			CuAssertTrue(tc, pointers[i] - pointers[j] >= 10 ||
			                     pointers[j] - pointers[i] >= 10);
		}
	}

	free(heap);
}

void basicFreeTest(CuTest *tc) {
	size_t heapSize = DEFAULT_HEAP_SIZE;
	void *heap = malloc(heapSize);
	size_t heapStart = (size_t)heap;
	size_t heapEnd = (size_t)heap + heapSize;
	memoryManagerInitialize(heapStart, heapEnd);

	MemoryState memoryState;

	int *number = ourMalloc(sizeof(int));
	getMemoryState(&memoryState);
	CuAssertTrue(tc, memoryState.usedMemory > 0);
	ourFree(number);
	getMemoryState(&memoryState);
	CuAssertIntEquals(tc, 0, memoryState.usedMemory);

	int mallocAmount = 10;
	int *ptrs[mallocAmount];
	for (size_t i = 0; i < mallocAmount; i++) {
		ptrs[i] = ourMalloc(sizeof(int));
	}
	getMemoryState(&memoryState);
	CuAssertTrue(tc, memoryState.usedMemory > 0);
	for (size_t i = 0; i < mallocAmount; i++) {
		ourFree(ptrs[i]);
	}
	getMemoryState(&memoryState);
	CuAssertIntEquals(tc, 0, memoryState.usedMemory);

	free(heap);
}

void basicMemStateTest(CuTest *tc) {
	size_t heapSize = DEFAULT_HEAP_SIZE;
	void *heap = malloc(heapSize);
	size_t heapStart = (size_t)heap;
	size_t heapEnd = (size_t)heap + heapSize;
	memoryManagerInitialize(heapStart, heapEnd);

	MemoryState memoryState;
	getMemoryState(&memoryState);

	CuAssertIntEquals(tc, heapSize, memoryState.totalMemory);
	CuAssertIntEquals(tc, 1, memoryState.fragmentsAmount);
	CuAssertIntEquals(tc, 0, memoryState.usedMemory);
	CuAssertPtrEquals(tc, (void *)heapStart, (void *)memoryState.heapStart);

	int mallocAmount = 20;
	int *ptrs[mallocAmount];
	int allocatedPtrsCount = 0;

	ptrs[allocatedPtrsCount++] = ourMalloc(1);
	getMemoryState(&memoryState);
	int minAllocSize = memoryState.usedMemory;
	CuAssertTrue(tc, minAllocSize > 0);

	for (; allocatedPtrsCount < mallocAmount; allocatedPtrsCount++) {
		ptrs[allocatedPtrsCount] = ourMalloc(1);
	}

	getMemoryState(&memoryState);
	CuAssertIntEquals(tc, minAllocSize * allocatedPtrsCount,
	                  memoryState.usedMemory);

	CuAssertIntEquals(tc, 1, memoryState.fragmentsAmount);

	ourFree(ptrs[allocatedPtrsCount - 1]);
	allocatedPtrsCount--;
	getMemoryState(&memoryState);
	CuAssertIntEquals(tc, minAllocSize * allocatedPtrsCount,
	                  memoryState.usedMemory);

	CuAssertIntEquals(tc, 2, memoryState.fragmentsAmount);

	for (; allocatedPtrsCount > 0; allocatedPtrsCount--) {
		ourFree(ptrs[allocatedPtrsCount - 1]);
	}

	getMemoryState(&memoryState);

	CuAssertIntEquals(tc, 0, memoryState.usedMemory);
	// amount of fragments = amount of frees + 1 (<- unused memory block)
	CuAssertIntEquals(tc, mallocAmount + 1, memoryState.fragmentsAmount);

	CuAssertIntEquals(tc, heapSize, memoryState.totalMemory);
	CuAssertPtrEquals(tc, (void *)heapStart, (void *)memoryState.heapStart);

	free(heap);
}

void testFree_Overflow(CuTest *tc) {
	size_t heapSize = DEFAULT_HEAP_SIZE;
	void *heap = malloc(heapSize);
	size_t heapStart = (size_t)heap;
	size_t heapEnd = (size_t)heap + heapSize;
	memoryManagerInitialize(heapStart, heapEnd);

	MemoryState memoryState;
	int *ptr = ourMalloc(1);

	for (size_t i = 0; i < 10; i++) {
		// For now this doesn't throw an error, so it's safe to test
		ourFree(ptr);
	}
	for (size_t i = 0; i < 50; i++) {
		ourFree((void *)(heapStart + i * heapSize / 50));
	}

	getMemoryState(&memoryState);

	CuAssertIntEquals(tc, heapSize, memoryState.totalMemory);
	// amount of fragments = 1 valid free + 1 block of unused space
	CuAssertIntEquals(tc, 2, memoryState.fragmentsAmount);
	CuAssertIntEquals(tc, 0, memoryState.usedMemory);
	CuAssertPtrEquals(tc, (void *)heapStart, (void *)memoryState.heapStart);

	free(heap);
}

void testMalloc_AllocatedSize(CuTest *tc) {
	size_t heapSize = DEFAULT_HEAP_SIZE;
	void *heap = malloc(heapSize);
	size_t heapStart = (size_t)heap;
	size_t heapEnd = (size_t)heap + heapSize;
	memoryManagerInitialize(heapStart, heapEnd);

	MemoryState memoryState;

	int *ptrs[100];
	int mallocCount = 0;

	ptrs[mallocCount++] = ourMalloc(1);
	getMemoryState(&memoryState);
	int mallocSize = memoryState.usedMemory;
	CuAssertTrue(tc, mallocSize > 0);

	// allocate memory until heap is full
	for (; mallocCount < heapSize / mallocSize; mallocCount++) {
		ptrs[mallocCount] = ourMalloc(1);
	}

	getMemoryState(&memoryState);
	CuAssertTrue(tc, memoryState.usedMemory >= heapSize - mallocSize);

	void *fullPtr = ourMalloc(1);
	CuAssertPtrEquals(tc, NULL, fullPtr);

	for (; mallocCount > 0; mallocCount--) {
		ourFree(ptrs[mallocCount - 1]);
	}

	getMemoryState(&memoryState);
	CuAssertIntEquals(tc, 0, memoryState.usedMemory);

	free(heap);
}

void testFree_InvalidUsage(CuTest *tc) {
	size_t heapSize = DEFAULT_HEAP_SIZE;
	void *heap = malloc(heapSize);
	size_t heapStart = (size_t)heap;
	size_t heapEnd = (size_t)heap + heapSize;
	memoryManagerInitialize(heapStart, heapEnd);

	MemoryState memoryState;

	ourFree(heap);

	int mallocAmount = 10;
	int *ptrs[mallocAmount];
	for (size_t i = 0; i < mallocAmount; i++) {
		ptrs[i] = ourMalloc(sizeof(int));
	}

	getMemoryState(&memoryState);
	size_t usedMemory_old = memoryState.usedMemory;

	ourFree(ptrs[1] + 1);

	getMemoryState(&memoryState);
	CuAssertIntEquals(tc, usedMemory_old, memoryState.usedMemory);

	ourFree((int *)((size_t)ptrs[1] + sizeof(int) - 1));

	getMemoryState(&memoryState);
	CuAssertIntEquals(tc, usedMemory_old, memoryState.usedMemory);

	for (size_t i = 0; i < mallocAmount; i++) {
		ourFree(ptrs[i]);
	}

	free(heap);
}
void testMalloc_InvalidUsage(CuTest *tc) {
	size_t heapSize = DEFAULT_HEAP_SIZE;
	void *heap = malloc(heapSize);
	size_t heapStart = (size_t)heap;
	size_t heapEnd = (size_t)heap + heapSize;
	memoryManagerInitialize(heapStart, heapEnd);

	void *ptr = ourMalloc(heapSize + 1);
	CuAssertPtrEquals(tc, NULL, ptr);

	ptr = ourMalloc(-1);
	CuAssertPtrEquals(tc, NULL, ptr);

	ptr = ourMalloc(0);
	CuAssertPtrEquals(tc, NULL, ptr);

	int *validPtr = ourMalloc(2);

	ptr = ourMalloc(heapSize - 1);
	CuAssertPtrEquals(tc, NULL, ptr);

	ourFree(validPtr);

	free(heap);
}

// Suites
// -----------------------------------------------------------------------
CuSuite *getMallocSuite() {
	CuSuite *suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, basicMallocTest);
	SUITE_ADD_TEST(suite, testMalloc_AllocatedSize);
	SUITE_ADD_TEST(suite, testMalloc_InvalidUsage);

	return suite;
}
CuSuite *getFreeSuite() {
	CuSuite *suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, basicFreeTest);
	// SUITE_ADD_TEST(suite, testFree_Overflow);
	SUITE_ADD_TEST(suite, testFree_InvalidUsage);

	return suite;
}
CuSuite *getMemStateSuite() {
	CuSuite *suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, basicMemStateTest);

	return suite;
}
