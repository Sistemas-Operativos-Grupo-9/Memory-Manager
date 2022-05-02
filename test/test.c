#include <stdio.h>
#include "malloc.h"




int main() {
	char *a = our_malloc(100);
	printf("TEST %llu\n", (unsigned long long)a);
	return 0;
}
