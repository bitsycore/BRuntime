#include "BCAutoreleasePool.h"

#include "Utilities/BCKeywords.h"
#include "Utilities/BCMemory.h"

#define CAPACITY 255

// =========================================================
// MARK: Struct
// =========================================================

typedef struct BCAutoreleasePool {
	struct BCAutoreleasePool* next;
	struct BCAutoreleasePool* parent;
	uint8_t count;
	BCObjectRef stack[CAPACITY];
} BCAutoreleasePool;

$TLS BCAutoreleasePool gRootPool = {
	.next = NULL,
	.parent = NULL,
	.count = 0,
	.stack = 0,
};

$TLS BCAutoreleasePool* CurrentAutoReleasePool = NULL;

// =========================================================
// MARK: Private
// =========================================================

static BCAutoreleasePool* AllocNewPool() {
	BCAutoreleasePool* pool = BCMalloc(sizeof(BCAutoreleasePool));
	pool->count = 0;
	pool->parent = NULL;
	pool->next = NULL;
	return pool;
}

// =========================================================
// MARK: Public
// =========================================================

void BCAutoreleasePoolPush(void) {
	if (CurrentAutoReleasePool == NULL) {
		CurrentAutoReleasePool = &gRootPool;
		gRootPool.count = 0;
		gRootPool.next = NULL;
	}
	else {
		BCAutoreleasePool* pool = AllocNewPool();
		pool->parent = CurrentAutoReleasePool;
		CurrentAutoReleasePool = pool;
	}
}

void BCAutoreleasePoolPop(void) {
	if (!CurrentAutoReleasePool) return;
	BCAutoreleasePool* pool = CurrentAutoReleasePool;
	CurrentAutoReleasePool = pool->parent;
	while (pool) {
		for (uint8_t i = 0; i < pool->count; i++) {
			BCRelease(pool->stack[i]);
		}
		BCAutoreleasePool* next = pool->next;
		if (pool != &gRootPool) BCFree(pool);
		pool = next;
	}
}

BCObjectRef BCAutorelease(const BCObjectRef obj) {
	if (!obj) return NULL;
	if (!CurrentAutoReleasePool) {
		fprintf(stderr, "Warning: Autorelease with no pool. Leaking.\n");
		return obj;
	}

	BCAutoreleasePool* pool = CurrentAutoReleasePool;
	if (pool->count == CAPACITY) {
		do{
			if (pool->next == NULL) {
				BCAutoreleasePool* newPool = AllocNewPool();
				pool->next = newPool;
				pool = newPool;
			} else {
				pool = pool->next;
			}
		} while (pool->count == CAPACITY);
	}

	pool->stack[pool->count++] = obj;
	return obj;
}