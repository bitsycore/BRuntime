#include "BO_Object.h"

#include "BCore/Memory/BC_Allocator.h"
#include "BCore/Memory/BC_Memory.h"
#include "BCore/Strings/BC_StringCompat.h"
#include "BCore/Thread/BC_Threads.h"

#include "BO_Map.h"
#include "BO_String.h"
#include "../BF_Class.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

// =========================================================
// MARK: Forward
// =========================================================

#if BC_SETTINGS_DEBUG_OBJECT_DUMP == 1
static void ObjectDebugTrack(BO_ObjectRef obj);
static void ObjectDebugMarkFreed(BO_ObjectRef obj);
#else
#define ObjectDebugTrack(obj)
#define ObjectDebugMarkFreed(obj)
#endif

// =========================================================
// MARK: Public
// =========================================================

BO_ObjectRef BO_ObjectAllocWithConfig(const BC_AllocatorRef alloc, const BF_ClassId cls, const size_t extraBytes, const uint16_t flags) {

	const BF_Class* class = BF_ClassIdGetRef(cls);
	const BC_AllocatorRef selectedAlloc = alloc == NULL ? BC_AllocatorGetDefault() : alloc;
	const BC_bool isSystemAllocator = selectedAlloc == kBC_AllocatorRefSystem;
	const size_t allocSize = (isSystemAllocator ? 0 : sizeof(BC_AllocatorRef)) + class->allocSize + extraBytes;

	BC_AllocatorRef* allocatorRef = BC_AllocatorAlloc(selectedAlloc, allocSize);
	const BO_ObjectRef objRef = (BO_ObjectRef)( isSystemAllocator ? allocatorRef : allocatorRef + 1 );

	objRef->cls = cls;
	objRef->flags = flags;
	objRef->ref_count = 1;

	if (isSystemAllocator) {
		BC_FLAG_CLEAR(objRef->flags, BC_OBJECT_FLAG_NON_SYSTEM_ALLOCATOR);
	} else {
		*allocatorRef = selectedAlloc;
		BC_FLAG_SET(objRef->flags, BC_OBJECT_FLAG_NON_SYSTEM_ALLOCATOR);
	}

	ObjectDebugTrack(objRef);

	return objRef;
}

BO_ObjectRef BO_ObjectAlloc(const BC_AllocatorRef alloc, const BF_ClassId cls) {
	return BO_ObjectAllocWithConfig(alloc, cls, 0, BC_OBJECT_DEFAULT_FLAGS);
}

BO_ObjectRef BO_Retain(const BO_ObjectRef obj) {
	if (obj == NULL || !BC_FLAG_HAS(obj->flags, BC_OBJECT_FLAG_REFCOUNT) ||
		BC_FLAG_HAS(obj->flags, BC_OBJECT_FLAG_CONSTANT))
		return obj;

	BC_atomic_fetch_add(&obj->ref_count, 1);

	return obj;
}

void BO_Release(const BO_ObjectRef obj) {
	if (obj == NULL || !BC_FLAG_HAS(obj->flags, BC_OBJECT_FLAG_REFCOUNT) ||
		BC_FLAG_HAS(obj->flags, BC_OBJECT_FLAG_CONSTANT))
		return;

	const BC_atomic_uint16 old_count = BC_atomic_fetch_sub(&obj->ref_count, 1);

	if (old_count == 1) {
		const BF_Class* cls = BF_ClassIdGetRef(obj->cls);
		if (cls && cls->dealloc)
			cls->dealloc(obj);

		ObjectDebugMarkFreed(obj);

		const BC_AllocatorRef allocator = BO_ObjectGetAllocator(obj);
		void* raw_ptr = BO_ObjectGetBasePointer(obj);
		BC_AllocatorFree(allocator, raw_ptr);
	}
}

BO_ObjectRef BO_Copy(const BO_ObjectRef obj) {
	if (!obj) return NULL;
	const BF_Class* cls = BF_ClassIdGetRef(obj->cls);
	if (cls && cls->copy) return cls->copy(obj);
	// Retain if no copy method,
	// assume it is immutable.
	return BO_Retain(obj);
}

uint32_t BO_Hash(const BO_ObjectRef obj) {
	if (!obj) return 0;
	const BF_Class* cls = BF_ClassIdGetRef(obj->cls);
	if (cls && cls->hash) return cls->hash(obj);
	return (uint32_t)(uintptr_t)obj;
}

BC_bool BO_Equal(const BO_ObjectRef a, const BO_ObjectRef b) {
	if (a == b) return BC_true;
	if (!a || !b) return BC_false;
	if (a->cls != b->cls) return BC_false;
	const BF_Class* cls = BF_ClassIdGetRef(a->cls);
	if (cls && cls->equal) return cls->equal(a, b);
	return BC_false;
}

BO_StringRef BO_ToString(const BO_ObjectRef obj) {
	if (obj == NULL) return BO_StringPooledLiteral("<null>");

	const BF_Class* cls = BF_ClassIdGetRef(obj->cls);

	if (cls && cls->toString) return cls->toString(obj);
	if (cls) return BO_StringCreate("<%s@%8x>", BO_StringCPtr(BF_ClassIdName(obj->cls)), BO_Hash(obj));

	return BO_StringPooledLiteral("<invalid>");
}

