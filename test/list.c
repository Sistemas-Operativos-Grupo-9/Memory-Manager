#include "test_list.h"
#include "block_list.h"

void test_list(CuTest *tc) {
	BlockList list;
	/*list_initialize(&list, );*/
}



CuSuite *get_list_suite() {
	CuSuite *suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, test_list);
	return suite;
}
