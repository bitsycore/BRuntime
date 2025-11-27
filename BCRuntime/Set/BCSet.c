#include "BCSet.h"

#include "../Class/BCClass.h"
#include "../Class/BCClassRegistry.h"
#include "../List/BCList.h"
#include "../String/BCStringBuilder.h"
#include "../Utilities/BC_Memory.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

// =========================================================
// MARK: Struct
// =========================================================

typedef struct BCSet {
	BCObject base;
	size_t capacity;
	size_t count;
	BCObjectRef* buckets;
} BCSet;

// =========================================================
// MARK: Forward
// =========================================================

static void SetPut(BCObjectRef* buckets, size_t cap, BCObjectRef val);

// =========================================================
// MARK: Methods
// =========================================================

static void SetDeallocImpl(const BCObjectRef obj) {
	const BCSetRef s = (BCSetRef)obj;
	for (size_t i = 0; i < s->capacity; i++) {
		if (s->buckets[i]) {
			BCRelease(s->buckets[i]);
		}
	}
	BCFree(s->buckets);
}

static BCStringRef SetToStringImpl(const BCObjectRef obj) {
	const BCSetRef s = (BCSetRef)obj;
	const BCStringBuilderRef sb = BCStringBuilderCreate(NULL);
	BCStringBuilderAppend(sb, "{ ");

	size_t n = 0;
	for (size_t i = 0; i < s->capacity; i++) {
		if (s->buckets[i]) {
			if (n > 0) {
				BCStringBuilderAppend(sb, ", ");
			}
			const BCStringRef valStr = BCToString(s->buckets[i]);
			BCStringBuilderAppendString(sb, valStr);
			BCRelease($OBJ valStr);
			n++;
		}
	}

	BCStringBuilderAppend(sb, " }");
	const BCStringRef result = BCStringBuilderFinish(sb);
	BCRelease($OBJ sb);
	return result;
}

// =========================================================
// MARK: Class
// =========================================================

static BCClass kBCSetClass = {
	.name = "BCSet", .id = BC_CLASS_ID_INVALID, .dealloc = SetDeallocImpl, .hash = NULL, .equal = NULL, .toString = SetToStringImpl, .copy = NULL, .allocSize = sizeof(BCSet)};

BCClassId BCSetClassId(void) { return kBCSetClass.id; }

void ___BCINTERNAL___SetInitialize(void) { BCClassRegister(&kBCSetClass); }

// =========================================================
// MARK: Constructors
// =========================================================

BCSetRef BCSetCreate(void) {
	const BCMutableSetRef set = BCMutableSetCreate();
	BC_FLAG_CLEAR(set->base.flags, BC_SET_FLAG_MUTABLE);
	return set;
}

BCMutableSetRef BCMutableSetCreate(void) {
	const BCSetRef s = (BCSetRef)BCObjectAllocWithConfig(NULL, kBCSetClass.id, 0, BC_OBJECT_DEFAULT_FLAGS | BC_SET_FLAG_MUTABLE);
	s->capacity = 8;
	s->count = 0;
	s->buckets = BCCalloc(s->capacity, sizeof(BCObjectRef));
	return s;
}

BCSetRef BCSetCreateWithObjects(const BC_bool retain, const size_t count, ...) {
	const BCMutableSetRef s = BCMutableSetCreate();
	va_list args;
	va_start(args, count);
	for (size_t i = 0; i < count; i++) {
		const BCObjectRef val = va_arg(args, BCObjectRef);
		BCSetAdd(s, val);
		if (!retain) {
			BCRelease(val);
		}
	}
	va_end(args);
	BC_FLAG_CLEAR(s->base.flags, BC_SET_FLAG_MUTABLE);
	return s;
}

// =========================================================
// MARK: Public
// =========================================================

void BCSetAdd(const BCMutableSetRef s, const BCObjectRef obj) {
	BCSet* set = s;
	if (!BC_FLAG_HAS(set->base.flags, BC_SET_FLAG_MUTABLE))
		return;
	if (obj == NULL)
		return;

	// Resize Check
	if (set->count >= (size_t)((double)set->capacity * 0.75)) {
		const size_t newCap = set->capacity * 2;
		BCObjectRef* newBuckets = BCCalloc(newCap, sizeof(BCObjectRef));
		if (!newBuckets)
			return;
		for (size_t i = 0; i < set->capacity; i++) {
			if (set->buckets[i]) {
				SetPut(newBuckets, newCap, set->buckets[i]);
			}
		}
		BCFree(set->buckets);
		set->buckets = newBuckets;
		set->capacity = newCap;
	}

	// Insert
	const uint32_t hash = BCHash(obj);
	size_t idx = hash % set->capacity;
	while (set->buckets[idx]) {
		if (BCEqual(set->buckets[idx], obj)) {
			// Already exists
			return;
		}
		idx = (idx + 1) % set->capacity;
	}

	// New Entry: RETAIN Value
	set->buckets[idx] = BCRetain(obj);
	set->count++;
}

void BCSetRemove(const BCMutableSetRef s, const BCObjectRef obj) {
	BCSet* set = s;
	if (!BC_FLAG_HAS(set->base.flags, BC_SET_FLAG_MUTABLE))
		return;
	if (obj == NULL)
		return;

	const uint32_t hash = BCHash(obj);
	size_t idx = hash % set->capacity;
	const size_t start = idx;

	while (set->buckets[idx]) {
		if (BCEqual(set->buckets[idx], obj)) {
			BCRelease(set->buckets[idx]);
			set->buckets[idx] = NULL;
			set->count--;

			// Rehash subsequent items in the cluster to fill the gap
			size_t nextIdx = (idx + 1) % set->capacity;
			while (set->buckets[nextIdx]) {
				BCObjectRef rehashObj = set->buckets[nextIdx];
				set->buckets[nextIdx] = NULL;
				set->count--;
				SetPut(set->buckets, set->capacity, rehashObj);
				set->count++;
				nextIdx = (nextIdx + 1) % set->capacity;
			}
			return;
		}
		idx = (idx + 1) % set->capacity;
		if (idx == start)
			break;
	}
}

BC_bool BCSetContains(const BCSetRef s, const BCObjectRef obj) {
	if (obj == NULL)
		return BC_false;
	const uint32_t hash = BCHash(obj);
	size_t idx = hash % s->capacity;
	const size_t start = idx;
	while (s->buckets[idx]) {
		if (BCEqual(s->buckets[idx], obj)) {
			return BC_true;
		}
		idx = (idx + 1) % s->capacity;
		if (idx == start)
			break;
	}
	return BC_false;
}

size_t BCSetCount(const BCSetRef s) { return s->count; }

BCListRef BCSetToList(const BCSetRef s) {
	const BCListRef list = BCListCreate();
	for (size_t i = 0; i < s->capacity; i++) {
		if (s->buckets[i]) {
			BCListAdd(list, s->buckets[i]);
		}
	}
	return list;
}

void BCSetForEach(const BCSetRef s, void (*block)(BCObjectRef item)) {
	if (!block)
		return;
	for (size_t i = 0; i < s->capacity; i++) {
		if (s->buckets[i]) {
			block(s->buckets[i]);
		}
	}
}

// =========================================================
// MARK: Internal
// =========================================================

static void SetPut(BCObjectRef* buckets, const size_t cap, const BCObjectRef val) {
	const uint32_t hash = BCHash(val);
	size_t idx = hash % cap;
	while (buckets[idx]) {
		// In resize/rehash, assume no duplicates, so just find next empty
		idx = (idx + 1) % cap;
	}
	buckets[idx] = val;
}
