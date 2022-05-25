#include "test_utils.h"

static bool seeded = false;
size_t getUniform(size_t max) {
	if (!seeded) {
		srand(time(0));
		seeded = true;
	}
	return max * ((double)rand() / RAND_MAX);
}

bool memcheck(void *start, uint8_t value, size_t size) {
	uint8_t *p = (uint8_t *)start;
	size_t i;

	for (i = 0; i < size; i++, p++)
		if (*p != value)
			return false;

	return true;
}