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
BCAllocator* const kBCDefaultAllocator = &_kDefaultAlloc;

// =========================================================
// MARK: Public
// =========================================================

BCObject* BCAllocRaw(const BCClass* cls, BCAllocator* alloc, size_t extraSize) {
	if (!alloc) alloc = kBCDefaultAllocator;
	size_t totalSize = sizeof(BCObject) + extraSize;

	BCObject* obj = alloc->alloc(totalSize, alloc->context);
	obj->isa = cls;
	obj->allocator = alloc;
	atomic_init(&obj->ref_count, 1);
	return obj;
}

BCObject* BCRetain(BCObject* obj) {
	if (!obj) return NULL;
	atomic_fetch_add(&obj->ref_count, 1);
	return obj;
}

void BCRelease(BCObject* obj) {
	if (!obj) return;
	atomic_int old_count = atomic_fetch_sub(&obj->ref_count, 1);
	if (old_count <= 1) {
		if (obj->isa->dealloc) {
			obj->isa->dealloc(obj);
		}
		obj->allocator->free(obj, obj->allocator->context);
	}
}

BCObject* BCCopy(const BCObject* obj) {
	if (!obj) return NULL;
	if (obj->isa->copy) {
		return obj->isa->copy(obj);
	}
	// Default behavior: Retain (assumes immutable if no copy provided)
	return BCRetain((BCObject*)obj);
}

uint32_t BCHash(const BCObject* obj) {
	if (!obj) return 0;
	if (obj->isa->hash) return obj->isa->hash(obj);
	return (uint32_t)(uintptr_t)obj;
}

bool BCEqual(const BCObject* a, const BCObject* b) {
	if (a == b) return true;
	if (!a || !b) return false;
	if (a->isa != b->isa) return false;
	if (a->isa->equal) return a->isa->equal(a, b);
	return false;
}

void BCLog(const BCObject* obj) {
	if (obj == NULL) return;
	if (obj->isa->description) {
		obj->isa->description(obj, 0);
		printf("\n");
	} else {
		printf("<BCObject %p>\n", (void*)obj);
	}
}

bool BCIsKindOf(const BCObject* obj, const BCClass* cls) {
	if (!obj || !cls) return false;
	return (obj->isa == cls);
}

BCString* BCClassName(const BCClass* cls) {
	return BCStringConst(cls->name);
}