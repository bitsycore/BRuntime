#ifndef BOBJECT_RELEASE_POOL_H
#define BOBJECT_RELEASE_POOL_H

#include "../BF_Types.h"

#include <stddef.h>

// =========================================================
// MARK: Object-based
// =========================================================

typedef struct BO_ReleasePool* BO_ReleasePoolRef;

BF_ClassId BO_ReleasePoolClassId();

BO_ReleasePoolRef BO_ReleasePoolCreate(BC_AllocatorRef allocator, size_t initialCapacity);
BO_ObjectRef BO_ReleasePoolAdd(BO_ReleasePoolRef pool, BO_ObjectRef obj);
uint16_t BO_ReleasePoolCount(BO_ReleasePoolRef pool);
uint16_t BO_ReleasePoolCapacity(BO_ReleasePoolRef pool);

#endif //BOBJECT_RELEASE_POOL_H
