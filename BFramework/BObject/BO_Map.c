#include "BO_Map.h"

#include "BCore/BC_Macro.h"
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

typedef struct {
	BO_ObjectRef key;
	BO_ObjectRef value;
} MapEntry;

typedef struct BO_Map {
	BO_Object base;
	size_t capacity;
	size_t count;
	MapEntry* buckets;
} BO_Map;

// =========================================================
// MARK: Forward
// =========================================================

static void PRIV_MapPut(MapEntry* buckets, size_t cap, BO_ObjectRef key, BO_ObjectRef val);

// =========================================================
// MARK: Methods
// =========================================================

static void IMPL_MapDealloc(const BO_ObjectRef obj) {
	const BO_MapRef d = (BO_MapRef)obj;
	for (size_t i = 0; i < d->capacity; i++) {
		if (d->buckets[i].key) {
			BO_Release(d->buckets[i].key);
			BO_Release(d->buckets[i].value);
		}
	}
	BC_Free(d->buckets);
}

BO_StringRef IMPL_MapToString(const BO_ObjectRef obj) {
	const BO_MapRef d = (BO_MapRef)obj;
	const BO_StringBuilderRef sb = BO_StringBuilderCreate(NULL);
	BO_StringBuilderAppend(sb, "{ ");

	size_t n = 0;
	for (size_t i = 0; i < d->capacity; i++) {
		if (d->buckets[i].key) {
			if (n > 0) {
				BO_StringBuilderAppend(sb, ", ");
			}
			const BO_StringRef keyStr = BO_ToString(d->buckets[i].key);
			const BO_StringRef valStr = BO_ToString(d->buckets[i].value);
			BO_StringBuilderAppendString(sb, keyStr);
			BO_StringBuilderAppendChar(sb, ':');
			BO_StringBuilderAppendChar(sb, ' ');
			BO_StringBuilderAppendString(sb, valStr);
			BO_Release($OBJ keyStr);
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

static BF_Class kBO_MapClass = {
	.name = "BO_Map",
	.id = BF_CLASS_ID_INVALID,
	.dealloc = IMPL_MapDealloc,
	.hash = NULL,
	.equal = NULL,
	.toString = IMPL_MapToString,
	.copy = NULL,
	.allocSize = sizeof(BO_Map)
};

BF_ClassId BO_MapClassId() {
	return kBO_MapClass.id;
}

void INTERNAL_BO_MapInitialize(void) {
	BF_ClassRegistryInsert(&kBO_MapClass);
}

// =========================================================
// MARK: Constructors
// =========================================================

BO_MapRef BO_MapCreate() {
	const BO_MutableMapRef dic = BO_MutableMapCreate();
	BC_FLAG_CLEAR(dic->base.flags, BO_MAP_FLAG_MUTABLE);
	return dic;
}

BO_MutableMapRef BO_MutableMapCreate() {
	const BO_MapRef d = (BO_MapRef)BO_ObjectAllocWithConfig(NULL, kBO_MapClass.id, 0, BC_OBJECT_DEFAULT_FLAGS | BO_MAP_FLAG_MUTABLE);
	d->capacity = 8;
	d->count = 0;
	d->buckets = BC_Calloc(d->capacity, sizeof(MapEntry));
	return d;
}

BO_MapRef BO_MapCreateWithObjects(const BC_bool retain, const size_t count, ...) {
	const BO_MutableMapRef d = BO_MutableMapCreate();
	va_list args;
	va_start(args, count);
	for (size_t i = 0; i < count / 2; i++) {
		const BO_ObjectRef key = va_arg(args, BO_ObjectRef);
		const BO_ObjectRef val = va_arg(args, BO_ObjectRef);
		BO_MapSet(d, key, val);
		if (!retain) {
			BO_Release(key);
			BO_Release(val);
		}
	}
	va_end(args);
	BC_FLAG_CLEAR(d->base.flags, BO_MAP_FLAG_MUTABLE);
	return d;
}

// =========================================================
// MARK: Public
// =========================================================

void BO_MapSet(const BO_MutableMapRef d, const BO_ObjectRef key, const BO_ObjectRef val) {
	BO_Map* dict = d;
	if (!BC_FLAG_HAS(dict->base.flags, BO_MAP_FLAG_MUTABLE)) return;

	// Resize Check
	if (dict->count >= (size_t)((double)dict->capacity * 0.75)) {
		const size_t newCap = dict->capacity * 2;
		MapEntry* newBuckets = BC_Calloc(newCap, sizeof(MapEntry));
		if (!newBuckets) return;
		for (size_t i = 0; i < dict->capacity; i++) {
			if (dict->buckets[i].key) {
				PRIV_MapPut(newBuckets, newCap, dict->buckets[i].key, dict->buckets[i].value);
			}
		}
		BC_Free(dict->buckets);
		dict->buckets = newBuckets;
		dict->capacity = newCap;
	}

	// Insert
	const uint32_t hash = BO_Hash(key);
	size_t idx = hash % dict->capacity;
	while (dict->buckets[idx].key) {
		if (BO_Equal(dict->buckets[idx].key, key)) {
			BO_Release(dict->buckets[idx].value);
			dict->buckets[idx].value = BO_Retain(val);
			return;
		}
		idx = (idx + 1) % dict->capacity;
	}

	// New Entry: COPY Key, RETAIN Value
	dict->buckets[idx].key = BO_Copy(key);
	dict->buckets[idx].value = BO_Retain(val);
	dict->count++;
}

BO_ObjectRef BO_MapGet(const BO_MapRef d, const BO_ObjectRef key) {
	const uint32_t hash = BO_Hash(key);
	size_t idx = hash % d->capacity;
	const size_t start = idx;
	while (d->buckets[idx].key) {
		if (BO_Equal(d->buckets[idx].key, key)) {
			return BO_Retain(d->buckets[idx].value);
		}
		idx = (idx + 1) % d->capacity;
		if (idx == start) break;
	}
	return NULL;
}

BO_ListRef BO_MapKeys(const BO_MapRef d) {
	const BO_ListRef arr = BO_ListCreate();
	for (size_t i = 0; i < d->capacity; i++) {
		if (d->buckets[i].key) {
			BO_ListAdd(arr, d->buckets[i].key);
		}
	}
	return arr;
}

BO_ListRef BO_MapValues(const BO_MapRef d) {
	const BO_ListRef arr = BO_ListCreate();
	for (size_t i = 0; i < d->capacity; i++) {
		if (d->buckets[i].key) {
			BO_ListAdd(arr, d->buckets[i].value);
		}
	}
	return arr;
}

size_t BO_MapCount(const BO_MapRef d) { return d->count; }

BC_bool BO_MapIsEmpty(const BO_MapRef d) { return d->count == 0; }

void BO_MapClear(const BO_MutableMapRef d) {
	BO_Map* dict = d;
	if (!BC_FLAG_HAS(dict->base.flags, BO_MAP_FLAG_MUTABLE))
		return;

	for (size_t i = 0; i < dict->capacity; i++) {
		if (dict->buckets[i].key) {
			BO_Release(dict->buckets[i].key);
			BO_Release(dict->buckets[i].value);
			dict->buckets[i].key = NULL;
			dict->buckets[i].value = NULL;
		}
	}
	dict->count = 0;
}

void BO_MapRemove(const BO_MutableMapRef d, const BO_ObjectRef key) {
	BO_Map* dict = d;
	if (!BC_FLAG_HAS(dict->base.flags, BO_MAP_FLAG_MUTABLE))
		return;
	if (key == NULL)
		return;

	const uint32_t hash = BO_Hash(key);
	size_t idx = hash % dict->capacity;
	const size_t start = idx;

	while (dict->buckets[idx].key) {
		if (BO_Equal(dict->buckets[idx].key, key)) {
			BO_Release(dict->buckets[idx].key);
			BO_Release(dict->buckets[idx].value);
			dict->buckets[idx].key = NULL;
			dict->buckets[idx].value = NULL;
			dict->count--;

			// Rehash chain
			size_t nextIdx = (idx + 1) % dict->capacity;
			while (dict->buckets[nextIdx].key) {
				BO_ObjectRef rehashKey = dict->buckets[nextIdx].key;
				BO_ObjectRef rehashVal = dict->buckets[nextIdx].value;
				dict->buckets[nextIdx].key = NULL;
				dict->buckets[nextIdx].value = NULL;
				PRIV_MapPut(dict->buckets, dict->capacity, rehashKey, rehashVal);
				nextIdx = (nextIdx + 1) % dict->capacity;
			}
			return;
		}
		idx = (idx + 1) % dict->capacity;
		if (idx == start)
			break;
	}
}

BC_bool BO_MapContainsKey(const BO_MapRef d, const BO_ObjectRef key) {
	return BO_MapGet(d, key) != NULL;
}

BC_bool BO_MapContainsValue(const BO_MapRef d, const BO_ObjectRef val) {
	for (size_t i = 0; i < d->capacity; i++) {
		if (d->buckets[i].key) {
			if (BO_Equal(d->buckets[i].value, val)) {
				return BC_true;
			}
		}
	}
	return BC_false;
}

BO_ObjectRef BO_MapGetOrDefault(const BO_MapRef d, const BO_ObjectRef key, const BO_ObjectRef defaultValue) {
	const BO_ObjectRef val = BO_MapGet(d, key);
	if (val)
		return val;
	return defaultValue ? BO_Retain(defaultValue) : NULL;
}

void BO_MapForEach(const BO_MapRef d, void (*block)(BO_ObjectRef key, BO_ObjectRef value)) {
	if (!block)
		return;
	for (size_t i = 0; i < d->capacity; i++) {
		if (d->buckets[i].key) {
			block(d->buckets[i].key, d->buckets[i].value);
		}
	}
}

// =========================================================
// MARK: Internal
// =========================================================

static void PRIV_MapPut(MapEntry* buckets, const size_t cap, const BO_ObjectRef key, const BO_ObjectRef val) {
	const uint32_t hash = BO_Hash(key);
	size_t idx = hash % cap;
	while (buckets[idx].key) {
		if (BO_Equal(buckets[idx].key, key)) {
			BO_Release(buckets[idx].value);
			buckets[idx].value = val; // Transfer ownership
			return;
		}
		idx = (idx + 1) % cap;
	}
	buckets[idx].key = key;
	buckets[idx].value = val;
}
