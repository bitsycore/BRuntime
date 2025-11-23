#include "BCObject.h"
#include "BCString.h"

#include <stdio.h>
#include <string.h>
#include <threads.h>
#include <time.h>

#include "Utilities/BCMemory.h"

// =========================================================
// MARK: Debug Tracking
// =========================================================

typedef struct BCObjectDebugNode {
	BCObjectRef obj;
	bool isFreed;
	BCObject copy;
	struct BCObjectDebugNode* next;
} BCObjectDebugNode;

static struct {
	mtx_t lock;
	BCObjectDebugNode* head;
	bool enabled;
	bool keepFreedObjects;
} ObjectDebugTracker;

void ___BCINTERNAL___ObjectDebugInitialize(void) {
	mtx_init(&ObjectDebugTracker.lock, mtx_plain);
	ObjectDebugTracker.head = NULL;
	ObjectDebugTracker.enabled = false;
	ObjectDebugTracker.keepFreedObjects = false;
}

void ___BCINTERNAL___ObjectDebugDeinitialize(void) {
	mtx_lock(&ObjectDebugTracker.lock);

	BCObjectDebugNode* node = ObjectDebugTracker.head;
	while (node) {
		BCObjectDebugNode* next = node->next;
		BCFree(node);
		node = next;
	}

	ObjectDebugTracker.head = NULL;
	mtx_unlock(&ObjectDebugTracker.lock);
	mtx_destroy(&ObjectDebugTracker.lock);
}

static void ObjectDebugTrack(const BCObjectRef obj) {
	if (!ObjectDebugTracker.enabled) return;

	mtx_lock(&ObjectDebugTracker.lock);

	BCObjectDebugNode* node = BCMalloc(sizeof(BCObjectDebugNode));
	node->obj = obj;
	node->copy = *obj;
	node->isFreed = false;
	node->next = ObjectDebugTracker.head;
	ObjectDebugTracker.head = node;

	mtx_unlock(&ObjectDebugTracker.lock);
}

static void ObjectDebugMarkFreed(const BCObjectRef obj) {
	if (!ObjectDebugTracker.enabled) return;

	mtx_lock(&ObjectDebugTracker.lock);

	BCObjectDebugNode* node = ObjectDebugTracker.head;
	while (node) {
		if (node->obj == obj) {
			if (ObjectDebugTracker.keepFreedObjects) {
				node->isFreed = true;
			} else {
				// Remove from list
				BCObjectDebugNode* prev = NULL;
				BCObjectDebugNode* curr = ObjectDebugTracker.head;
				while (curr) {
					if (curr == node) {
						if (prev) {
							prev->next = curr->next;
						} else {
							ObjectDebugTracker.head = curr->next;
						}
						BCFree(curr);
						break;
					}
					prev = curr;
					curr = curr->next;
				}
			}
			break;
		}
		node = node->next;
	}

	mtx_unlock(&ObjectDebugTracker.lock);
}

// =========================================================
// MARK: Allocator
// =========================================================

static void* AllocatorDefaultAlloc(const size_t size, const void* ctx) {
	(void)ctx;
	return BCMalloc(size);
}

static void AllocatorDefaultFree(void* ptr, const void* ctx) {
	(void)ctx;
	BCFree(ptr);
}

static BCAllocator _kBCAllocatorDefault = {AllocatorDefaultAlloc, AllocatorDefaultFree, NULL};
BCAllocatorRef const kBCAllocatorDefault = &_kBCAllocatorDefault;

// =========================================================
// MARK: Public
// =========================================================

BCObjectRef BCAllocObjectWithExtra(const BCClassRef cls, BCAllocatorRef alloc, const size_t extraBytes, const uint32_t flags) {
	if (!alloc) alloc = kBCAllocatorDefault;

	const BCObjectRef obj = alloc->alloc(cls->allocSize + extraBytes, alloc->context);
	obj->cls = cls;
	obj->flags = flags;
	obj->allocator = alloc;
	obj->ref_count = 1;

	ObjectDebugTrack(obj);

	return obj;
}

