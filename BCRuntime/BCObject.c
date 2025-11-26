#include "BCObject.h"

#include "BCAllocator.h"
#include "BCClass.h"
#include "String/BCString.h"
#include "Utilities/BCMemory.h"
#include "Utilities/BCThreads.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

// =========================================================
// MARK: Forward
// =========================================================

#if BC_SETTINGS_DEBUG_OBJECT_DUMP == 1
static void ObjectDebugTrack(BCObjectRef obj);
static void ObjectDebugMarkFreed(BCObjectRef obj);
#else
#define ObjectDebugTrack(obj)
#define ObjectDebugMarkFreed(obj)
#endif

// =========================================================
// MARK: Public
// =========================================================

BCObjectRef BCObjectAllocWithConfig(const BCClassRef cls, const BCAllocatorRef alloc, const size_t extraBytes, const uint16_t flags) {
	BC_bool useDefaultAllocator = 0;
	if (alloc == NULL || alloc == BCAllocatorGetDefault()) {
		useDefaultAllocator = BC_true;
	}

	void* obj = BCAllocatorAlloc(
		alloc,
		(useDefaultAllocator ? 0 : sizeof(BCAllocatorRef))
			+ cls->allocSize
			+ extraBytes
	);

	if (!useDefaultAllocator) {
		obj = alloc;
		obj += sizeof(BCAllocatorRef);
	}

	const BCObjectRef objRef = obj;
	objRef->cls = cls;
	objRef->flags = flags;
	objRef->ref_count = 1;
	if (!useDefaultAllocator) BC_FLAG_SET(objRef->flags, BC_OBJECT_FLAG_NON_DEFAULT_ALLOCATOR);

	ObjectDebugTrack(obj);

	return objRef;
}

BCObjectRef BCObjectAlloc(const BCAllocatorRef alloc, const BCClassRef cls) {
	return BCObjectAllocWithConfig(cls, alloc, 0, BC_OBJECT_FLAG_REFCOUNT);
}

BCObjectRef BCRetain(const BCObjectRef obj) {
	if (
		obj == NULL
		|| !BC_FLAG_HAS(obj->flags, BC_OBJECT_FLAG_REFCOUNT)
		|| BC_FLAG_HAS(obj->flags, BC_OBJECT_FLAG_CONSTANT)
	) return obj;

	BC_atomic_fetch_add(&obj->ref_count, 1);

	return obj;
}

