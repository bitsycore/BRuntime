#ifndef BCRUNTIME_BCOBJECT_H
#define BCRUNTIME_BCOBJECT_H

#include "BCSettings.h"
#include "BCTypes.h"
#include "Utilities/BCAtomics.h"

#include <stddef.h>

typedef struct BCClass {
	const char* name;
	BCDeallocFunc dealloc;
	BCHashFunc hash;
	BCEqualFunc equal;
	BCToStringFunc toString;
	BCCopyFunc copy;
	size_t allocSize;
} BCClass;

typedef struct BCObject {
	BCClassRef cls;
	uint32_t flags;
	BC_atomic_int ref_count;
	BCAllocatorRef allocator;
} BCObject;

// Flags 0 -> 15 Object
#define BC_OBJECT_FLAG_REFCOUNT  1 << 0
#define BC_OBJECT_FLAG_HEAP      1 << 1
#define BC_OBJECT_FLAG_STATIC    1 << 2
#define BC_OBJECT_FLAG_POOLED    1 << 3
#define BC_OBJECT_FLAG_HAS_HASH  1 << 4
// Flags 16 -> 31 Free usage for class
#define BC_OBJECT_FLAG_CLASS_MASK 0xFFFF0000

#define BC_FLAG_HAS(obj, flag) ((obj) & (flag))
#define BC_FLAG_SET(obj, flag) ((obj) |= (flag))
#define BC_FLAG_CLEAR(obj, flag) ((obj) &= ~(flag))
#define BC_FLAG_TOGGLE(obj, flag) ((obj) ^= (flag))

typedef struct BCAllocator {
	void* (* alloc)(size_t size, const void* ctx);
	void (* free)(void* ptr, const void* ctx);
	void* context;
} BCAllocator;

extern BCAllocatorRef const kBCAllocatorDefault;

BCObjectRef BCAllocObject(BCClassRef cls, BCAllocatorRef alloc);
BCObjectRef BCAllocObjectWithExtra(BCClassRef cls, BCAllocatorRef alloc, size_t extraBytes, uint32_t flags);

BCObjectRef BCRetain(BCObjectRef obj);
void BCRelease(BCObjectRef obj);

BCObjectRef BCObjectCopy(BCObjectRef obj);
uint32_t BCHash(BCObjectRef obj);
bool BCEqual(BCObjectRef a, BCObjectRef b);
BCStringRef BCToString(BCObjectRef obj);

BCStringRef BCClassName(BCClassRef cls);

BCClassRef BCObjectClass(BCObjectRef obj);
bool BCObjectIsClass(BCObjectRef obj, BCClassRef cls);

// =========================================================
// MARK: Debug
// =========================================================

#ifdef BC_SETTINGS_DEBUG_OBJECT_DUMP
void BCObjectDebugSetEnabled(bool enabled);
void BCObjectDebugSetKeepFreed(bool keepFreed);
void BCObjectDebugDump(void);
#else
#define BCObjectDebugSetEnabled(...)
#define BCObjectDebugSetKeepFreed(...)
#define BCObjectDebugDump(...)
#endif

#endif //BCRUNTIME_BCOBJECT_H