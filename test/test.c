#include "CuTest.h"
#include "malloc.h"
#include <stdio.h>

void test_simple(CuTest *tc) {
	char *a = our_malloc(100);
	CuAssertPtrEquals(tc, (void *)7, a);
}

CuSuite *get_suite() {
	CuSuite *suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, test_simple);
	return suite;
}

void run_all_tests() {
	CuString *output = CuStringNew();
	CuSuite *suite = CuSuiteNew();

	CuSuiteAddSuite(suite, get_suite());

	CuSuiteRun(suite);
	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	printf("%s\n", output->buffer);
}

int main() {
	run_all_tests();
	return 0;
}
