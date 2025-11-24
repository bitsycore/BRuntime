#include "BCAllocator.h"

#include "Utilities/BCMemory.h"

// =========================================================
// MARK: Allocator
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