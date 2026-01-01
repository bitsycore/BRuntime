#include "BF_AutoreleasePool.h"

#include "BCore/BC_Keywords.h"
#include "BCore/Memory/BC_Memory.h"

#include "BObject/BO_Object.h"

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
} PRIV_AutoreleasePool;

// =========================================================
// MARK: Thread Local Storage
// =========================================================

static BC_TLS struct RootPool {
	PRIV_AutoreleasePool* parent;
	PRIV_AutoreleasePool* hot;
	PRIV_AutoreleasePool* next;
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

BC_TLS PRIV_AutoreleasePool* gCurrentAutoReleasePool = NULL;

// Free list for pool reuse - avoids repeated malloc/free
BC_TLS PRIV_AutoreleasePool* gFreePoolList = NULL;
BC_TLS uint8_t gFreePoolCount = 0;

// =========================================================
// MARK: Private Functions
// =========================================================

static inline PRIV_AutoreleasePool* PRIV_AllocPool(void) {
	PRIV_AutoreleasePool* pool;

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
		const size_t totalSize = sizeof(PRIV_AutoreleasePool) + BF_AUTORELEASE_POOL_CAPACITY * sizeof(BO_ObjectRef);
		pool = BC_Malloc(totalSize);
		pool->count = 0;
		pool->parent = NULL;
		pool->next = NULL;
		pool->capacity = BF_AUTORELEASE_POOL_CAPACITY;
		pool->hot = pool;
	}

	return pool;
}

// Return pool to free list for reuse, or free if list is full
static inline void PRIV_FreeOrRecyclePool(PRIV_AutoreleasePool* pool) {
	// Never free the root pool
	if (pool == (PRIV_AutoreleasePool*)&gRootPool)
		return;

	// Add to free list if there's room, otherwise free
	if (gFreePoolCount < BF_AUTORELEASE_POOL_FREE_LIST_LIMIT) {
		pool->next = gFreePoolList;
		gFreePoolList = pool;
		gFreePoolCount++;
	}
	else {
		BC_Free(pool);
	}
}

// =========================================================
// MARK: Runtime Lifecycle
// =========================================================

void INTERNAL_BF_AutoreleaseInitialize(void) {
	// Reset the root pool
	gRootPool.parent = NULL;
	gRootPool.hot = NULL;
	gRootPool.next = NULL;
	gRootPool.count = 0;
	gRootPool.capacity = BF_AUTORELEASE_POOL_CAPACITY;

	// Reset thread-local state
	gCurrentAutoReleasePool = NULL;
	gFreePoolList = NULL;
	gFreePoolCount = 0;
}

void INTERNAL_BF_AutoreleaseDeinitialize(void) {
	// Clean up any active pools first
	if (gCurrentAutoReleasePool) {
		while (gCurrentAutoReleasePool) {
			BF_AutoreleasePoolPop();
		}
	}

	// Free all pools in the free list
	PRIV_AutoreleasePool* pool = gFreePoolList;
	while (pool) {
		PRIV_AutoreleasePool* next = pool->next;
		BC_Free(pool);
		pool = next;
	}

	// Reset state
	gFreePoolList = NULL;
	gFreePoolCount = 0;
	gCurrentAutoReleasePool = NULL;
}

// =========================================================
// MARK: Autorelease Pool API
// =========================================================

void BF_AutoreleasePoolPush(void) {
	if (gCurrentAutoReleasePool == NULL) {
		// First push - use the root pool
		gCurrentAutoReleasePool = (PRIV_AutoreleasePool*)&gRootPool;
		gRootPool.count = 0;
		gRootPool.next = NULL;
		gRootPool.hot = (PRIV_AutoreleasePool*)&gRootPool;
	}
	else {
		// Subsequent push - allocate or reuse a pool
		PRIV_AutoreleasePool* pool = PRIV_AllocPool();
		pool->parent = gCurrentAutoReleasePool;
		gCurrentAutoReleasePool = pool;
	}
}

void BF_AutoreleasePoolPop(void) {
	if (!gCurrentAutoReleasePool)
		return;

	PRIV_AutoreleasePool* pool = gCurrentAutoReleasePool;
	gCurrentAutoReleasePool = pool->parent;

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

		PRIV_AutoreleasePool* next = pool->next;
		PRIV_FreeOrRecyclePool(pool);
		pool = next;
	}
}

BO_ObjectRef BF_Autorelease(const BO_ObjectRef obj) {
	if (!obj) return NULL;

	if (!gCurrentAutoReleasePool) {
		fprintf(stderr, "Warning: Autorelease with no pool. Leaking.\n");
		return obj;
	}

	PRIV_AutoreleasePool* pool = gCurrentAutoReleasePool->hot;

	// Check if current pool is full
	if (pool->count == pool->capacity) {
		// Allocate overflow pool
		PRIV_AutoreleasePool* newPool = PRIV_AllocPool();
		pool->next = newPool;
		gCurrentAutoReleasePool->hot = newPool;
		pool = newPool;
	}

	pool->stack[pool->count++] = obj;
	return obj;
}
