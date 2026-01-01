#include "BC_Allocator.h"

#include "BC_Memory.h"
#include "../BC_Keywords.h"

#include <string.h>

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

static BC_Allocator const kBC_AllocatorSystem = {AllocatorDefaultAlloc, AllocatorDefaultFree, NULL};
const BC_AllocatorRef kBC_AllocatorRefSystem = (BC_AllocatorRef)&kBC_AllocatorSystem;

BC_TLS BC_AllocatorRef gBC_AllocatorDefault = kBC_AllocatorRefSystem;

// =========================================================
// MARK: Public
// =========================================================

void* BC_AllocatorAlloc(BC_AllocatorRef allocator, const size_t size) {
	if (!allocator) allocator = kBC_AllocatorRefSystem;
	return allocator->alloc(size, allocator->context);
}

void* BC_AllocatorRealloc(const BC_AllocatorRef allocator, void* ptr, const size_t oldSize, const size_t newSize) {
	char* newBuffer = BC_AllocatorAlloc(allocator, newSize);
	if (!newBuffer) {
		fprintf(stderr, "BC_AllocatorRealloc: Failed to allocate buffer\n");
		return NULL;
	}
	memcpy(newBuffer, ptr, oldSize);
	BC_AllocatorFree(allocator, ptr);
	return newBuffer;
}

void BC_AllocatorFree(BC_AllocatorRef allocator, void* ptr) {
	if (!allocator) allocator = kBC_AllocatorRefSystem;
	allocator->free(ptr, allocator->context);
}

BC_AllocatorRef BC_AllocatorGetDefault() {
	return gBC_AllocatorDefault;
}

void BC_AllocatorSetDefault(const BC_AllocatorRef allocator) {
	gBC_AllocatorDefault = allocator ? allocator : kBC_AllocatorRefSystem;
}
