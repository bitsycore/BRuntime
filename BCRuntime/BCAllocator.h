#ifndef BCRUNTIME_BCALLOCATOR_H
#define BCRUNTIME_BCALLOCATOR_H

#include "BCTypes.h"

#include <stddef.h>

typedef struct BCAllocator {
	void* (* alloc)(size_t size, const void* ctx);
	void (* free)(void* ptr, const void* ctx);
	void* context;
} BCAllocator;

extern BCAllocatorRef const kBCAllocatorDefault;

#define BC_OBJECT_GET_ALLOCATOR(obj) ( BC_FLAG_HAS((obj)->flags, BC_OBJECT_FLAG_NON_DEFAULT_ALLOCATOR) ? (BCAllocatorRef)( (char*)(obj) - sizeof(BCAllocatorRef) ) : kBCAllocatorDefault )

#endif //BCRUNTIME_BCALLOCATOR_H