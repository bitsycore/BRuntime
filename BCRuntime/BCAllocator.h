#ifndef BCRUNTIME_BCALLOCATOR_H
#define BCRUNTIME_BCALLOCATOR_H

#include "BCTypes.h"

#include <stddef.h>

typedef struct BCAllocator {
	void* (* alloc)(size_t size, const void* ctx);
	void (* free)(void* ptr, const void* ctx);
	void* context;
} BCAllocator;

void* BCAllocatorAlloc(BCAllocatorRef allocator, size_t size);
void* BCAllocatorRealloc(BCAllocatorRef allocator, void* ptr, size_t oldSize, size_t newSize);
void BCAllocatorFree(BCAllocatorRef allocator, void* ptr);

BCAllocatorRef BCAllocatorGetDefault();
#define BCObjectGetAllocator(obj) ( BC_FLAG_HAS((obj)->flags, BC_OBJECT_FLAG_NON_DEFAULT_ALLOCATOR) ? (BCAllocatorRef)( (char*)(obj) - sizeof(BCAllocatorRef) ) : BCAllocatorGetDefault() )

#endif //BCRUNTIME_BCALLOCATOR_H