void BCRelease(const BCObjectRef obj) {
	if (
		obj == NULL
		|| !BC_FLAG_HAS(obj->flags, BC_OBJECT_FLAG_REFCOUNT)
		|| BC_FLAG_HAS(obj->flags, BC_OBJECT_FLAG_CONSTANT)
	) return;

	const BC_atomic_uint16 old_count = BC_atomic_fetch_sub(&obj->ref_count, 1);

	if (old_count == 1) {
		if (obj->cls->dealloc) obj->cls->dealloc(obj);
		if (BC_FLAG_HAS(obj->flags, BC_OBJECT_FLAG_NON_DEFAULT_ALLOCATOR)) {
			const BCAllocatorRef allocator = BCObjectGetAllocator(obj);
			BCAllocatorFree(allocator, obj);
		} else {
			BCAllocatorFree(NULL, obj);
		}
		ObjectDebugMarkFreed(obj);
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

BC_bool BCEqual(const BCObjectRef a, const BCObjectRef b) {
	if (a == b) return BC_true;
	if (!a || !b) return BC_false;
	if (a->cls != b->cls) return BC_false;
	if (a->cls->equal) return a->cls->equal(a, b);
	return BC_false;
}

BCStringRef BCToString(const BCObjectRef obj) {
	if (obj == NULL) return BCStringPooledLiteral("<null>");
	if (obj->cls->toString) return obj->cls->toString(obj);
	return BCStringCreate("<%s@%d>", BCStringCPtr(BCClassName(obj->cls)), BCHash(obj));
}

BC_bool BCObjectIsClass(const BCObjectRef obj, const BCClassRef cls) {
	if (!obj || !cls) return BC_false;
	return obj->cls == cls;
}

// =========================================================
// MARK: Debug Tracking
// =========================================================

#if BC_SETTINGS_DEBUG_OBJECT_DUMP == 1

typedef struct BCObjectDebugNode {
	BCObjectRef obj;
	BCObject copy;
	struct BCObjectDebugNode* next;
} BCObjectDebugNode;

static struct {
	BC_MUTEX_MAYBE(lock)
	BCObjectDebugNode* head;
	BC_atomic_bool enabled;
	BC_atomic_bool keepFreedObjects;
} ObjectDebugTracker;

void ___BCINTERNAL___ObjectDebugInitialize(void) {
	BCMutexInit(&ObjectDebugTracker.lock);
	ObjectDebugTracker.head = NULL;
	ObjectDebugTracker.enabled = BC_false;
	ObjectDebugTracker.keepFreedObjects = BC_false;
}

void ___BCINTERNAL___ObjectDebugDeinitialize(void) {
	BCMutexLock(&ObjectDebugTracker.lock);

	BCObjectDebugNode* node = ObjectDebugTracker.head;
	while (node) {
		BCObjectDebugNode* next = node->next;
		BCFree(node);
		node = next;
	}

	ObjectDebugTracker.head = NULL;
	BCMutexUnlock(&ObjectDebugTracker.lock);
	BCMutexDestroy(&ObjectDebugTracker.lock);
}

static void ObjectDebugTrack(const BCObjectRef obj) {
	if (!ObjectDebugTracker.enabled) return;

	BCMutexLock(&ObjectDebugTracker.lock);

	BCObjectDebugNode* node = BCMalloc(sizeof(BCObjectDebugNode));
	node->obj = obj;
	node->copy = *obj;
	node->next = ObjectDebugTracker.head;
	ObjectDebugTracker.head = node;

	BCMutexUnlock(&ObjectDebugTracker.lock);
}

static void ObjectDebugMarkFreed(const BCObjectRef obj) {
	if (!ObjectDebugTracker.enabled) return;

	BCMutexLock(&ObjectDebugTracker.lock);

	BCObjectDebugNode* prev = NULL;
	BCObjectDebugNode* curr = ObjectDebugTracker.head;

	while (curr) {
		if (curr->obj == obj) {
			curr->obj = NULL;

			if (!ObjectDebugTracker.keepFreedObjects) {
				if (prev) {
					prev->next = curr->next;
				} else {
					ObjectDebugTracker.head = curr->next;
				}
				BCFree(curr);
			}
			break;
		}
		prev = curr;
		curr = curr->next;
	}

	BCMutexUnlock(&ObjectDebugTracker.lock);
}

static const char* FlagsToString(const BCClassRef cls, const uint16_t flags) {
	static char buffer[30];
	buffer[0] = '\0';

	if (BC_FLAG_HAS(flags, BC_OBJECT_FLAG_REFCOUNT)) strcat(buffer, "REF ");
	if (BC_FLAG_HAS(flags, BC_OBJECT_FLAG_CONSTANT)) strcat(buffer, "CST ");

	if (cls == kBCStringClassRef) {
		if (flags & BC_OBJECT_FLAG_CLASS_MASK) {
			strcat(buffer, "STR(");
		}
		if (BC_FLAG_HAS(flags, BC_STRING_FLAG_POOLED)) strcat(buffer, " POOL");
		if (BC_FLAG_HAS(flags, BC_STRING_FLAG_STATIC)) strcat(buffer, " CST");
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

// =========================================================
// MARK: Public Object Debug
// =========================================================

void BCObjectDebugSetEnabled(const BC_bool enabled) {
	BC_atomic_store(&ObjectDebugTracker.enabled, enabled);
}

void BCObjectDebugSetKeepFreed(const BC_bool keepFreed) {
	BC_atomic_store(&ObjectDebugTracker.keepFreedObjects, keepFreed);
}

#define DGRAY "\033[48;5;234m"
#define BLACK "\033[48;5;235m"
#define RESET "\033[0m"
#define BOLD "\033[1m"

void BCObjectDebugDump(void) {
	BCMutexLock(&ObjectDebugTracker.lock);
	const clock_t start = clock();

	// --------------------------------------------------------------------------
	// HEADER
	printf("\n"
		"                                                      "BOLD"Object Dump"RESET"\n"
		"┌"          "──────────────────"           "┬"    "──────────────────"           "┬"    "──────────────────────"           "┬"    "──────────"           "┬"    "───────────"           "┬"    "──────────────────────────────"     "┐\n"
		"│"DGRAY BOLD"     Address      "RESET DGRAY"│"BOLD"      Class       "RESET DGRAY"│"BOLD"         Flags        "RESET DGRAY"│"BOLD" RefCount "RESET DGRAY"│"BOLD" Allocator "RESET DGRAY"│"BOLD"          Description         "RESET"│\n"
		"├"BLACK     "──────────────────"           "┼"    "──────────────────"           "┼"    "──────────────────────"           "┼"    "──────────"           "┼"    "───────────"           "┼"    "──────────────────────────────"RESET"┤\n"
	);

	// Print entries
	size_t count = 0;
	size_t freedCount = 0;
	BCObjectDebugNode* node = ObjectDebugTracker.head;
	while (node) {
		const BCObjectRef obj = node->obj == NULL ? &node->copy : node->obj;
		if (node->obj == NULL) freedCount++;
		const char* className = obj->cls->name;
		const char* flags = FlagsToString(obj->cls, obj->flags);
		const int refCount = BC_atomic_load(&obj->ref_count);

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
		if (!BC_FLAG_HAS(obj->flags, BC_OBJECT_FLAG_NON_DEFAULT_ALLOCATOR)) {
			snprintf(allocatorPtr, sizeof(allocatorPtr), "DEFAULT");
		} else {
			const BCAllocatorRef allocator = BCObjectGetAllocator(obj);
			snprintf(allocatorPtr, sizeof(allocatorPtr), "%p", (void*)allocator);
		}
		const char* color = count % 2 == 0 ? DGRAY : BLACK;
		if (node->obj == NULL) {
			printf("│%s %-16s │ %-16s │ %-20s │ %-8d │ %-9s │ %-28s "RESET"│\n",
				color,
				"       -        ",
				classDisplay,
				flagsDisplay,
				0,
				allocatorPtr,
				""
			);
		} else {
			const BC_bool enabledOld = BC_atomic_load(&ObjectDebugTracker.enabled);
			BCObjectDebugSetEnabled(BC_false);
			const BCStringRef description = BCToString(node->obj);
			printf("│%s %-16p │ %-16s │ %-20s │ %-8d │ %-9s │ %-28s "RESET"│\n",
				color,
				(void *)node->obj,
				classDisplay,
				flagsDisplay,
				refCount,
				allocatorPtr,
				BCStringCPtr(description)
			);
			BCRelease($OBJ description);
			BCObjectDebugSetEnabled(enabledOld);
		}
		count++;
		node = node->next;
	}

	const clock_t end = clock();
	const double elapsed = (double)(end - start) / CLOCKS_PER_SEC * 1000;

	// --------------------------------------------------------------------------
	// FOOTER
	printf(
		"└──────────────────┴──────────────────┴──────────────────────┴──────────┴───────────┴──────────────────────────────┘\n"
		"    %zu entr%s (%zu freed, %fms)\n\n",
		count,
		count == 1 ? "y" : "ies",
		freedCount,
		elapsed
	);

	BCMutexUnlock(&ObjectDebugTracker.lock);
}
#else
void ___BCINTERNAL___ObjectDebugInitialize() {}
void ___BCINTERNAL___ObjectDebugDeinitialize() {}
#endif