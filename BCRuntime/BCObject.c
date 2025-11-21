#include "BCObject.h"
#include "BCString.h"

#include <stdio.h>
#include <stdlib.h>

// =========================================================
// MARK: Allocator
// =========================================================

static void* DefaultAlloc(const size_t size, const void* ctx) {
	(void)ctx;
	return malloc(size);
}

static void DefaultFree(void* ptr, const void* ctx) {
	(void)ctx;
	free(ptr);
}

static BCAllocator _kBCAllocatorDefault = {DefaultAlloc, DefaultFree, NULL};
BCAllocatorRef const kBCAllocatorDefault = &_kBCAllocatorDefault;

// =========================================================
// MARK: Public
// =========================================================

BCObjectRef BCObjectAlloc(const BCClassRef cls, BCAllocatorRef alloc) {
	if (!alloc) alloc = kBCAllocatorDefault;

	const BCObjectRef obj = alloc->alloc(cls->allocSize, alloc->context);
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

void BCRelease(const BCObjectRef obj) {
	if (!obj) return;
	const atomic_int old_count = atomic_fetch_sub(&obj->ref_count, 1);
	if (old_count <= 1) {
		if (obj->cls->dealloc) {
			obj->cls->dealloc(obj);
		}
		if (obj->allocator != NULL) {
			obj->allocator->free(obj, obj->allocator->context);
		}
	}
}

BCObjectRef BCObjectCopy(const BCObjectRef obj) {
	if (!obj) return NULL;
	if (obj->cls->copy) {
		return obj->cls->copy(obj);
	}
	// Retain if no copy method,
	// assume it is immutable.
	return BCRetain(obj);
}

uint32_t BCHash(const BCObjectRef obj) {
	if (!obj) return 0;
	if (obj->cls->hash) return obj->cls->hash(obj);
	return (uint32_t)(uintptr_t)obj;
}

bool BCEqual(const BCObjectRef a, const BCObjectRef b) {
	if (a == b) return true;
	if (!a || !b) return false;
	if (a->cls != b->cls) return false;
	if (a->cls->equal) return a->cls->equal(a, b);
	return false;
}

BCStringRef BCToString(const BCObjectRef obj) {
	if (obj == NULL) return BCStringConst("<null>");
	if (obj->cls->toString) return obj->cls->toString(obj);
	return BCStringCreate("<%s@%d>", BCStringGetCString(BCClassName(obj->cls)), BCHash(obj));
}

bool BCObjectIsClass(const BCObjectRef obj, const BCClassRef cls) {
	if (!obj || !cls) return false;
	return obj->cls == cls;
}

BCStringRef BCClassName(const BCClassRef cls) {
	return BCStringConst(cls->name);
}
