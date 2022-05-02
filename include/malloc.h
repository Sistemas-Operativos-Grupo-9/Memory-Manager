#pragma once
#include <stdlib.h>

// Allocates 'byte_count' bytes and returns it's memory location.
void *our_malloc(size_t byte_count);

// Frees a previously allocated buffer.
void free(void *buffer);


