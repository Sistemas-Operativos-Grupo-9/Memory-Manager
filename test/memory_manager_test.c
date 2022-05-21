#include "memory_manager.h"
#include <stdio.h>
#include <stdlib.h>

#include "memory_manager_test.h"

#define DEFAULT_HEAP_SIZE 1000

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

void basicFreeTest(CuTest *tc) {}

void basicMemStateTest(CuTest *tc) {}

void testFree_Overflow(CuTest *tc) {}

void testMalloc_AllocatedSize(CuTest *tc) {}

void testFree_InvalidUsage(CuTest *tc) {}
void testMalloc_InvalidUsage(CuTest *tc) {}

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
	SUITE_ADD_TEST(suite, testFree_Overflow);
	SUITE_ADD_TEST(suite, testFree_InvalidUsage);

	return suite;
}
CuSuite *getMemStateSuite() {
	CuSuite *suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, basicMemStateTest);

	return suite;
}
