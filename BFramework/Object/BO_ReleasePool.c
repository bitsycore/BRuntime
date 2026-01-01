#include "BO_ReleasePool.h"

#include "BCore/BC_Macro.h"
#include "BCore/Memory/BC_Allocator.h"

#include "BO_Object.h"
#include "../BF_Class.h"

#include <stdio.h>

// =========================================================
// MARK: Struct
// =========================================================

typedef struct BO_ReleasePool {
	BO_Object base;
	uint16_t capacity;
	BO_ObjectRef* stack;
} BO_ReleasePool;

#define COUNT base.class_reserved

// =========================================================
// MARK: Base Methods
// =========================================================

static void ReleasePoolDealloc(const BO_ObjectRef obj) {
	const BO_ReleasePool* pool = (BO_ReleasePool*)obj;

	for (uint16_t i = 0; i < pool->COUNT; i++) {
		BO_Release(pool->stack[i]);
	}

	const BC_AllocatorRef alloc = BO_ObjectGetAllocator(obj);
	BC_AllocatorFree(alloc, pool->stack);
}

// =========================================================
// MARK: Class
// =========================================================

static BF_Class kBO_ReleasePoolClass = {
	.name = "BO_ReleasePool",
	.dealloc = ReleasePoolDealloc,
	.hash = NULL,
	.equal = NULL,
	.toString = NULL,
	.copy = NULL,
	.allocSize = sizeof(BO_ReleasePool)
};

BF_ClassId BO_ReleasePoolClassId() {
	return kBO_ReleasePoolClass.id;
}

void ___BO_INTERNAL___ReleasePoolInitialize(void) {
	BF_ClassRegistryInsert((BF_Class*)&kBO_ReleasePoolClass);
}

// =========================================================
// MARK: Public
// =========================================================

BO_ReleasePoolRef BO_ReleasePoolCreate(const BC_AllocatorRef allocator, const size_t initialCapacity) {
	const BO_ReleasePoolRef pool = (BO_ReleasePoolRef)BO_ObjectAlloc(allocator, kBO_ReleasePoolClass.id);
	pool->COUNT = 0;
	pool->capacity = initialCapacity;
	pool->stack = BC_AllocatorAlloc(allocator, sizeof(BO_ObjectRef) * initialCapacity);
	return pool;
}

BO_ObjectRef BO_ReleasePoolAdd(const BO_ReleasePoolRef pool, const BO_ObjectRef obj) {
	if (!obj) return NULL;
	if (!pool) return obj;

	if (pool->COUNT == pool->capacity) {
		const size_t newCapacity = pool->capacity * 2;

		// overflow
		if (newCapacity > UINT16_MAX || newCapacity <= pool->capacity) {
			fprintf(stderr, "Error: BO_ReleasePool capacity overflow (current: %u), leaking\n", pool->capacity);
			return obj;
		}

		const BC_AllocatorRef allocator = BO_ObjectGetAllocator((BO_ObjectRef)pool);
		BO_ObjectRef* newPtr = BC_AllocatorRealloc(
			allocator,
			pool->stack,
			sizeof(BO_ObjectRef) * pool->capacity,
			sizeof(BO_ObjectRef) * newCapacity
		);
		pool->stack = newPtr;
		pool->capacity = newCapacity;
	}

	pool->stack[pool->COUNT++] = obj;

	return obj;
}

uint16_t BO_ReleasePoolCount(const BO_ReleasePoolRef pool) {
	if (!pool) return 0;
	return pool->COUNT;
}

uint16_t BO_ReleasePoolCapacity(const BO_ReleasePoolRef pool) {
	if (!pool) return 0;
	return pool->capacity;
}
