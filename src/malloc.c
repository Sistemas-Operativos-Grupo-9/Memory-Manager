#include "malloc.h"
#include "block_list.h"


BlockList free_blocks;
BlockList reserved_blocks;

void *our_malloc(size_t byte_count) {
	return (void *)4;
}


void free(void *buffer) {

}

