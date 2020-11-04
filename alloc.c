// Same allocator, written in C rather than asm
// Assumptions:
// - page size is 4096 bytes
// - signed integers use two's complement
#include <stdlib.h>
#include <sys/mman.h>

#include "asmalloc.h"

enum { PAGE = 4096 };

struct block {
	size_t size; // Block size in multiples of PAGE
	char *alloc; // Pointer to next unallocated element
	void **free; // Pointer to freed element. Deref to get the next value of free
	void *next;  // Pointer to next block
};

// Current blocks for each pool
struct block *_64 = NULL; // <= 64 bytes
struct block *_1k = NULL; // <= 1024 bytes
struct block *_lg = NULL; // Large

// Allocate n pages and return a pointer to the metadata at the end
struct block *_newblock(size_t n, struct block *next) {
	char *pages = mmap(NULL, n * PAGE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	struct block *block = (struct block *)(pages + n * PAGE) - 1;
	block->size = n;
	block->alloc = pages;
	block->next = next;
	return block;
}

// Free the pages containing a block
void _delblock(struct block *block) {
	size_t size = block->size * PAGE;
	char *pages = (char *)(block + 1) - size;
	munmap(pages, size);
}

// Allocate within a block
void *_alloc(struct block **block, size_t block_size) {
	if (!*block) *block = _newblock(8, NULL);
	struct block *blk = *block;

	// Try to allocate from the free list
	if (blk->free) {
		void *mem = blk->free;
		blk->free = *blk->free;
		return mem;
	}

	// Try to allocate a new element
	if (blk->alloc + block_size > (char *)blk) {
		// No space left, make a new block
		*block = blk = _newblock(8, blk);
	}
	void *mem = blk->alloc;
	blk->alloc += block_size;
	return mem;
}

// Allocate some memory
void *asmalloc(size_t size) {
	if (size <= 64) {
		return _alloc(&_64, 64);
	} else if (size <= 1024) {
		return _alloc(&_1k, 1024);
	} else {
		size += sizeof *_lg;
		size = (size - 1) / PAGE + 1; // Divide, rounding up
		_lg = _newblock(size, _lg);
		return _lg->alloc;
	}
}
