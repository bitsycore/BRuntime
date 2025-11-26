#ifndef BCRUNTIME_BCOBJECT_H
#define BCRUNTIME_BCOBJECT_H

#include "BCAllocator.h"
#include "BCSettings.h"
#include "BCTypes.h"
#include "Utilities/BCAtomics.h"

#include <stddef.h>

typedef struct BCObject {
	BCClassId cls;
	uint16_t flags;
	BC_atomic_uint16 ref_count;
	uint8_t allocator_ptr[sizeof(uintptr_t)];
} BCObject;

// Flags 0 -> 7 Object
// Use BC refcounting mechanism
#define BC_OBJECT_FLAG_REFCOUNT 1 << 0
// Object is constant and ignore free
#define BC_OBJECT_FLAG_CONSTANT 1 << 1
// Object uses non-default allocator meaning it use extended layout
#define BC_OBJECT_FLAG_NON_DEFAULT_ALLOCATOR 1 << 2
// Object uses non-default allocator meaning it use extended layout
#define BC_OBJECT_FLAG_INLINED 1 << 2
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
// MARK: Allocator Handling
// =========================================================

static inline void* ___BCINTERNAL_ObjectAllocatorLoad(const BCObjectRef o) {
	uintptr_t v;
	memcpy(&v, o->allocator_ptr, sizeof(v));
	return (void*)v;
}

static inline void ___BCINTERNAL_ObjectAllocatorSave(const BCObjectRef o, const BCAllocatorRef p) {
	memcpy(o->allocator_ptr, &p, sizeof(uintptr_t));
}

#define BCObjectGetAllocator(obj) ( BC_FLAG_HAS( (obj)->flags, BC_OBJECT_FLAG_NON_DEFAULT_ALLOCATOR ) ? ___BCINTERNAL_ObjectAllocatorLoad(obj) : BCAllocatorGetDefault() )

#define BCObjectSetAllocator(obj, allocator) \
    do { \
        if ( allocator == NULL || allocator == BCAllocatorGetDefault() ) { \
			BC_FLAG_CLEAR( (obj)->flags, BC_OBJECT_FLAG_NON_DEFAULT_ALLOCATOR ); \
			___BCINTERNAL_ObjectAllocatorSave((obj), NULL); \
        } else { \
			BC_FLAG_SET( (obj)->flags, BC_OBJECT_FLAG_NON_DEFAULT_ALLOCATOR ); \
            ___BCINTERNAL_ObjectAllocatorSave((obj), (allocator)); \
        } \
    } while (0)

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