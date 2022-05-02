#include "block_list.h"
#include "test_list.h"

void test_simple_add(CuTest *tc) {
	BlockList list;
	list_initialize(&list, block_get_size);
	Block expected_block = {.size = 100, .start = (void *)10};
	list_add_block(&list, expected_block);

	Block b = list_get_block(&list, 0);
	CuAssertTrue(tc, b.start == expected_block.start);
	CuAssertTrue(tc, b.size == expected_block.size);
}

void test_find_by_size(CuTest *tc) {
	BlockList list;
	list_initialize(&list, block_get_size);

#define ADD_BLOCK_WITH_SIZE(_size) list_add_block(&list, (Block){.size = _size})
	ADD_BLOCK_WITH_SIZE(20);
	ADD_BLOCK_WITH_SIZE(30);
	ADD_BLOCK_WITH_SIZE(10);
	Block expected_block = {.size = 13, .start = (void *)10};
	list_add_block(&list, expected_block);
	ADD_BLOCK_WITH_SIZE(15);
	ADD_BLOCK_WITH_SIZE(23);
	ADD_BLOCK_WITH_SIZE(12);
#undef ADD_BLOCK_WITH_SIZE

	int i = list_find_by_key(&list, 13);
	CuAssertTrue(tc, i >= 0);
	Block b = list_get_block(&list, i);
	CuAssertTrue(tc, b.start == expected_block.start);
	CuAssertTrue(tc, b.size == expected_block.size);
}

CuSuite *get_list_suite() {
	CuSuite *suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, test_simple_add);
	SUITE_ADD_TEST(suite, test_find_by_size);
	return suite;
}
