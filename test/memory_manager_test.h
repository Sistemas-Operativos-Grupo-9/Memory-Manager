#pragma once
#include "CuTest.h"

void basicMallocTest(CuTest *tc);
void basicFreeTest(CuTest *tc);
void basicMemStateTest(CuTest *tc);

void testFree_Overflow(CuTest *tc);

void testMalloc_AllocatedSize(CuTest *tc);

void testFree_InvalidUsage(CuTest *tc);
void testMalloc_InvalidUsage(CuTest *tc);

CuSuite *getMallocSuite();
CuSuite *getFreeSuite();
CuSuite *getMemStateSuite();
CuSuite *getMemoryManagerIntegratedSuite();
