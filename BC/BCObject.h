#ifndef BCRUNTIME_BCOBJECT_H
#define BCRUNTIME_BCOBJECT_H

#include "BCTypes.h"

#include <stdatomic.h>

typedef struct BCClass {
	const char* name;
	BCDeallocFunc dealloc;
	BCHashFunc hash;
	BCEqualFunc equal;
	BCDescFunc description;
	BCCopyFunc copy;
} BCClass;

typedef struct BCObject {
	BCClassRef cls;
	atomic_int ref_count;
	BCAllocatorRef allocator;
} BCObject;

typedef struct BCAllocator {
	void* (* alloc)(size_t size, void* ctx);
	void (* free)(void* ptr, void* ctx);
	void* context;
} BCAllocator;

extern BCAllocatorRef const kBCDefaultAllocator;

BCObject* BCAllocRaw(BCClassRef cls, BCAllocatorRef alloc, size_t extraSize);
BCObject* BCRetain(BCObjectRef obj);
void BCRelease(BCObjectRef obj);
BCObject* BCCopy(BCObjectRef obj);
uint32_t BCHash(BCObjectRef obj);
bool BCEqual(BCObjectRef a, BCObjectRef b);
void BCLog(BCObjectRef obj, int indent);
bool BCIsKindOf(BCObjectRef obj, BCClassRef cls);
BCStringRef BCClassName(BCClassRef cls);

BCClassRef BCGetClass(BCObjectRef obj);

#endif //BCRUNTIME_BCOBJECT_H
