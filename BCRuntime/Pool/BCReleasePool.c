#include "BCReleasePool.h"

#include "../BCAllocator.h"
#include "../BCClass.h"
#include "../BCObject.h"
#include "../Utilities/BCMacro.h"

#include <stdio.h>

// =========================================================
// MARK: Struct
// =========================================================

typedef struct BCReleasePool {
	BCObject base;
	uint16_t count;
	uint16_t capacity;
	BCObjectRef* stack;
} BCReleasePool;

// =========================================================
// MARK: Base Methods
// =========================================================

static void ReleasePoolDealloc(const BCObjectRef obj) {
    const BCReleasePool* pool = (BCReleasePool*)obj;

    for (uint16_t i = 0; i < pool->count; i++) {
        BCRelease(pool->stack[i]);
    }

	const BCAllocatorRef alloc = BCObjectGetAllocator(obj);
	BCAllocatorFree(alloc, pool->stack);
}

// =========================================================
// MARK: Class
// =========================================================

static const BCClass kBCReleasePoolClass = {
	.name = "BCReleasePool",
	.dealloc = ReleasePoolDealloc,
	.hash =	NULL,
	.equal = NULL,
	.toString = NULL,
	.copy = NULL,
	.allocSize = sizeof(BCReleasePool)
};

BCClassRef BCReleasePoolClass() {
	return (BCClassRef)&kBCReleasePoolClass;
}

// =========================================================
// MARK: Public
// =========================================================

BCReleasePoolRef BCReleasePoolCreate(const BCAllocatorRef allocator, const size_t initialCapacity) {
	const BCReleasePoolRef pool = (BCReleasePoolRef)BCObjectAlloc(allocator, BCReleasePoolClass());
	pool->count = 0;
	pool->capacity = initialCapacity;
	pool->stack = BCAllocatorAlloc(allocator, sizeof(BCObjectRef) * initialCapacity);
	return pool;
}

BCObjectRef BCReleasePoolAdd(const BCReleasePoolRef pool, const BCObjectRef obj) {
	if (!obj) return NULL;
	if (!pool) return obj;

	if (pool->count == pool->capacity) {
		const size_t newCapacity = pool->capacity * 2;

		// overflow
		if (newCapacity > UINT16_MAX || newCapacity <= pool->capacity) {
			fprintf(stderr, "Error: BCReleasePool capacity overflow (current: %u), leaking\n", pool->capacity);
			return obj;
		}

		const BCAllocatorRef allocator = BCObjectGetAllocator((BCObjectRef)pool);
		BCObjectRef* newPtr = BCAllocatorRealloc(
			allocator,
			pool->stack,
			sizeof(BCObjectRef) * pool->capacity,
			sizeof(BCObjectRef) * newCapacity
		);
		pool->stack = newPtr;
		pool->capacity = newCapacity;
	}

	pool->stack[pool->count++] = obj;

	return obj;
}

uint16_t BCReleasePoolCount(const BCReleasePoolRef pool) {
	if (!pool) return 0;
	return pool->count;
}

uint16_t BCReleasePoolCapacity(const BCReleasePoolRef pool) {
	if (!pool) return 0;
	return pool->capacity;
}