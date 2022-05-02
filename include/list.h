#pragma once
#include <stdlib.h>


typedef struct Block {
  size_t start;
  size_t size;
} Block;

typedef struct List {
  size_t count;
  Block blocks[64];
} List;




