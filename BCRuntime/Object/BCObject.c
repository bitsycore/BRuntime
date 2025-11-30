#include "BCObject.h"

#include "BCMap.h"
#include "BCString.h"
#include "../Core/BCAllocator.h"
#include "../Core/BCClass.h"
#include "../Utilities/BC_Compat.h"
#include "../Utilities/BC_Memory.h"
#include "../Utilities/BC_Threads.h"

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

BCObjectRef BCObjectAllocWithConfig(const BCAllocatorRef alloc, const BCClassId cls, const size_t extraBytes, const uint16_t flags) {
	const BCClassRef class = BCClassIdGetRef(cls);

	const BCObjectRef obj = BCAllocatorAlloc(alloc, class->allocSize + extraBytes);

	obj->cls = cls;
	obj->flags = flags;
	obj->ref_count = 1;

	BCObjectSetAllocator(obj, alloc);

	ObjectDebugTrack(obj);

	return obj;
}

BCObjectRef BCObjectAlloc(const BCAllocatorRef alloc, const BCClassId cls) {
	return BCObjectAllocWithConfig(alloc, cls, 0, BC_OBJECT_DEFAULT_FLAGS);
}

BCObjectRef BCRetain(const BCObjectRef obj) {
	if (obj == NULL || !BC_FLAG_HAS(obj->flags, BC_OBJECT_FLAG_REFCOUNT) ||
		BC_FLAG_HAS(obj->flags, BC_OBJECT_FLAG_CONSTANT))
		return obj;

	BC_atomic_fetch_add(&obj->ref_count, 1);

	return obj;
}

void BCRelease(const BCObjectRef obj) {
	if (obj == NULL || !BC_FLAG_HAS(obj->flags, BC_OBJECT_FLAG_REFCOUNT) ||
		BC_FLAG_HAS(obj->flags, BC_OBJECT_FLAG_CONSTANT))
		return;

	const BC_atomic_uint16 old_count = BC_atomic_fetch_sub(&obj->ref_count, 1);

	if (old_count == 1) {
		const BCClassRef cls = BCClassIdGetRef(obj->cls);
		if (cls && cls->dealloc)
			cls->dealloc(obj);
		if (BC_FLAG_HAS(obj->flags, BC_OBJECT_FLAG_NON_SYSTEM_ALLOCATOR)) {
			const BCAllocatorRef allocator = BCObjectGetAllocator(obj);
			BCAllocatorFree(allocator, obj);
		}
		else {
			BCAllocatorFree(NULL, obj);
		}
		ObjectDebugMarkFreed(obj);
	}
}

BCObjectRef BCCopy(const BCObjectRef obj) {
	if (!obj) return NULL;
	const BCClassRef cls = BCClassIdGetRef(obj->cls);
	if (cls && cls->copy) return cls->copy(obj);
	// Retain if no copy method,
	// assume it is immutable.
	return BCRetain(obj);
}

uint32_t BCHash(const BCObjectRef obj) {
	if (!obj) return 0;
	const BCClassRef cls = BCClassIdGetRef(obj->cls);
	if (cls && cls->hash) return cls->hash(obj);
	return (uint32_t)(uintptr_t)obj;
}

BC_bool BCEqual(const BCObjectRef a, const BCObjectRef b) {
	if (a == b) return BC_true;
	if (!a || !b) return BC_false;
	if (a->cls != b->cls) return BC_false;
	const BCClassRef cls = BCClassIdGetRef(a->cls);
	if (cls && cls->equal) return cls->equal(a, b);
	return BC_false;
}

BCStringRef BCToString(const BCObjectRef obj) {
	if (obj == NULL) return BCStringPooledLiteral("<null>");

	const BCClassRef cls = BCClassIdGetRef(obj->cls);

	if (cls && cls->toString) return cls->toString(obj);
	if (cls) return BCStringCreate("<%s@%8x>", BCStringCPtr(BCClassIdName(obj->cls)), BCHash(obj));

	return BCStringPooledLiteral("<invalid>");
}

BC_bool BCIsClass(const BCObjectRef obj, const BCClassId cls) {
	if (!obj) return BC_false;
	return obj->cls = cls;
}

BCClassRef BCObjectClass(const BCObjectRef obj) {
	if (!obj) return NULL;
	return BCClassIdGetRef(obj->cls);
}

