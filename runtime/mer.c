#include <stdio.h>
#include <stdlib.h>
#define WORD_SIZE 8


extern int _c0_main();

/* The main function, which calls _c0_main */
int main() {
	printf("%d\n", _c0_main());
	exit(0);
}


void assert(int b) {
	if (!b) abort();
}


void* _c0_alloc(size_t size) {
	return malloc(size);
}


void* _c0_alloc_array(size_t size, size_t n) {
	void* ptr = calloc(n+1, size);
	*(int*)(ptr + size - WORD_SIZE) = n;
	return ptr + size;
}


void _c0_bounds_check(void* A, int i) {
	int n = *(int*)(A-WORD_SIZE);
	if (i < 0 || i >= n)
		exit(12);  // -12 = SIGUSR2?
}
