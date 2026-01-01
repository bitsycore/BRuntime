#ifndef BCORE_ALLOCATOR_H
#define BCORE_ALLOCATOR_H

#include "../BC_Types.h"

#include <string.h>

typedef struct BC_Allocator {
	void* (*alloc)(size_t size, const void* ctx);
	void (*free)(void* ptr, const void* ctx);
	void* context;
} BC_Allocator;

extern const BC_AllocatorRef kBC_AllocatorRefSystem;

void* BC_AllocatorAlloc(BC_AllocatorRef allocator, size_t size);
void* BC_AllocatorRealloc(BC_AllocatorRef allocator, void* ptr, size_t oldSize, size_t newSize);
void BC_AllocatorFree(BC_AllocatorRef allocator, void* ptr);

void BC_AllocatorSetDefault(BC_AllocatorRef allocator);
BC_AllocatorRef BC_AllocatorGetDefault();

#endif //BCORE_ALLOCATOR_H
