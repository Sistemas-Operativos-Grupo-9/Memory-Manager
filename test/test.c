#include "CuTest.h"
#include <stdio.h>

#include "memory_manager.h"
#include "memory_manager_test.h"
#include "test_list.h"

void runSuite(CuSuite *suiteToRun, char *name);

void runAllTests() {
	printf("Initializing tests\n\n");
	runSuite(getListSuite(), "List Suite");
	runSuite(getMallocSuite(), "Malloc Suite");
	runSuite(getFreeSuite(), "Free suite");
	runSuite(getMemStateSuite(), "Memory State Suite");
}

void runSuite(CuSuite *suiteToRun, char *name) {
	CuString *output = CuStringNew();
	CuSuite *suite = CuSuiteNew();

	CuSuiteAddSuite(suite, suiteToRun);

	printf("/////////////////////////\n");
	// printf("Starting: %s\n", name);
	CuSuiteRun(suite);
	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	printf("Results from %s: \n%s\n", name, output->buffer);

	CuSuiteDelete(suite);
	CuStringDelete(output);
}

int main() {
	runAllTests();
	return 0;
}
