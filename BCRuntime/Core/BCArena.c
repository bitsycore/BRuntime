#include "BCArena.h"

#include <stdio.h>
#include <string.h>

#include "../Utilities/BC_Keywords.h"
#include "../Utilities/BC_Memory.h"

// =========================================================
// MARK: Arena Structure
// =========================================================

typedef struct BCArena {
	void* buffer;
	size_t size;
	size_t offset;
	BCAllocatorRef allocatorRef;  // Allocator for fallback and for freeing
	BCAllocator allocator;        // The allocator interface for this arena
	BC_bool ownsBuffer;           //
} BCArena;

// =========================================================
// MARK: Arena Allocator Implementation
// =========================================================

static void* ArenaAlloc(const size_t size, const void* ctx) {
	BCArenaRef arena = (BCArenaRef)ctx;

	// Align to 8 bytes
	const size_t alignment = 8;
	size_t alignedOffset = (arena->offset + alignment - 1) & ~(alignment - 1);

	// Check if we have enough space
	if (alignedOffset + size > arena->size) {
		// Try to allocate from allocator if available
		if (arena->allocatorRef) {
			return BCAllocatorAlloc(arena->allocatorRef, size);
		}
		fprintf(stderr, "BCArena: Out of memory (requested %zu bytes, available %zu bytes)\n",
			size, arena->size - alignedOffset);
		return NULL;
	}

	void* ptr = (char*)arena->buffer + alignedOffset;
	arena->offset = alignedOffset + size;

	return ptr;
}

static void ArenaFree(void* ptr, const void* ctx) {
	// Arena allocations are freed all at once when the arena is reset or destroyed
	// Individual frees are no-ops
	(void)ptr;
	(void)ctx;
}

// =========================================================
// MARK: Public API
// =========================================================

BCArenaRef BCArenaCreate(const BCAllocatorRef allocator, const size_t size) {
	return BCArenaCreateWithBuffer(allocator, NULL, size);
}

BCArenaRef BCArenaCreateWithBuffer(BCAllocatorRef allocator, void* buffer, const size_t size) {
	if (size == 0) {
		fprintf(stderr, "BCArenaCreate: Invalid size (must be > 0)\n");
		return NULL;
	}

	// Use system allocator if none provided
	if (!allocator) {
		allocator = kBCAllocatorRefSystem;
	}

	// Allocate the arena structure itself
	const BCArenaRef arena = BCAllocatorAlloc(allocator, sizeof(BCArena));
	if (!arena) {
		fprintf(stderr, "BCArenaCreate: Failed to allocate arena structure\n");
		return NULL;
	}

	// If no buffer provided, allocate one
	if (!buffer) {
		buffer = BCAllocatorAlloc(allocator, size);
		if (!buffer) {
			fprintf(stderr, "BCArenaCreate: Failed to allocate arena buffer of %zu bytes\n", size);
			BCAllocatorFree(allocator, arena);
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
	arena->allocator.alloc = ArenaAlloc;
	arena->allocator.free = ArenaFree;
	arena->allocator.context = arena;

	return arena;
}

void BCArenaDestroy(const BCArenaRef arena) {
	if (!arena) return;

	const BCAllocatorRef allocator = arena->allocatorRef ? arena->allocatorRef : kBCAllocatorRefSystem;

	// Free the buffer if we allocated it
	if (arena->ownsBuffer && arena->buffer) {
		BCAllocatorFree(allocator, arena->buffer);
	}

	// Free the arena structure itself
	BCAllocatorFree(allocator, arena);
}

BCAllocatorRef BCArenaAllocator(const BCArenaRef arena) {
	if (!arena) return NULL;
	return &arena->allocator;
}

void BCArenaReset(const BCArenaRef arena) {
	if (!arena) return;
	arena->offset = 0;
}

size_t BCArenaCapacity(const BCArenaRef arena) {
	if (!arena) return 0;
	return arena->size;
}

size_t BCArenaUsed(const BCArenaRef arena) {
	if (!arena) return 0;
	return arena->offset;
}
