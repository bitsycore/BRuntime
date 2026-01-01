#ifndef BOBJECT_OBJECT_H
#define BOBJECT_OBJECT_H

#include "BCore/Memory/BC_Allocator.h"
#include "BCore/Thread/BC_Atomics.h"

#include "../BF_Settings.h"
#include "../BF_Types.h"
#include "../BF_Class.h"

#include <stddef.h>

typedef struct BO_Object {
	BF_ClassId cls;
	BC_atomic_uint16 ref_count;
	uint16_t flags;
	uint16_t class_reserved;
} BO_Object;

#define BC_OBJECT_DEFAULT_FLAGS (BC_OBJECT_FLAG_REFCOUNT)

// Flags 0 -> 7 Object
// Use BC refcounting mechanism
#define BC_OBJECT_FLAG_REFCOUNT 1 << 0
// Object is constant and ignore free
#define BC_OBJECT_FLAG_CONSTANT 1 << 1
// Object uses non-default allocator meaning it use extended layout
#define BC_OBJECT_FLAG_NON_SYSTEM_ALLOCATOR 1 << 2
// Object uses non-default allocator meaning it use extended layout
#define BC_OBJECT_FLAG_INLINED 1 << 3
// Flags 8 -> 15 Free usage for class
#define BC_OBJECT_FLAG_CLASS_MASK 0xFF00

BO_ObjectRef BO_ObjectAlloc(BC_AllocatorRef alloc, BF_ClassId cls);
BO_ObjectRef BO_ObjectAllocWithConfig(BC_AllocatorRef alloc, BF_ClassId cls, size_t extraBytes, uint16_t flags);

BO_ObjectRef BO_Retain(BO_ObjectRef obj);
void BO_Release(BO_ObjectRef obj);

BO_ObjectRef BO_Copy(BO_ObjectRef obj);
uint32_t BO_Hash(BO_ObjectRef obj);
BC_bool BO_Equal(BO_ObjectRef a, BO_ObjectRef b);
BO_StringRef BO_ToString(BO_ObjectRef obj);

BC_bool BO_IsClass(BO_ObjectRef obj, BF_ClassId cls);
BF_Class* BO_ObjectClass(BO_ObjectRef obj);

// =========================================================
// MARK: Allocator Handling
// =========================================================

#define BO_ObjectGetAllocator(obj) ( BC_FLAG_HAS( (obj)->flags, BC_OBJECT_FLAG_NON_SYSTEM_ALLOCATOR ) ?  ( ((BC_AllocatorRef)(obj)) - 1 ) : kBC_AllocatorRefSystem )
#define BO_ObjectGetBasePointer(obj) ( BC_FLAG_HAS((obj)->flags, BC_OBJECT_FLAG_NON_SYSTEM_ALLOCATOR) ? ((BC_AllocatorRef)(obj)) - 1 : (BC_AllocatorRef)(obj) )
#define BO_ObjectSetAllocator(obj, allocator) \
	do { \
		__typeof__(allocator) temp_alloc = allocator ? allocator : BC_AllocatorGetDefault();\
		if ( allocator == kBC_AllocatorRefSystem ) { \
			BC_FLAG_CLEAR( (obj)->flags, BC_OBJECT_FLAG_NON_SYSTEM_ALLOCATOR ); \
		} else { \
			BC_FLAG_SET( (obj)->flags, BC_OBJECT_FLAG_NON_SYSTEM_ALLOCATOR ); \
			*(((BC_AllocatorRef*)(obj)) - 1 ) = (temp_alloc); \
		} \
	} while (0)

// =========================================================
// MARK: Debug
// =========================================================

#if BC_SETTINGS_DEBUG_OBJECT_DUMP == 1

void BO_ObjectDebugSetEnabled(BC_bool enabled);
void BO_ObjectDebugSetKeepFreed(BC_bool keepFreed);
void BO_ObjectDebugDump(void);

#else

#define BO_ObjectDebugSetEnabled(...)
#define BO_ObjectDebugSetKeepFreed(...)
#define BO_ObjectDebugDump(...)

#endif

#endif //BOBJECT_OBJECT_H