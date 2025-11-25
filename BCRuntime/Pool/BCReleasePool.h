#ifndef BCRUNTIME_BCRELEASEPOOL_H
#define BCRUNTIME_BCRELEASEPOOL_H

#include "../BCTypes.h"

#include <stddef.h>

// =========================================================
// MARK: Object-based
// =========================================================

typedef struct BCReleasePool* BCReleasePoolRef;

BCClassRef BCReleasePoolClass();

BCReleasePoolRef BCReleasePoolCreate(BCAllocatorRef allocator, size_t initialCapacity);
BCObjectRef BCReleasePoolAdd(BCReleasePoolRef pool, BCObjectRef obj);
uint16_t BCReleasePoolCount(BCReleasePoolRef pool);
uint16_t BCReleasePoolCapacity(BCReleasePoolRef pool);

#endif //BCRUNTIME_BCRELEASEPOOL_H
