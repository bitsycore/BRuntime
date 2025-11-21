#include "BCObject.h"
#include "BCString.h"

#include <stdio.h>
#include <stdlib.h>

// =========================================================
// MARK: Allocator
// =========================================================

static void* _StdAlloc(const size_t size, const void* ctx) {
	(void)ctx;
	return malloc(size);
}

static void _StdFree(void* ptr, const void* ctx) {
	(void)ctx;
	free(ptr);
}

static BCAllocator _kDefaultAlloc = {_StdAlloc, _StdFree, NULL};
BCAllocatorRef const kBCDefaultAllocator = &_kDefaultAlloc;

// =========================================================
// MARK: Public
// =========================================================

BCObjectRef BCObjectAlloc(const BCClassRef cls, BCAllocatorRef alloc) {

	if (!alloc) alloc = kBCDefaultAllocator;

	const BCObjectRef obj = alloc->alloc(cls->bytes_size, alloc->context);
	obj->cls = cls;
	obj->allocator = alloc;
	obj->ref_count = 1;

	return obj;
}

BCObjectRef BCRetain(const BCObjectRef obj) {
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
		if (obj->allocator != NULL) {
			obj->allocator->free(obj, obj->allocator->context);
		}
	}
}

BCObjectRef BCObjectCopy(BCObjectRef obj) {
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

void BCDescription(BCObjectRef obj, int indent) {
	if (obj == NULL) return;
	if (obj->cls->description) {
		obj->cls->description(obj, indent);
	} else {
		printf("<BCObject %p>", (void*)obj);
	}
}

bool BCObjectIsClass(BCObjectRef obj, BCClassRef cls) {
	if (!obj || !cls) return false;
	return (obj->cls == cls);
}

BCStringRef BCClassName(BCClassRef cls) {
	return BCStringConst(cls->name);
}