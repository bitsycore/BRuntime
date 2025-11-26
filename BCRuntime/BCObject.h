#ifndef BCRUNTIME_BCOBJECT_H
#define BCRUNTIME_BCOBJECT_H

#include "BCClassRegistry.h"
#include "BCSettings.h"
#include "BCTypes.h"
#include "Utilities/BCAtomics.h"

#include <stddef.h>

typedef struct BCObject {
  BCClassId cls;
  uint16_t flags;
  BC_atomic_uint16 ref_count;
} BCObject;

// Flags 0 -> 7 Object
// Use BC refcounting mechanism
#define BC_OBJECT_FLAG_REFCOUNT 1 << 0
// Object is constant and ignore free
#define BC_OBJECT_FLAG_CONSTANT 1 << 1
// Object uses non-default allocator meaning it use extended layout
#define BC_OBJECT_FLAG_NON_DEFAULT_ALLOCATOR 1 << 2
// Object uses non-default allocator meaning it use extended layout
#define BC_OBJECT_FLAG_INLINED 1 << 3
// Flags 8 -> 15 Free usage for class
#define BC_OBJECT_FLAG_CLASS_MASK 0xFF00

BCObjectRef BCObjectAlloc(BCAllocatorRef alloc, BCClassId cls);
BCObjectRef BCObjectAllocWithConfig(BCAllocatorRef alloc, BCClassId cls, size_t extraBytes, uint16_t flags);

BCObjectRef BCRetain(BCObjectRef obj);
void BCRelease(BCObjectRef obj);

BCObjectRef BCCopy(BCObjectRef obj);
uint32_t BCHash(BCObjectRef obj);
BC_bool BCEqual(BCObjectRef a, BCObjectRef b);
BCStringRef BCToString(BCObjectRef obj);

BC_bool BCIsClass(BCObjectRef obj, BCClassId cls);
BCClassRef BCObjectClass(BCObjectRef obj);


// =========================================================
// MARK: Debug
// =========================================================

#if BC_SETTINGS_DEBUG_OBJECT_DUMP == 1

void BCObjectDebugSetEnabled(BC_bool enabled);
void BCObjectDebugSetKeepFreed(BC_bool keepFreed);
void BCObjectDebugDump(void);

#else

#define BCObjectDebugSetEnabled(...)
#define BCObjectDebugSetKeepFreed(...)
#define BCObjectDebugDump(...)

#endif

#endif // BCRUNTIME_BCOBJECT_H