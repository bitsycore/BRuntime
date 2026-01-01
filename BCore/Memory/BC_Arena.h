#ifndef BCORE_ARENA_H
#define BCORE_ARENA_H

#include "BC_Allocator.h"

BC_ArenaRef BC_ArenaCreate(BC_AllocatorRef allocator, size_t size);
BC_ArenaRef BC_ArenaCreateWithBuffer(BC_AllocatorRef allocator, void* buffer, size_t size);
void BC_ArenaDestroy(BC_ArenaRef arena);

BC_AllocatorRef BC_ArenaAllocator(BC_ArenaRef arena);
void BC_ArenaReset(BC_ArenaRef arena);

size_t BC_ArenaCapacity(BC_ArenaRef arena);
size_t BC_ArenaUsed(BC_ArenaRef arena);

#endif //BCORE_ARENA_H
