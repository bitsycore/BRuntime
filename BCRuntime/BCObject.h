#ifndef BCRUNTIME_BCOBJECT_H
#define BCRUNTIME_BCOBJECT_H

#include "BCTypes.h"

#include <stdatomic.h>
#include <stddef.h>

typedef struct BCClass {
	const char* name;
	BCDeallocFunc dealloc;
	BCHashFunc hash;
	BCEqualFunc equal;
	BCDescriptionFunc toString;
	BCCopyFunc copy;
	size_t allocSize;
} BCClass;

typedef struct BCObject {
	BCClassRef cls;
	atomic_int ref_count;
	BCAllocatorRef allocator;
} BCObject;

typedef struct BCAllocator {
	void* (* alloc)(size_t size, const void* ctx);
	void (* free)(void* ptr, const void* ctx);
	void* context;
} BCAllocator;

extern BCAllocatorRef const kBCAllocatorDefault;

BCObject* BCObjectAlloc(BCClassRef cls, BCAllocatorRef alloc);

BCObject* BCRetain(BCObjectRef obj);
void BCRelease(BCObjectRef obj);

BCObject* BCObjectCopy(BCObjectRef obj);
uint32_t BCHash(BCObjectRef obj);
bool BCEqual(BCObjectRef a, BCObjectRef b);
BCStringRef BCToString(BCObjectRef obj);

BCStringRef BCClassName(BCClassRef cls);

BCClassRef BCObjectClass(BCObjectRef obj);
bool BCObjectIsClass(BCObjectRef obj, BCClassRef cls);

#endif //BCRUNTIME_BCOBJECT_H
