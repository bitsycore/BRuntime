#include "BCObject.h"
#include "BCString.h"

#include <stdlib.h>
#include <stdio.h>

// =========================================================
// MARK: Allocator
// =========================================================

static void* _StdAlloc(size_t s, void* c) {
	(void)c;
	return calloc(1, s);
}

static void _StdFree(void* p, void* c) {
	(void)c;
	free(p);
}

static BCAllocator _kDefaultAlloc = {_StdAlloc, _StdFree, NULL};
BCAllocatorRef const kBCDefaultAllocator = &_kDefaultAlloc;

// =========================================================
// MARK: Public
// =========================================================

BCObjectRef BCAllocRaw(BCClassRef cls, BCAllocatorRef alloc, size_t extraSize) {
	if (!alloc) alloc = kBCDefaultAllocator;
	size_t totalSize = sizeof(BCObject) + extraSize;

	BCObjectRef obj = alloc->alloc(totalSize, alloc->context);
	obj->cls = cls;
	obj->allocator = alloc;
	atomic_init(&obj->ref_count, 1);
	return obj;
}

BCObjectRef BCRetain(BCObjectRef obj) {
	if (!obj) return NULL;
	atomic_fetch_add(&obj->ref_count, 1);
	return obj;
}

void BCRelease(BCObjectRef obj) {
	if (!obj) return;
	atomic_int old_count = atomic_fetch_sub(&obj->ref_count, 1);
	if (old_count <= 1) {
		if (obj->cls->dealloc) {
			obj->cls->dealloc(obj);
		}
		obj->allocator->free(obj, obj->allocator->context);
	}
}

BCObjectRef BCCopy(BCObjectRef obj) {
	if (!obj) return NULL;
	if (obj->cls->copy) {
		return obj->cls->copy(obj);
	}
	// Default behavior: Retain (assumes immutable if no copy provided)
	return BCRetain((BCObjectRef)obj);
}

uint32_t BCHash(BCObjectRef obj) {
	if (!obj) return 0;
	if (obj->cls->hash) return obj->cls->hash(obj);
	return (uint32_t)(uintptr_t)obj;
}

bool BCEqual(BCObjectRef a, BCObjectRef b) {
	if (a == b) return true;
	if (!a || !b) return false;
	if (a->cls != b->cls) return false;
	if (a->cls->equal) return a->cls->equal(a, b);
	return false;
}

void BCLog(BCObjectRef obj, int indent) {
	if (obj == NULL) return;
	if (obj->cls->description) {
		obj->cls->description(obj, indent);
		printf("\n");
	} else {
		printf("<BCObject %p>\n", (void*)obj);
	}
}

bool BCIsKindOf(BCObjectRef obj, BCClassRef cls) {
	if (!obj || !cls) return false;
	return (obj->cls == cls);
}

BCStringRef BCClassName(BCClassRef cls) {
	return BCStringConst(cls->name);
}