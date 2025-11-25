#include "BCMap.h"

#include "../BCClass.h"
#include "../BCStringBuilder.h"
#include "../List/BCList.h"
#include "../Utilities/BCMemory.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

// =========================================================
// MARK: Struct
// =========================================================

typedef struct {
	BCObjectRef key;
	BCObjectRef value;
} BCMapEntry;

typedef struct BCMap {
	BCObject base;
	BC_bool isMutable;
	size_t capacity;
	size_t count;
	BCMapEntry* buckets;
} BCMap;

// =========================================================
// MARK: Forward
// =========================================================

static void MapPut(BCMapEntry* buckets, size_t cap, BCObjectRef key, BCObjectRef val);

// =========================================================
// MARK: Class
// =========================================================

static void MapDeallocImpl(const BCObjectRef obj) {
	const BCMapRef d = (BCMapRef) obj;
	for (size_t i = 0; i < d->capacity; i++) {
		if (d->buckets[i].key) {
			BCRelease(d->buckets[i].key);
			BCRelease(d->buckets[i].value);
		}
	}
	BCFree(d->buckets);
}

BCStringRef MapToStringImpl(const BCObjectRef obj) {
	const BCMapRef d = (BCMapRef) obj;
	const BCStringBuilderRef sb = BCStringBuilderCreate(NULL);
	BCStringBuilderAppend(sb, "{ ");

	size_t n = 0;
	for (size_t i = 0; i < d->capacity; i++) {
		if (d->buckets[i].key) {
			if (n > 0) {
				BCStringBuilderAppend(sb, ", ");
			}
			const BCStringRef keyStr = BCToString(d->buckets[i].key);
			const BCStringRef valStr = d->buckets[i].value->cls->toString(d->buckets[i].value);
			BCStringBuilderAppendString(sb, keyStr);
			BCStringBuilderAppendChar(sb, ':');
			BCStringBuilderAppendChar(sb, ' ');
			BCStringBuilderAppendString(sb, valStr);
			BCRelease($OBJ keyStr);
			BCRelease($OBJ valStr);
			n++;
		}
	}

	BCStringBuilderAppend(sb, " }");
	const BCStringRef result = BCStringBuilderFinish(sb);
	BCRelease($OBJ sb);
	return result;
}


static const BCClass kBCDictClass = {
	"BCMap",
	MapDeallocImpl,
	NULL,
	NULL,
	MapToStringImpl,
	NULL,
	sizeof(BCMap)
};

const BCClassRef kBCDictClassRef = (BCClassRef) &kBCDictClass;

// =========================================================
// MARK: Public
// =========================================================

BCMapRef BCMapCreate() {
	const BCMutableMapRef dic = BCMutableMapCreate();
	dic->isMutable = BC_false;
	return  dic;
}

BCMutableMapRef BCMutableMapCreate() {
	const BCMapRef d = (BCMapRef) BCObjectAlloc(NULL, (BCClassRef)&kBCDictClass);
	d->isMutable = BC_true;
	d->capacity = 8;
	d->count = 0;
	d->buckets = BCCalloc(d->capacity, sizeof(BCMapEntry));
	return d;
}

void BCMapSet(const BCMutableMapRef d, const BCObjectRef key, const BCObjectRef val) {
	BCMap *dict = d;
	if (!dict->isMutable) return;

	// Resize Check
	if (dict->count >= (size_t) ((double) dict->capacity * 0.75)) {
		const size_t newCap = dict->capacity * 2;
		BCMapEntry* newBuckets = BCCalloc(newCap, sizeof(BCMapEntry));
		if (!newBuckets) return;
		for (size_t i = 0; i < dict->capacity; i++) {
			if (dict->buckets[i].key) {
				MapPut(newBuckets, newCap, dict->buckets[i].key, dict->buckets[i].value);
			}
		}
		BCFree(dict->buckets);
		dict->buckets = newBuckets;
		dict->capacity = newCap;
	}

	// Insert
	const uint32_t hash = BCHash(key);
	size_t idx = hash % dict->capacity;
	while (dict->buckets[idx].key) {
		if (BCEqual(dict->buckets[idx].key, key)) {
			BCRelease(dict->buckets[idx].value);
			dict->buckets[idx].value = BCRetain(val);
			return;
		}
		idx = (idx + 1) % dict->capacity;
	}

	// New Entry: COPY Key, RETAIN Value
	dict->buckets[idx].key = BCObjectCopy(key);
	dict->buckets[idx].value = BCRetain(val);
	dict->count++;
}

BCObjectRef BCMapGet(const BCMapRef d, const BCObjectRef key) {
	const uint32_t hash = BCHash(key);
	size_t idx = hash % d->capacity;
	const size_t start = idx;
	while (d->buckets[idx].key) {
		if (BCEqual(d->buckets[idx].key, key)) {
			return BCRetain(d->buckets[idx].value);
		}
		idx = (idx + 1) % d->capacity;
		if (idx == start) break;
	}
	return NULL;
}

BCListRef BCMapKeys(const BCMapRef d) {
	const BCListRef arr = BCListCreate();
	for (size_t i = 0; i < d->capacity; i++) {
		if (d->buckets[i].key) {
			BCListAdd(arr, d->buckets[i].key);
		}
	}
	return arr;
}

BCListRef BCMapValues(const BCMapRef d) {
	const BCListRef arr = BCListCreate();
	for (size_t i = 0; i < d->capacity; i++) {
		if (d->buckets[i].key) {
			BCListAdd(arr, d->buckets[i].value);
		}
	}
	return arr;
}

BCMapRef BCMapCreateWithObjects(const BC_bool retain, const size_t count, ...) {
	const BCMutableMapRef d = BCMutableMapCreate();
	va_list args;
	va_start(args, count);
	for (size_t i = 0; i < count/2; i++) {
		const BCObjectRef key = va_arg(args, BCObjectRef);
		const BCObjectRef val = va_arg(args, BCObjectRef);
		BCMapSet(d, key, val);
		if (!retain) {
			BCRelease(key);
			BCRelease(val);
		}
	}
	va_end(args);
	d->isMutable = BC_false;
	return d;
}

// =========================================================
// MARK: Internal
// =========================================================

static void MapPut(BCMapEntry* buckets, const size_t cap, const BCObjectRef key, const BCObjectRef val) {
	const uint32_t hash = BCHash(key);
	size_t idx = hash % cap;
	while (buckets[idx].key) {
		if (BCEqual(buckets[idx].key, key)) {
			BCRelease(buckets[idx].value);
			buckets[idx].value = val; // Transfer ownership
			return;
		}
		idx = (idx + 1) % cap;
	}
	buckets[idx].key = key;
	buckets[idx].value = val;
}