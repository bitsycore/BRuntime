#include "BCAutoreleasePool.h"

#include "Utilities/BCKeywords.h"

// =========================================================
// MARK: Struct
// =========================================================

typedef struct BCAutoreleasePool {
	BCObjectRef* stack;
	size_t capacity;
	size_t count;
	struct BCAutoreleasePool* parent;
} BCAutoreleasePool;

$TLS BCAutoreleasePool* CurrentAutoReleasePool = NULL;

// =========================================================
// MARK: Public
// =========================================================

void BCAutoreleasePoolPush(void) {
	BCAutoreleasePool* pool = calloc(1, sizeof(BCAutoreleasePool));
	pool->capacity = 32;
	pool->stack = calloc(pool->capacity, sizeof(BCObjectRef));
	pool->parent = CurrentAutoReleasePool;
	CurrentAutoReleasePool = pool;
}

void BCAutoreleasePoolPop(void) {
	if (!CurrentAutoReleasePool) return;
	BCAutoreleasePool* pool = CurrentAutoReleasePool;

	for (size_t i = 0; i < pool->count; i++) {
		BCRelease(pool->stack[i]);
	}

	CurrentAutoReleasePool = pool->parent;
	free(pool->stack);
	free(pool);
}

BCObjectRef BCAutorelease(const BCObjectRef obj) {
	if (!obj) return NULL;
	if (!CurrentAutoReleasePool) {
		fprintf(stderr, "Warning: Autorelease with no pool. Leaking.\n");
		return obj;
	}

	BCAutoreleasePool* pool = CurrentAutoReleasePool;
	if (pool->count == pool->capacity) {
		pool->capacity *= 2;
		pool->stack = realloc(pool->stack, pool->capacity * sizeof(BCObjectRef));
	}

	pool->stack[pool->count++] = obj;
	return obj;
}