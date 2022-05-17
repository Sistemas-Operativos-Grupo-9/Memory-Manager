#pragma once
#include <stdlib.h>

// Allocates 'byte_count' bytes and returns it's memory location.
void *ourMalloc(size_t byte_count);

// Frees a previously allocated buffer.
void ourFree(void *buffer);


