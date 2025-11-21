#include "BCAutoreleasePool.h"

// =========================================================
// MARK: Struct
// =========================================================

typedef struct BCAutoreleasePool {
	BCObject** stack;
	size_t capacity;
	size_t count;
	struct BCAutoreleasePool* parent;
} BCAutoreleasePool;

_Thread_local BCAutoreleasePool* _bcCurrentPool = NULL;

// =========================================================
// MARK: Public
// =========================================================

void BCAutoreleasePoolPush(void) {
	BCAutoreleasePool* pool = calloc(1, sizeof(BCAutoreleasePool));
	pool->capacity = 32;
	pool->stack = calloc(pool->capacity, sizeof(BCObject*));
	pool->parent = _bcCurrentPool;
	_bcCurrentPool = pool;
}

void BCAutoreleasePoolPop(void) {
	if (!_bcCurrentPool) return;
	BCAutoreleasePool* pool = _bcCurrentPool;

	for (size_t i = 0; i < pool->count; i++) {
		BCRelease(pool->stack[i]);
	}

	_bcCurrentPool = pool->parent;
	free(pool->stack);
	free(pool);
}

BCObject* BCAutorelease(BCObject* obj) {
	if (!obj) return NULL;
	if (!_bcCurrentPool) {
		fprintf(stderr, "Warning: Autorelease with no pool. Leaking.\n");
		return obj;
	}

	BCAutoreleasePool* pool = _bcCurrentPool;
	if (pool->count == pool->capacity) {
		pool->capacity *= 2;
		pool->stack = realloc(pool->stack, pool->capacity * sizeof(BCObject*));
	}
	pool->stack[pool->count++] = obj;
	return obj;

}