BC_bool BO_IsClass(const BO_ObjectRef obj, const BF_ClassId cls) {
	if (!obj) return BC_false;
	return obj->cls = cls;
}

BF_Class* BO_ObjectClass(const BO_ObjectRef obj) {
	if (!obj) return NULL;
	return BF_ClassIdGetRef(obj->cls);
}

BF_ClassId BO_ObjectClassId(const BO_ObjectRef obj) {
	if (!obj) return BF_CLASS_ID_INVALID;
	return obj->cls;
}

// =========================================================
// MARK: Debug Tracking
// =========================================================

#if BC_SETTINGS_DEBUG_OBJECT_DUMP == 1

typedef struct BO_ObjectDebugNode {
	BO_ObjectRef obj;
	BO_Object copy;
	struct BO_ObjectDebugNode* next;
} BO_ObjectDebugNode;

static struct {
	BC_SPINLOCK_MAYBE(lock)
	BO_ObjectDebugNode* head;
	BC_atomic_bool enabled;
	BC_atomic_bool keepFreedObjects;
} ObjectDebugTracker;

void ___BO_INTERNAL___ObjectInitialize(void) {
	BCSpinlockInit(&ObjectDebugTracker.lock);
	ObjectDebugTracker.head = NULL;
	ObjectDebugTracker.enabled = BC_false;
	ObjectDebugTracker.keepFreedObjects = BC_false;
}

void ___BO_INTERNAL___ObjectDebugDeinitialize(void) {
	BCSpinlockLock(&ObjectDebugTracker.lock);

	BO_ObjectDebugNode* node = ObjectDebugTracker.head;
	while (node) {
		BO_ObjectDebugNode* next = node->next;
		BCFree(node);
		node = next;
	}

	ObjectDebugTracker.head = NULL;
	BCSpinlockUnlock(&ObjectDebugTracker.lock);
	BCSpinlockDestroy(&ObjectDebugTracker.lock);
}

static void ObjectDebugTrack(const BO_ObjectRef obj) {
	if (!ObjectDebugTracker.enabled || BC_FLAG_HAS(obj->flags,BC_OBJECT_FLAG_NON_SYSTEM_ALLOCATOR))
		return;

	BCSpinlockLock(&ObjectDebugTracker.lock);

	BO_ObjectDebugNode* node = BCMalloc(sizeof(BO_ObjectDebugNode));
	node->obj = obj;
	node->copy = *obj;
	node->next = ObjectDebugTracker.head;
	ObjectDebugTracker.head = node;

	BCSpinlockUnlock(&ObjectDebugTracker.lock);
}

static void ObjectDebugMarkFreed(const BO_ObjectRef obj) {
	if (!ObjectDebugTracker.enabled || BC_FLAG_HAS(obj->flags,BC_OBJECT_FLAG_NON_SYSTEM_ALLOCATOR))
		return;

	BCSpinlockLock(&ObjectDebugTracker.lock);

	BO_ObjectDebugNode* prev = NULL;
	BO_ObjectDebugNode* curr = ObjectDebugTracker.head;

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

static const char* FlagsToString(const BF_ClassId cls, const uint16_t flags) {
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

	if (cls == BO_StringClassId()) {
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

	if (cls == BO_MapClassId()) {
		if (flags & BC_OBJECT_FLAG_CLASS_MASK) {
			BC_strcat_s(buffer, sizeof(buffer), "MAP(");
		}
		if (BC_FLAG_HAS(flags, BO_MAP_FLAG_MUTABLE))
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

void BO_ObjectDebugSetEnabled(const BC_bool enabled) {
	BC_atomic_store(&ObjectDebugTracker.enabled, enabled);
}

void BO_ObjectDebugSetKeepFreed(const BC_bool keepFreed) {
	BC_atomic_store(&ObjectDebugTracker.keepFreedObjects, keepFreed);
}

#define DGRAY "\033[48;5;234m"
#define BLACK "\033[48;5;235m"
#define RESET "\033[0m"
#define BOLD "\033[1m"

void BO_ObjectDebugDump(void) {
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
	BO_ObjectDebugNode* node = ObjectDebugTracker.head;
	while (node) {
		const BO_ObjectRef obj = node->obj == NULL ? &node->copy : node->obj;
		if (node->obj == NULL)
			freedCount++;
		const BF_Class* cls = BF_ClassIdGetRef(obj->cls);
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
			const BC_AllocatorRef allocator = BO_ObjectGetAllocator(obj);
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
			BO_ObjectDebugSetEnabled(BC_false);
			const BO_StringRef description = BO_ToString(node->obj);
			printf("│%s %-16p │ %-16s │ %-20s │ %-8d │ %-9s │ %-28s " RESET "│\n",
				   color, (void*)node->obj, classDisplay, flagsDisplay, refCount,
				   allocatorPtr, BO_StringCPtr(description));
			BO_Release($OBJ description);
			BO_ObjectDebugSetEnabled(enabledOld);
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
void ___BF_INTERNAL___ObjectDebugInitialize() {}
void ___BF_INTERNAL___ObjectDebugDeinitialize() {}
#endif
