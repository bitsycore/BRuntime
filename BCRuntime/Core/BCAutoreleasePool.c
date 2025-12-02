#include "BCAutoreleasePool.h"

#include "../Utilities/BC_Keywords.h"
#include "../Utilities/BC_Memory.h"

// =========================================================
// MARK: Configuration
// =========================================================

#define BC_POOL_CAPACITY 256
#define BC_POOL_FREE_LIST_LIMIT 32

// =========================================================
// MARK: Struct
// =========================================================

typedef struct BCAutoreleasePool {
	struct BCAutoreleasePool* parent;
	struct BCAutoreleasePool* hot;
	struct BCAutoreleasePool* next;
	uint16_t count;
	uint16_t capacity;
	BCObjectRef stack[];
} BCAutoreleasePool;

// =========================================================
// MARK: Thread Local Storage
// =========================================================

static $TLS struct {
	BCAutoreleasePool* parent;
	BCAutoreleasePool* hot;
	BCAutoreleasePool* next;
	uint16_t count;
	uint16_t capacity;
	BCObjectRef stack[BC_POOL_CAPACITY];
} gRootPool = {
	.parent = NULL,
	.hot = NULL,
	.next = NULL,
	.count = 0,
	.capacity = BC_POOL_CAPACITY,
	.stack = {},
};

$TLS BCAutoreleasePool* CurrentAutoReleasePool = NULL;

// Free list for pool reuse - avoids repeated malloc/free
$TLS BCAutoreleasePool* gFreePoolList = NULL;
$TLS uint8_t gFreePoolCount = 0;

// =========================================================
// MARK: Private Functions
// =========================================================

static inline BCAutoreleasePool* AllocNewAutoreleasePool(void) {
	BCAutoreleasePool* pool;

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
		const size_t totalSize = sizeof(BCAutoreleasePool) + BC_POOL_CAPACITY * sizeof(BCObjectRef);
		pool = BCMalloc(totalSize);
		pool->count = 0;
		pool->parent = NULL;
		pool->next = NULL;
		pool->capacity = BC_POOL_CAPACITY;
		pool->hot = pool;
	}

	return pool;
}

// Return pool to free list for reuse, or free if list is full
static inline void FreeOrRecyclePool(BCAutoreleasePool* pool) {
	// Never free the root pool
	if (pool == (BCAutoreleasePool*)&gRootPool)
		return;

	// Add to free list if there's room, otherwise free
	if (gFreePoolCount < BC_POOL_FREE_LIST_LIMIT) {
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

void ___BCINTERNAL___AutoreleaseInitialize(void) {
	// Reset the root pool
	gRootPool.parent = NULL;
	gRootPool.hot = NULL;
	gRootPool.next = NULL;
	gRootPool.count = 0;
	gRootPool.capacity = BC_POOL_CAPACITY;

	// Reset thread-local state
	CurrentAutoReleasePool = NULL;
	gFreePoolList = NULL;
	gFreePoolCount = 0;
}

void ___BCINTERNAL___AutoreleaseDeinitialize(void) {
	// Clean up any active pools first
	if (CurrentAutoReleasePool) {
		while (CurrentAutoReleasePool) {
			BCAutoreleasePoolPop();
		}
	}

	// Free all pools in the free list
	BCAutoreleasePool* pool = gFreePoolList;
	while (pool) {
		BCAutoreleasePool* next = pool->next;
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

void BCAutoreleasePoolPush(void) {
	if (CurrentAutoReleasePool == NULL) {
		// First push - use the root pool
		CurrentAutoReleasePool = (BCAutoreleasePool*)&gRootPool;
		gRootPool.count = 0;
		gRootPool.next = NULL;
		gRootPool.hot = (BCAutoreleasePool*)&gRootPool;
	}
	else {
		// Subsequent push - allocate or reuse a pool
		BCAutoreleasePool* pool = AllocNewAutoreleasePool();
		pool->parent = CurrentAutoReleasePool;
		CurrentAutoReleasePool = pool;
	}
}

void BCAutoreleasePoolPop(void) {
	if (!CurrentAutoReleasePool)
		return;

	BCAutoreleasePool* pool = CurrentAutoReleasePool;
	CurrentAutoReleasePool = pool->parent;

	// Walk the overflow chain and release all objects
	while (pool) {
		// process in reverse order
		const BCObjectRef* obj = pool->stack + pool->count;
		uint16_t remaining = pool->count;

		// Process releases
		while (remaining > 0) {
			--obj;
			BCRelease(*obj);
			--remaining;
		}

		BCAutoreleasePool* next = pool->next;
		FreeOrRecyclePool(pool);
		pool = next;
	}
}

BCObjectRef BCAutorelease(const BCObjectRef obj) {
	if (!obj) return NULL;

	if (!CurrentAutoReleasePool) {
		fprintf(stderr, "Warning: Autorelease with no pool. Leaking.\n");
		return obj;
	}

	BCAutoreleasePool* pool = CurrentAutoReleasePool->hot;

	// Check if current pool is full
	if (pool->count == pool->capacity) {
		// Allocate overflow pool
		BCAutoreleasePool* newPool = AllocNewAutoreleasePool();
		pool->next = newPool;
		CurrentAutoReleasePool->hot = newPool;
		pool = newPool;
	}

	pool->stack[pool->count++] = obj;
	return obj;
}
