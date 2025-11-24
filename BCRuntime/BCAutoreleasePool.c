#include "BCAutoreleasePool.h"

#include "Utilities/BCKeywords.h"
#include "Utilities/BCMemory.h"

#define CAPACITY 255

// =========================================================
// MARK: Struct
// =========================================================

typedef struct BCAutoreleasePool {
  struct BCAutoreleasePool *next;
  struct BCAutoreleasePool *parent;
  struct BCAutoreleasePool *hot;
  uint8_t count;
  BCObjectRef stack[CAPACITY];
} BCAutoreleasePool;

$TLS BCAutoreleasePool gRootPool = {
    .next = NULL,
    .parent = NULL,
    .hot = NULL,
    .count = 0,
    .stack = {},
};

$TLS BCAutoreleasePool *CurrentAutoReleasePool = NULL;

// =========================================================
// MARK: Private
// =========================================================

static BCAutoreleasePool *AllocNewPool() {
  BCAutoreleasePool *pool = BCMalloc(sizeof(BCAutoreleasePool));
  pool->count = 0;
  pool->parent = NULL;
  pool->next = NULL;
  pool->hot = pool;
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
    gRootPool.hot = &gRootPool;
  } else {
    BCAutoreleasePool *pool = AllocNewPool();
    pool->parent = CurrentAutoReleasePool;
    CurrentAutoReleasePool = pool;
  }
}

void BCAutoreleasePoolPop(void) {
  if (!CurrentAutoReleasePool)
    return;
  BCAutoreleasePool *pool = CurrentAutoReleasePool;
  CurrentAutoReleasePool = pool->parent;
  while (pool) {
    for (uint8_t i = 0; i < pool->count; i++) {
      BCRelease(pool->stack[i]);
    }
    BCAutoreleasePool *next = pool->next;
    if (pool != &gRootPool)
      BCFree(pool);
    pool = next;
  }
}

BCObjectRef BCAutorelease(const BCObjectRef obj) {
  if (!obj)
    return NULL;
  if (!CurrentAutoReleasePool) {
    fprintf(stderr, "Warning: Autorelease with no pool. Leaking.\n");
    return obj;
  }

  BCAutoreleasePool *pool = CurrentAutoReleasePool->hot;
  if (pool->count == CAPACITY) {
    BCAutoreleasePool *newPool = AllocNewPool();
    pool->next = newPool;
    CurrentAutoReleasePool->hot = newPool;
    pool = newPool;
  }

  pool->stack[pool->count++] = obj;
  return obj;
}