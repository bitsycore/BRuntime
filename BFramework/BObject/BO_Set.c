#include "BO_Set.h"

#include "BCore/Memory/BC_Memory.h"

#include "BO_List.h"
#include "BO_StringBuilder.h"
#include "../BF_Class.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

// =========================================================
// MARK: Struct
// =========================================================

typedef struct BO_Set {
	BO_Object base;
	size_t capacity;
	size_t count;
	BO_ObjectRef* buckets;
} BO_Set;

// =========================================================
// MARK: Forward
// =========================================================

static void PRIV_SetPut(BO_ObjectRef* buckets, size_t cap, BO_ObjectRef val);

// =========================================================
// MARK: Methods
// =========================================================

static void IMPL_SetDealloc(const BO_ObjectRef obj) {
	const BO_SetRef s = (BO_SetRef)obj;
	for (size_t i = 0; i < s->capacity; i++) {
		if (s->buckets[i]) {
			BO_Release(s->buckets[i]);
		}
	}
	BC_Free(s->buckets);
}

static BO_StringRef IMPL_SetToString(const BO_ObjectRef obj) {
	const BO_SetRef s = (BO_SetRef)obj;
	const BO_StringBuilderRef sb = BO_StringBuilderCreate(NULL);
	BO_StringBuilderAppend(sb, "{ ");

	size_t n = 0;
	for (size_t i = 0; i < s->capacity; i++) {
		if (s->buckets[i]) {
			if (n > 0) {
				BO_StringBuilderAppend(sb, ", ");
			}
			const BO_StringRef valStr = BO_ToString(s->buckets[i]);
			BO_StringBuilderAppendString(sb, valStr);
			BO_Release($OBJ valStr);
			n++;
		}
	}

	BO_StringBuilderAppend(sb, " }");
	const BO_StringRef result = BO_StringBuilderFinish(sb, BC_false);
	BO_Release($OBJ sb);
	return result;
}

// =========================================================
// MARK: Class
// =========================================================

static BF_Class kBO_SetClass = {
	.name = "BO_Set", .id = BF_CLASS_ID_INVALID, .dealloc = IMPL_SetDealloc, .hash = NULL, .equal = NULL, .toString = IMPL_SetToString, .copy = NULL, .allocSize = sizeof(BO_Set)
};

BF_ClassId BO_SetClassId(void) { return kBO_SetClass.id; }

void INTERNAL_BO_SetInitialize(void) { BF_ClassRegistryInsert(&kBO_SetClass); }

// =========================================================
// MARK: Constructors
// =========================================================

BO_SetRef BO_SetCreate(void) {
	const BO_MutableSetRef set = BO_MutableSetCreate();
	BC_FLAG_CLEAR(set->base.flags, BO_SET_FLAG_MUTABLE);
	return set;
}

BO_MutableSetRef BO_MutableSetCreate(void) {
	const BO_SetRef s = (BO_SetRef)BO_ObjectAllocWithConfig(NULL, kBO_SetClass.id, 0, BC_OBJECT_DEFAULT_FLAGS | BO_SET_FLAG_MUTABLE);
	s->capacity = 8;
	s->count = 0;
	s->buckets = BC_Calloc(s->capacity, sizeof(BO_ObjectRef));
	return s;
}

BO_SetRef BO_SetCreateWithObjects(const BC_bool retain, const size_t count, ...) {
	const BO_MutableSetRef s = BO_MutableSetCreate();
	va_list args;
	va_start(args, count);
	for (size_t i = 0; i < count; i++) {
		const BO_ObjectRef val = va_arg(args, BO_ObjectRef);
		BO_SetAdd(s, val);
		if (!retain) {
			BO_Release(val);
		}
	}
	va_end(args);
	BC_FLAG_CLEAR(s->base.flags, BO_SET_FLAG_MUTABLE);
	return s;
}

// =========================================================
// MARK: Public
// =========================================================

void BO_SetAdd(const BO_MutableSetRef s, const BO_ObjectRef obj) {
	BO_Set* set = s;
	if (!BC_FLAG_HAS(set->base.flags, BO_SET_FLAG_MUTABLE))
		return;
	if (obj == NULL)
		return;

	// Resize Check
	if (set->count >= (size_t)((double)set->capacity * 0.75)) {
		const size_t newCap = set->capacity * 2;
		BO_ObjectRef* newBuckets = BC_Calloc(newCap, sizeof(BO_ObjectRef));
		if (!newBuckets)
			return;
		for (size_t i = 0; i < set->capacity; i++) {
			if (set->buckets[i]) {
				PRIV_SetPut(newBuckets, newCap, set->buckets[i]);
			}
		}
		BC_Free(set->buckets);
		set->buckets = newBuckets;
		set->capacity = newCap;
	}

	// Insert
	const uint32_t hash = BO_Hash(obj);
	size_t idx = hash % set->capacity;
	while (set->buckets[idx]) {
		if (BO_Equal(set->buckets[idx], obj)) {
			// Already exists
			return;
		}
		idx = (idx + 1) % set->capacity;
	}

	// New Entry: RETAIN Value
	set->buckets[idx] = BO_Retain(obj);
	set->count++;
}

void BO_SetRemove(const BO_MutableSetRef s, const BO_ObjectRef obj) {
	BO_Set* set = s;
	if (!BC_FLAG_HAS(set->base.flags, BO_SET_FLAG_MUTABLE))
		return;
	if (obj == NULL)
		return;

	const uint32_t hash = BO_Hash(obj);
	size_t idx = hash % set->capacity;
	const size_t start = idx;

	while (set->buckets[idx]) {
		if (BO_Equal(set->buckets[idx], obj)) {
			BO_Release(set->buckets[idx]);
			set->buckets[idx] = NULL;
			set->count--;

			// Rehash subsequent items in the cluster to fill the gap
			size_t nextIdx = (idx + 1) % set->capacity;
			while (set->buckets[nextIdx]) {
				BO_ObjectRef rehashObj = set->buckets[nextIdx];
				set->buckets[nextIdx] = NULL;
				set->count--;
				PRIV_SetPut(set->buckets, set->capacity, rehashObj);
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

BC_bool BO_SetContains(const BO_SetRef s, const BO_ObjectRef obj) {
	if (obj == NULL)
		return BC_false;
	const uint32_t hash = BO_Hash(obj);
	size_t idx = hash % s->capacity;
	const size_t start = idx;
	while (s->buckets[idx]) {
		if (BO_Equal(s->buckets[idx], obj)) {
			return BC_true;
		}
		idx = (idx + 1) % s->capacity;
		if (idx == start)
			break;
	}
	return BC_false;
}

size_t BO_SetCount(const BO_SetRef s) { return s->count; }

BO_ListRef BO_SetToList(const BO_SetRef s) {
	const BO_ListRef list = BO_ListCreate();
	for (size_t i = 0; i < s->capacity; i++) {
		if (s->buckets[i]) {
			BO_ListAdd(list, s->buckets[i]);
		}
	}
	return list;
}

void BO_SetForEach(const BO_SetRef s, void (*block)(BO_ObjectRef item)) {
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

static void PRIV_SetPut(BO_ObjectRef* buckets, const size_t cap, const BO_ObjectRef val) {
	const uint32_t hash = BO_Hash(val);
	size_t idx = hash % cap;
	while (buckets[idx]) {
		// In resize/rehash, assume no duplicates, so just find next empty
		idx = (idx + 1) % cap;
	}
	buckets[idx] = val;
}
