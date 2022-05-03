#include "CuTest.h"
#include "malloc.h"
#include <stdio.h>

#include "test_list.h"

void run_all_tests() {
	CuString *output = CuStringNew();
	CuSuite *suite = CuSuiteNew();

	CuSuiteAddSuite(suite, get_list_suite());

	CuSuiteRun(suite);
	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	printf("%s\n", output->buffer);

	CuSuiteDelete(suite);
	CuStringDelete(output);
}

int main() {
	run_all_tests();
	return 0;
}
