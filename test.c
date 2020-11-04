#include <stdio.h>

#include "asmalloc.h"

int main() {
	printf("%p\n", asmalloc(1));
	printf("%p\n", asmalloc(16));
	printf("%p\n", asmalloc(64));

	printf("%p\n", asmalloc(65));
	printf("%p\n", asmalloc(1000));
	printf("%p\n", asmalloc(1024));

	printf("%p\n", asmalloc(1025));
	printf("%p\n", asmalloc(4000));
	printf("%p\n", asmalloc(4064));
	printf("%p\n", asmalloc(4065));
	printf("%p\n", asmalloc(4096));
	printf("%p\n", asmalloc(100000)); // 100k

	return 0;
}
