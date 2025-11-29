#ifndef BCRUNTIME_BCARENA_H
#define BCRUNTIME_BCARENA_H

#include "BCAllocator.h"
#include "BCTypes.h"

BCArenaRef BCArenaCreate(BCAllocatorRef allocator, size_t size);
BCArenaRef BCArenaCreateWithBuffer(BCAllocatorRef allocator, void* buffer, size_t size);
void BCArenaDestroy(BCArenaRef arena);

BCAllocatorRef BCArenaAllocator(BCArenaRef arena);
void BCArenaReset(BCArenaRef arena);

size_t BCArenaCapacity(BCArenaRef arena);
size_t BCArenaUsed(BCArenaRef arena);

#endif //BCRUNTIME_BCARENA_H
