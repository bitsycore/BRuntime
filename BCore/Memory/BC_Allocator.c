#include "BC_Allocator.h"

#include "BC_Memory.h"
#include "../BC_Keywords.h"

#include <string.h>

// =========================================================
// MARK: Default Allocator
// =========================================================

static void* IMPL_AllocatorDefaultAlloc(const size_t size, const void* ctx) {
	(void)ctx;
	return BC_Malloc(size);
}

static void IMPL_AllocatorDefaultFree(void* ptr, const void* ctx) {
	(void)ctx;
	BC_Free(ptr);
}

static BC_Allocator const PRIV_kAllocatorSystem = {IMPL_AllocatorDefaultAlloc, IMPL_AllocatorDefaultFree, NULL};
const BC_AllocatorRef kBC_AllocatorRefSystem = (BC_AllocatorRef)&PRIV_kAllocatorSystem;

BC_TLS BC_AllocatorRef PRIV_gAllocatorDefault = kBC_AllocatorRefSystem;

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
	return PRIV_gAllocatorDefault;
}

void BC_AllocatorSetDefault(const BC_AllocatorRef allocator) {
	PRIV_gAllocatorDefault = allocator ? allocator : kBC_AllocatorRefSystem;
}
