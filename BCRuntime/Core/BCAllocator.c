#include "BCAllocator.h"

#include <string.h>

#include "../Utilities/BC_Keywords.h"
#include "../Utilities/BC_Memory.h"

// =========================================================
// MARK: Default Allocator
// =========================================================

static void* AllocatorDefaultAlloc(const size_t size, const void* ctx) {
	(void)ctx;
	return BCMalloc(size);
}

static void AllocatorDefaultFree(void* ptr, const void* ctx) {
	(void)ctx;
	BCFree(ptr);
}

static BCAllocator const kBCAllocatorSystem = {AllocatorDefaultAlloc, AllocatorDefaultFree, NULL};
const BCAllocatorRef kBCAllocatorRefSystem = (BCAllocatorRef)&kBCAllocatorSystem;

$TLS BCAllocatorRef gBCAllocatorDefault = kBCAllocatorRefSystem;

// =========================================================
// MARK: Public
// =========================================================

void* BCAllocatorAlloc(BCAllocatorRef allocator, const size_t size) {
	if (!allocator) allocator = kBCAllocatorRefSystem;
	return allocator->alloc(size, allocator->context);
}

void* BCAllocatorRealloc(const BCAllocatorRef allocator, void* ptr, const size_t oldSize, const size_t newSize) {
	char* newBuffer = BCAllocatorAlloc(allocator, newSize);
	if (!newBuffer) {
		fprintf(stderr, "BCAllocatorRealloc: Failed to allocate buffer\n");
		return NULL;
	}
	memcpy(newBuffer, ptr, oldSize);
	BCAllocatorFree(allocator, ptr);
	return newBuffer;
}

void BCAllocatorFree(BCAllocatorRef allocator, void* ptr) {
	if (!allocator) allocator = kBCAllocatorRefSystem;
	allocator->free(ptr, allocator->context);
}

BCAllocatorRef BCAllocatorGetDefault() {
	return gBCAllocatorDefault;
}

void BCAllocatorSetDefault(const BCAllocatorRef allocator) {
	gBCAllocatorDefault = allocator ? allocator : kBCAllocatorRefSystem;
}
