#include "BCAllocator.h"

#include <string.h>

#include "Utilities/BCMemory.h"

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

static BCAllocator _kBCAllocatorDefault = {AllocatorDefaultAlloc, AllocatorDefaultFree, NULL};
BCAllocatorRef const kBCAllocatorDefault = &_kBCAllocatorDefault;

// =========================================================
// MARK: Public
// =========================================================

void* BCAllocatorAlloc(BCAllocatorRef allocator, const size_t size) {
	if (!allocator) allocator = kBCAllocatorDefault;
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
	if (!allocator) allocator = kBCAllocatorDefault;
	allocator->free(ptr, allocator->context);
}

BCAllocatorRef BCAllocatorGetDefault() {
	return kBCAllocatorDefault;
}