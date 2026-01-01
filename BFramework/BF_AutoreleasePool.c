#include "BF_AutoreleasePool.h"

#include "BCore/BC_Keywords.h"
#include "BCore/Memory/BC_Memory.h"
#include "BFramework/BObject/BO_Object.h"

// =========================================================
// MARK: Configuration
// =========================================================

#define BF_AUTORELEASE_POOL_CAPACITY 256
#define BF_AUTORELEASE_POOL_FREE_LIST_LIMIT 32

// =========================================================
// MARK: Struct
// =========================================================

typedef struct AutoreleasePool {
	struct AutoreleasePool* parent;
	struct AutoreleasePool* hot;
	struct AutoreleasePool* next;
	uint16_t count;
	uint16_t capacity;
	BO_ObjectRef stack[];
} AutoreleasePool;

// =========================================================
// MARK: Thread Local Storage
// =========================================================

static BC_TLS struct {
	AutoreleasePool* parent;
	AutoreleasePool* hot;
	AutoreleasePool* next;
	uint16_t count;
	uint16_t capacity;
	BO_ObjectRef stack[BF_AUTORELEASE_POOL_CAPACITY];
} gRootPool = {
	.parent = NULL,
	.hot = NULL,
	.next = NULL,
	.count = 0,
	.capacity = BF_AUTORELEASE_POOL_CAPACITY,
	.stack = {},
};

BC_TLS AutoreleasePool* CurrentAutoReleasePool = NULL;

// Free list for pool reuse - avoids repeated malloc/free
BC_TLS AutoreleasePool* gFreePoolList = NULL;
BC_TLS uint8_t gFreePoolCount = 0;

// =========================================================
// MARK: Private Functions
// =========================================================

static inline AutoreleasePool* AllocNewAutoreleasePool(void) {
	AutoreleasePool* pool;

	// Try to reuse from free list first
	if (gFreePoolList) {
		pool = gFreePoolList;
		gFreePoolList = pool->next;
		gFreePoolCount--;

		// Reset the pool state for reuse
		pool->count = 0;
		pool->parent = NULL;
		pool->next = NULL;
		pool->hot = pool;
	}
	else {
		// Allocate pool + stack
		const size_t totalSize = sizeof(AutoreleasePool) + BF_AUTORELEASE_POOL_CAPACITY * sizeof(BO_ObjectRef);
		pool = BCMalloc(totalSize);
		pool->count = 0;
		pool->parent = NULL;
		pool->next = NULL;
		pool->capacity = BF_AUTORELEASE_POOL_CAPACITY;
		pool->hot = pool;
	}

	return pool;
}

// Return pool to free list for reuse, or free if list is full
static inline void FreeOrRecyclePool(AutoreleasePool* pool) {
	// Never free the root pool
	if (pool == (AutoreleasePool*)&gRootPool)
		return;

	// Add to free list if there's room, otherwise free
	if (gFreePoolCount < BF_AUTORELEASE_POOL_FREE_LIST_LIMIT) {
		pool->next = gFreePoolList;
		gFreePoolList = pool;
		gFreePoolCount++;
	}
	else {
		BCFree(pool);
	}
}

// =========================================================
// MARK: Runtime Lifecycle
// =========================================================

void ___BF_INTERNAL___AutoreleaseInitialize(void) {
	// Reset the root pool
	gRootPool.parent = NULL;
	gRootPool.hot = NULL;
	gRootPool.next = NULL;
	gRootPool.count = 0;
	gRootPool.capacity = BF_AUTORELEASE_POOL_CAPACITY;

	// Reset thread-local state
	CurrentAutoReleasePool = NULL;
	gFreePoolList = NULL;
	gFreePoolCount = 0;
}

void ___BF_INTERNAL___AutoreleaseDeinitialize(void) {
	// Clean up any active pools first
	if (CurrentAutoReleasePool) {
		while (CurrentAutoReleasePool) {
			BFAutoreleasePoolPop();
		}
	}

	// Free all pools in the free list
	AutoreleasePool* pool = gFreePoolList;
	while (pool) {
		AutoreleasePool* next = pool->next;
		BCFree(pool);
		pool = next;
	}

	// Reset state
	gFreePoolList = NULL;
	gFreePoolCount = 0;
	CurrentAutoReleasePool = NULL;
}

// =========================================================
// MARK: Autorelease Pool API
// =========================================================

void BFAutoreleasePoolPush(void) {
	if (CurrentAutoReleasePool == NULL) {
		// First push - use the root pool
		CurrentAutoReleasePool = (AutoreleasePool*)&gRootPool;
		gRootPool.count = 0;
		gRootPool.next = NULL;
		gRootPool.hot = (AutoreleasePool*)&gRootPool;
	}
	else {
		// Subsequent push - allocate or reuse a pool
		AutoreleasePool* pool = AllocNewAutoreleasePool();
		pool->parent = CurrentAutoReleasePool;
		CurrentAutoReleasePool = pool;
	}
}

void BFAutoreleasePoolPop(void) {
	if (!CurrentAutoReleasePool)
		return;

	AutoreleasePool* pool = CurrentAutoReleasePool;
	CurrentAutoReleasePool = pool->parent;

	// Walk the overflow chain and release all objects
	while (pool) {
		// process in reverse order
		const BO_ObjectRef* obj = pool->stack + pool->count;
		uint16_t remaining = pool->count;

		// Process releases
		while (remaining > 0) {
			--obj;
			BO_Release(*obj);
			--remaining;
		}

		AutoreleasePool* next = pool->next;
		FreeOrRecyclePool(pool);
		pool = next;
	}
}

BO_ObjectRef BFAutorelease(const BO_ObjectRef obj) {
	if (!obj) return NULL;

	if (!CurrentAutoReleasePool) {
		fprintf(stderr, "Warning: Autorelease with no pool. Leaking.\n");
		return obj;
	}

	AutoreleasePool* pool = CurrentAutoReleasePool->hot;

	// Check if current pool is full
	if (pool->count == pool->capacity) {
		// Allocate overflow pool
		AutoreleasePool* newPool = AllocNewAutoreleasePool();
		pool->next = newPool;
		CurrentAutoReleasePool->hot = newPool;
		pool = newPool;
	}

	pool->stack[pool->count++] = obj;
	return obj;
}
