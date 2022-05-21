#include "CuTest.h"
#include <stdio.h>

#include "memory_manager.h"
#include "memory_manager_test.h"
#include "test_list.h"

void runSuite(CuSuite *suiteToRun);

void runAllTests() {
	printf("Initializing tests\n");
	runSuite(getListSuite());
	runSuite(getMallocSuite());
	runSuite(getFreeSuite());
	runSuite(getMemStateSuite());
}

void runSuite(CuSuite *suiteToRun) {
	CuString *output = CuStringNew();
	CuSuite *suite = CuSuiteNew();

	CuSuiteAddSuite(suite, suiteToRun);

	CuSuiteRun(suite);
	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	printf("%s\n", output->buffer);

	CuSuiteDelete(suite);
	CuStringDelete(output);
}

int main() {
	runAllTests();
	return 0;
}