BCObjectRef BCAllocObject(const BCClassRef cls, const BCAllocatorRef alloc) {
	return BCAllocObjectWithExtra(cls, alloc, 0, BC_OBJECT_FLAG_REFCOUNT);
}

BCObjectRef BCRetain(const BCObjectRef obj) {
	if (!obj || !BC_FLAG_HAS(obj->flags, BC_OBJECT_FLAG_REFCOUNT)) return obj;
	atomic_fetch_add_explicit(&obj->ref_count, 1, memory_order_relaxed);
	return obj;
}

void BCRelease(const BCObjectRef obj) {
	if (!obj || !BC_FLAG_HAS(obj->flags, BC_OBJECT_FLAG_REFCOUNT)) return;
	const atomic_int old_count = atomic_fetch_sub(&obj->ref_count, 1);
	if (old_count == 1) {
		ObjectDebugMarkFreed(obj);
		if (obj->cls->dealloc) obj->cls->dealloc(obj);
		if (obj->allocator) obj->allocator->free(obj, obj->allocator->context);
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
	if (obj == NULL) return BCStringPooledLiteral("<null>");
	if (obj->cls->toString) return obj->cls->toString(obj);
	return BCStringCreate("<%s@%d>", BCStringCPtr(BCClassName(obj->cls)), BCHash(obj));
}

bool BCObjectIsClass(const BCObjectRef obj, const BCClassRef cls) {
	if (!obj || !cls) return false;
	return obj->cls == cls;
}

BCStringRef BCClassName(const BCClassRef cls) {
	return BCStringPooled(cls->name);
}

// =========================================================
// MARK: Debug Dump
// =========================================================

#if BC_SETTINGS_DEBUG_OBJECT_DUMP == 1

void BCObjectDebugSetEnabled(const bool enabled) {
	mtx_lock(&ObjectDebugTracker.lock);
	ObjectDebugTracker.enabled = enabled;
	mtx_unlock(&ObjectDebugTracker.lock);
}

void BCObjectDebugSetKeepFreed(const bool keepFreed) {
	mtx_lock(&ObjectDebugTracker.lock);
	ObjectDebugTracker.keepFreedObjects = keepFreed;
	mtx_unlock(&ObjectDebugTracker.lock);
}

extern const BCClassRef kBCStringClassRef;

static const char* FlagsToString(const BCClassRef cls, const uint32_t flags) {
	static char buffer[30];
	buffer[0] = '\0';

	if (BC_FLAG_HAS(flags, BC_OBJECT_FLAG_REFCOUNT)) strcat(buffer, "REF ");
	if (BC_FLAG_HAS(flags, BC_OBJECT_FLAG_HEAP)) strcat(buffer, "HEAP ");
	if (BC_FLAG_HAS(flags, BC_OBJECT_FLAG_STATIC)) strcat(buffer, "STATIC ");
	if (BC_FLAG_HAS(flags, BC_OBJECT_FLAG_POOLED)) strcat(buffer, "POOL ");
	if (BC_FLAG_HAS(flags, BC_OBJECT_FLAG_HAS_HASH)) strcat(buffer, "HASH ");

	if (cls == kBCStringClassRef) {
		if (flags & BC_OBJECT_FLAG_CLASS_MASK) {
			strcat(buffer, "STR(");
		}
		if (BC_FLAG_HAS(flags, BC_STRING_FLAG_POOLED)) strcat(buffer, " POOL");
		if (BC_FLAG_HAS(flags, BC_STRING_FLAG_STATIC)) strcat(buffer, " STATIC");
		if (flags & BC_OBJECT_FLAG_CLASS_MASK) {
			strcat(buffer, " ) ");
		}
	}

	if (buffer[0] == '\0') {
		strcpy(buffer, "NONE");
	} else {
		// Remove trailing space
		buffer[strlen(buffer) - 1] = '\0';
	}

	return buffer;
}

#define DGRAY "\033[48;5;234m"
#define BLACK "\033[48;5;235m"
#define RESET "\033[0m"
#define BOLD "\033[1m"

void BCObjectDebugDump(void) {
	mtx_lock(&ObjectDebugTracker.lock);
	const clock_t start = clock();

	// --------------------------------------------------------------------------
	// HEADER
	printf("\n"
		"                                                      "BOLD"Object Dump"RESET"\n"
		"┌"          "──────────────────┬────────────────────────┬──────────────────────────────┬──────────┬──────────────────┬─────────"     "┐\n"
		"│"DGRAY BOLD"     Address      │      Class Type        │             Flags            │ RefCount │    Allocator     │  Freed  "RESET"│\n"
		"├"BLACK     "──────────────────┼────────────────────────┼──────────────────────────────┼──────────┼──────────────────┼─────────"RESET"┤\n"
	);

	// Print entries
	size_t count = 0;
	size_t freedCount = 0;
	BCObjectDebugNode* node = ObjectDebugTracker.head;
	while (node) {
		const BCObjectRef obj = node->isFreed ? &node->copy : node->obj;
		if (node->isFreed) freedCount++;
		const char* className = obj->cls->name;
		const char* flags = FlagsToString(obj->cls, obj->flags);
		const int refCount = atomic_load(&obj->ref_count);

		// Truncate class name if too long
		char classDisplay[23];
		if (strlen(className) > 22) {
			snprintf(classDisplay, sizeof(classDisplay), "%.19s...", className);
		} else {
			snprintf(classDisplay, sizeof(classDisplay), "%s", className);
		}

		// Truncate flags if too long
		char flagsDisplay[41];
		if (strlen(flags) > 40) {
			snprintf(flagsDisplay, sizeof(flagsDisplay), "%.37s...", flags);
		} else {
			snprintf(flagsDisplay, sizeof(flagsDisplay), "%s", flags);
		}

		// Format allocator pointer
		char allocatorPtr[18];
		if (obj->allocator == kBCAllocatorDefault) {
			snprintf(allocatorPtr, sizeof(allocatorPtr), "DEFAULT");
		} else if (obj->allocator) {
			snprintf(allocatorPtr, sizeof(allocatorPtr), "%p", (void*)obj->allocator);
		} else {
			snprintf(allocatorPtr, sizeof(allocatorPtr), "NULL");
		}

		if (count % 2 == 0) {
			printf("│"DGRAY" %-16p │ %-22s │ %-28s │ %-8d │ %-16s │ %s "RESET"│\n",
				   (void*)node->obj,
				   classDisplay,
				   flagsDisplay,
				   node->isFreed ? 0 : refCount,
				   allocatorPtr,
				   node->isFreed ? " FREED " : "       "
			);
		}
		else {
			printf("│"BLACK" %-16p │ %-22s │ %-28s │ %-8d │ %-16s │ %s "RESET"│\n",
				   (void*)node->obj,
				   classDisplay,
				   flagsDisplay,
				   node->isFreed ? 0 : refCount,
				   allocatorPtr,
				   node->isFreed ? " FREED " : "       "
			);
		}
		count++;
		node = node->next;
	}

	const clock_t end = clock();
	const double elapsed = (double)(end - start) / CLOCKS_PER_SEC * 1000;

	// --------------------------------------------------------------------------
	// FOOTER
	printf(
		"└──────────────────┴────────────────────────┴──────────────────────────────┴──────────┴──────────────────┴─────────┘\n"
		"    %zu entr%s (%zu freed, %fms)\n\n",
		count,
		count == 1 ? "y" : "ies",
		freedCount,
		elapsed
	);

	mtx_unlock(&ObjectDebugTracker.lock);
}

#endif