BCClassId BCObjectClassId(const BCObjectRef obj) {
	if (!obj) return BC_CLASS_ID_INVALID;
	return obj->cls;
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
	BC_SPINLOCK_MAYBE(lock)
	BCObjectDebugNode* head;
	BC_atomic_bool enabled;
	BC_atomic_bool keepFreedObjects;
} ObjectDebugTracker;

void ___BCINTERNAL___ObjectDebugInitialize(void) {
	BCSpinlockInit(&ObjectDebugTracker.lock);
	ObjectDebugTracker.head = NULL;
	ObjectDebugTracker.enabled = BC_false;
	ObjectDebugTracker.keepFreedObjects = BC_false;
}

void ___BCINTERNAL___ObjectDebugDeinitialize(void) {
	BCSpinlockLock(&ObjectDebugTracker.lock);

	BCObjectDebugNode* node = ObjectDebugTracker.head;
	while (node) {
		BCObjectDebugNode* next = node->next;
		BCFree(node);
		node = next;
	}

	ObjectDebugTracker.head = NULL;
	BCSpinlockUnlock(&ObjectDebugTracker.lock);
	BCSpinlockDestroy(&ObjectDebugTracker.lock);
}

static void ObjectDebugTrack(const BCObjectRef obj) {
	if (!ObjectDebugTracker.enabled)
		return;

	BCSpinlockLock(&ObjectDebugTracker.lock);

	BCObjectDebugNode* node = BCMalloc(sizeof(BCObjectDebugNode));
	node->obj = obj;
	node->copy = *obj;
	node->next = ObjectDebugTracker.head;
	ObjectDebugTracker.head = node;

	BCSpinlockUnlock(&ObjectDebugTracker.lock);
}

static void ObjectDebugMarkFreed(const BCObjectRef obj) {
	if (!ObjectDebugTracker.enabled)
		return;

	BCSpinlockLock(&ObjectDebugTracker.lock);

	BCObjectDebugNode* prev = NULL;
	BCObjectDebugNode* curr = ObjectDebugTracker.head;

	while (curr) {
		if (curr->obj == obj) {
			curr->obj = NULL;

			if (!ObjectDebugTracker.keepFreedObjects) {
				if (prev) {
					prev->next = curr->next;
				}
				else {
					ObjectDebugTracker.head = curr->next;
				}
				BCFree(curr);
			}
			break;
		}
		prev = curr;
		curr = curr->next;
	}

	BCSpinlockUnlock(&ObjectDebugTracker.lock);
}

