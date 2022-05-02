#include "block_list.h"
#include "test_list.h"

void test_list(CuTest *tc) {
	BlockList list;
	list_initialize(&list, block_get_size);
	Block expected_block = {.size = 100, .start = (void *)10};
	list_add_block(&list, expected_block);

	Block b = list_get_block(&list, 0);
	CuAssertTrue(tc, b.start == expected_block.start);
	CuAssertTrue(tc, b.size == expected_block.size);
}

CuSuite *get_list_suite() {
	CuSuite *suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, test_list);
	return suite;
}
