#ifndef BCRUNTIME_BCALLOCATOR_H
#define BCRUNTIME_BCALLOCATOR_H

#include "BCTypes.h"

#include <string.h>

typedef struct BCAllocator {
	void* (* alloc)(size_t size, const void* ctx);
	void (* free)(void* ptr, const void* ctx);
	void* context;
} BCAllocator;

void* BCAllocatorAlloc(BCAllocatorRef allocator, size_t size);
void* BCAllocatorRealloc(BCAllocatorRef allocator, void* ptr, size_t oldSize, size_t newSize);
void BCAllocatorFree(BCAllocatorRef allocator, void* ptr);

BCAllocatorRef BCAllocatorGetDefault();

#endif //BCRUNTIME_BCALLOCATOR_H