static const char* FlagsToString(const BCClassId cls, const uint16_t flags) {
	static char buffer[30];
	buffer[0] = '\0';

	if (BC_FLAG_HAS(flags, BC_OBJECT_FLAG_REFCOUNT))
		BC_strcat_s(buffer, sizeof(buffer), "REF ");
	if (BC_FLAG_HAS(flags, BC_OBJECT_FLAG_CONSTANT))
		BC_strcat_s(buffer, sizeof(buffer), "CST ");
	if (BC_FLAG_HAS(flags, BC_OBJECT_FLAG_NON_SYSTEM_ALLOCATOR))
		BC_strcat_s(buffer, sizeof(buffer), "ALL ");
	if (BC_FLAG_HAS(flags, BC_OBJECT_FLAG_INLINED))
		BC_strcat_s(buffer, sizeof(buffer), "INL ");

	if (cls == BCStringClassId()) {
		if (flags & BC_OBJECT_FLAG_CLASS_MASK) {
			BC_strcat_s(buffer, sizeof(buffer), "STR(");
		}
		if (BC_FLAG_HAS(flags, BC_STRING_FLAG_POOLED))
			BC_strcat_s(buffer, sizeof(buffer), " POL");
		if (BC_FLAG_HAS(flags, BC_STRING_FLAG_STATIC))
			BC_strcat_s(buffer, sizeof(buffer), " CST");
		if (flags & BC_OBJECT_FLAG_CLASS_MASK) {
			BC_strcat_s(buffer, sizeof(buffer), " ) ");
		}
	}

	if (cls == BCMapClassId()) {
		if (flags & BC_OBJECT_FLAG_CLASS_MASK) {
			BC_strcat_s(buffer, sizeof(buffer), "MAP(");
		}
		if (BC_FLAG_HAS(flags, BC_MAP_FLAG_MUTABLE))
			BC_strcat_s(buffer, sizeof(buffer), " MUT");
		if (flags & BC_OBJECT_FLAG_CLASS_MASK) {
			BC_strcat_s(buffer, sizeof(buffer), " ) ");
		}
	}

	if (buffer[0] == '\0') {
		BC_strcat_s(buffer, sizeof(buffer), "NONE");
	}
	else {
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
	BCSpinlockLock(&ObjectDebugTracker.lock);
	const clock_t start = clock();

	// --------------------------------------------------------------------------
	// HEADER
	printf("\n"
		"                                                      " BOLD
		"Object Dump" RESET "\n"
		"┌──────────────────┬──────────────────┬──────────────────────┬──────────┬───────────┬──────────────────────────────┐\n"
		"│" DGRAY BOLD "     Address      " RESET DGRAY
		"│" BOLD    "      Class       "    RESET DGRAY
		"│" BOLD  "         Flags        "  RESET DGRAY
		"│" BOLD        " RefCount "        RESET DGRAY
		"│" BOLD        " Allocator "       RESET DGRAY
		"│" BOLD "          Description         " RESET "│\n"
		"├" BLACK "──────────────────┼──────────────────┼──────────────────────┼──────────┼───────────┼──────────────────────────────" RESET "┤\n"
	);

	// Print entries
	size_t count = 0;
	size_t freedCount = 0;
	BCObjectDebugNode* node = ObjectDebugTracker.head;
	while (node) {
		const BCObjectRef obj = node->obj == NULL ? &node->copy : node->obj;
		if (node->obj == NULL)
			freedCount++;
		const BCClassRef cls = BCClassIdGetRef(obj->cls);
		const char* className = cls ? cls->name : "<unknown>";
		const char* flags = FlagsToString(obj->cls, obj->flags);
		const int refCount = BC_atomic_load(&obj->ref_count);

		// Truncate class name if too long
		char classDisplay[23];
		if (strlen(className) > 22) {
			snprintf(classDisplay, sizeof(classDisplay), "%.19s...", className);
		}
		else {
			snprintf(classDisplay, sizeof(classDisplay), "%s", className);
		}

		// Truncate flags if too long
		char flagsDisplay[41];
		if (strlen(flags) > 40) {
			snprintf(flagsDisplay, sizeof(flagsDisplay), "%.37s...", flags);
		}
		else {
			snprintf(flagsDisplay, sizeof(flagsDisplay), "%s", flags);
		}

		// Format allocator pointer
		char allocatorPtr[18];
		if (!BC_FLAG_HAS(obj->flags, BC_OBJECT_FLAG_NON_SYSTEM_ALLOCATOR)) {
			snprintf(allocatorPtr, sizeof(allocatorPtr), "DEFAULT");
		}
		else {
			const BCAllocatorRef allocator = BCObjectGetAllocator(obj);
			snprintf(allocatorPtr, sizeof(allocatorPtr), "%p", (void*)allocator);
		}
		const char* color = count % 2 == 0 ? DGRAY : BLACK;
		if (node->obj == NULL) {
			printf("│%s %-16s │ %-16s │ %-20s │ %-8d │ %-9s │ %-28s " RESET "│\n",
				   color, "       -        ", classDisplay, flagsDisplay, 0,
				   allocatorPtr, "");
		}
		else {
			const BC_bool enabledOld = BC_atomic_load(&ObjectDebugTracker.enabled);
			BCObjectDebugSetEnabled(BC_false);
			const BCStringRef description = BCToString(node->obj);
			printf("│%s %-16p │ %-16s │ %-20s │ %-8d │ %-9s │ %-28s " RESET "│\n",
				   color, (void*)node->obj, classDisplay, flagsDisplay, refCount,
				   allocatorPtr, BCStringCPtr(description));
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
	printf("└──────────────────┴──────────────────┴──────────────────────┴───────"
		   "───┴───────────┴──────────────────────────────┘\n"
		   "    %zu entr%s (%zu freed, %fms)\n\n",
		   count, count == 1 ? "y" : "ies", freedCount, elapsed);

	BCSpinlockUnlock(&ObjectDebugTracker.lock);
}
#else
void ___BCINTERNAL___ObjectDebugInitialize() {}
void ___BCINTERNAL___ObjectDebugDeinitialize() {}
#endif
