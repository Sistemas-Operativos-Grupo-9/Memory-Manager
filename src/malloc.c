#include "malloc.h"
#include "list.h"


List free_blocks;
List reserved_blocks;

void *our_malloc(size_t byte_count) {
	return (void *)4;
}


void free(void *buffer) {

}

