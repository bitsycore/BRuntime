#include "BC_Arena.h"

#include "BC_Allocator.h"
#include "../BC_Types.h"

#include <stdio.h>

// =========================================================
// MARK: Arena Structure
// =========================================================

typedef struct BC_Arena {
	void* buffer;
	size_t size;
	size_t offset;
	BC_AllocatorRef allocatorRef;  // Allocator for fallback and for freeing
	BC_Allocator allocator;        // The allocator interface for this arena
	BC_bool ownsBuffer;           //
} BC_Arena;

// =========================================================
// MARK: Arena Allocator Implementation
// =========================================================

static void* IMPL_ArenaAlloc(const size_t size, const void* ctx) {
	const BC_ArenaRef arena = (BC_ArenaRef)ctx;

	// Align to 8 bytes
	const size_t alignment = 8;
	const size_t alignedOffset = (arena->offset + alignment - 1) & ~(alignment - 1);

	// Check if we have enough space
	if (alignedOffset + size > arena->size) {
		// Try to allocate from allocator if available
		if (arena->allocatorRef) {
			return BC_AllocatorAlloc(arena->allocatorRef, size);
		}
		fprintf(stderr, "BC_Arena: Out of memory (requested %zu bytes, available %zu bytes)\n",
			size, arena->size - alignedOffset);
		return NULL;
	}

	void* ptr = (char*)arena->buffer + alignedOffset;
	arena->offset = alignedOffset + size;

	return ptr;
}

static void IMPL_ArenaFree(void* ptr, const void* ctx) {
	// Arena allocations are freed all at once when the arena is reset or destroyed
	// Individual frees are no-ops
	(void)ptr;
	(void)ctx;
}

// =========================================================
// MARK: Public API
// =========================================================

BC_ArenaRef BC_ArenaCreate(const BC_AllocatorRef allocator, const size_t size) {
	return BC_ArenaCreateWithBuffer(allocator, NULL, size);
}

BC_ArenaRef BC_ArenaCreateWithBuffer(BC_AllocatorRef allocator, void* buffer, const size_t size) {
	if (size == 0) {
		fprintf(stderr, "BC_ArenaCreate: Invalid size (must be > 0)\n");
		return NULL;
	}

	// Use system allocator if none provided
	if (!allocator) {
		allocator = kBC_AllocatorRefSystem;
	}

	// Allocate the arena structure itself
	const BC_ArenaRef arena = BC_AllocatorAlloc(allocator, sizeof(BC_Arena));
	if (!arena) {
		fprintf(stderr, "BC_ArenaCreate: Failed to allocate arena structure\n");
		return NULL;
	}

	// If no buffer provided, allocate one
	if (!buffer) {
		buffer = BC_AllocatorAlloc(allocator, size);
		if (!buffer) {
			fprintf(stderr, "BC_ArenaCreate: Failed to allocate arena buffer of %zu bytes\n", size);
			BC_AllocatorFree(allocator, arena);
			return NULL;
		}
		arena->ownsBuffer = BC_true;
	} else {
		arena->ownsBuffer = BC_false;
	}

	arena->buffer = buffer;
	arena->size = size;
	arena->offset = 0;
	arena->allocatorRef = allocator;
	arena->allocator.alloc = IMPL_ArenaAlloc;
	arena->allocator.free = IMPL_ArenaFree;
	arena->allocator.context = arena;

	return arena;
}

void BC_ArenaDestroy(const BC_ArenaRef arena) {
	if (!arena) return;

	const BC_AllocatorRef allocator = arena->allocatorRef ? arena->allocatorRef : kBC_AllocatorRefSystem;

	// Free the buffer if we allocated it
	if (arena->ownsBuffer && arena->buffer) {
		BC_AllocatorFree(allocator, arena->buffer);
	}

	// Free the arena structure itself
	BC_AllocatorFree(allocator, arena);
}

BC_AllocatorRef BC_ArenaAllocator(const BC_ArenaRef arena) {
	if (!arena) return NULL;
	return &arena->allocator;
}

void BC_ArenaReset(const BC_ArenaRef arena) {
	if (!arena) return;
	arena->offset = 0;
}

size_t BC_ArenaCapacity(const BC_ArenaRef arena) {
	if (!arena) return 0;
	return arena->size;
}

size_t BC_ArenaUsed(const BC_ArenaRef arena) {
	if (!arena) return 0;
	return arena->offset;
}
