#include "BCMap.h"

#include "../BCString.h"
#include "../Array/BCArray.h"

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
	bool isMutable;
	size_t capacity;
	size_t count;
	BCMapEntry* buckets;
} BCMap;

// =========================================================
// MARK: Forward
// =========================================================

static void _DicPutInternal(BCMapEntry* buckets, size_t cap, BCObjectRef key, BCObjectRef val);

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
	free(d->buckets);
}

BCStringRef MapToStringImpl(const BCObjectRef obj) {
	const BCMapRef d = (BCMapRef) obj;
	return BCStringCreate("BCMap(count: %zu)", d->count);
	printf("{\n");
	for (size_t i = 0; i < d->capacity; i++) {
		if (d->buckets[i].key) {
			BCToString(d->buckets[i].key);
			printf(":");
			d->buckets[i].value->cls->toString(d->buckets[i].value);
			printf(",\n");
		}
	}
	printf("}");
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

// =========================================================
// MARK: Public
// =========================================================

BCMapRef BCMapCreate() {
	const BCMutableMapRef dic = BCMutableMapCreate();
	dic->isMutable = false;
	return  dic;
}

BCMutableMapRef BCMutableMapCreate() {
	const BCMapRef d = (BCMapRef) BCAllocObject((BCClassRef)&kBCDictClass, NULL);
	d->isMutable = true;
	d->capacity = 8;
	d->count = 0;
	d->buckets = calloc(d->capacity, sizeof(BCMapEntry));
	return d;
}

void BCMapSet(const BCMutableMapRef d, const BCObjectRef key, const BCObjectRef val) {
	BCMap *dict = d;
	if (!dict->isMutable) return;

	// Resize Check
	if (dict->count >= (size_t) ((double) dict->capacity * 0.75)) {
		const size_t newCap = dict->capacity * 2;
		BCMapEntry* newBuckets = calloc(newCap, sizeof(BCMapEntry));
		if (!newBuckets) return;
		for (size_t i = 0; i < dict->capacity; i++) {
			if (dict->buckets[i].key) {
				_DicPutInternal(newBuckets, newCap, dict->buckets[i].key, dict->buckets[i].value);
			}
		}
		free(dict->buckets);
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

BCArrayRef BCMapKeys(const BCMapRef d) {
	const BCArrayRef arr = BCArrayCreate();
	for (size_t i = 0; i < d->capacity; i++) {
		if (d->buckets[i].key) {
			BCArrayAdd(arr, d->buckets[i].key);
		}
	}
	return arr;
}

BCArrayRef BCMapValues(const BCMapRef d) {
	const BCArrayRef arr = BCArrayCreate();
	for (size_t i = 0; i < d->capacity; i++) {
		if (d->buckets[i].key) {
			BCArrayAdd(arr, d->buckets[i].value);
		}
	}
	return arr;
}

BCMapRef BCMapCreateWithObjects(const bool retain, const size_t count, ...) {
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
	d->isMutable = false;
	return d;
}

// =========================================================
// MARK: Internal
// =========================================================

static void _DicPutInternal(BCMapEntry* buckets, const size_t cap, const BCObjectRef key, const BCObjectRef val) {
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