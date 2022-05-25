#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

size_t getUniform(size_t max);
bool memcheck(void *start, uint8_t value, size_t size);