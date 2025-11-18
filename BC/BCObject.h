#ifndef BCRUNTIME_BCOBJECT_H
#define BCRUNTIME_BCOBJECT_H

#include <stdatomic.h>
#include "BCTypes.h"

struct BCAllocator {
	void* (* alloc)(size_t size, void* ctx);
	void (* free)(void* ptr, void* ctx);
	void* context;
};

extern BCAllocator* const kBCDefaultAllocator;

// CLASS
typedef struct BCClass {
	const char* name;
	BCDeallocProc dealloc;
	BCHashProc hash;
	BCEqualProc equal;
	BCDescProc description;
	BCCopyProc copy;
} BCClass;

// BASE OBJECT
typedef struct BCObject {
	const BCClass* isa;
	atomic_int ref_count;
	BCAllocator* allocator;
} BCObject;

BCObject* BCAllocRaw(const BCClass* cls, BCAllocator* alloc, size_t extraSize);
BCObject* BCRetain(BCObject* obj);
void      BCRelease(BCObject* obj);
BCObject* BCCopy(const BCObject* obj);
uint32_t  BCHash(const BCObject* obj);
bool      BCEqual(const BCObject* a, const BCObject* b);
void      BCLog(const BCObject* obj);
bool      BCIsKindOf(const BCObject* obj, const BCClass* cls);
BCString* BCClassName(const BCClass* cls);

#endif //BCRUNTIME_BCOBJECT_H
