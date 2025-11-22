#include "BCDictionary.h"

#include "BCArray.h"
#include "BCString.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

// =========================================================
// MARK: Struct
// =========================================================

typedef struct {
	BCObjectRef key;
	BCObjectRef value;
} BCDictionaryEntry;

typedef struct BCDictionary {
	BCObject base;
	BCDictionaryEntry* buckets;
	size_t capacity;
	size_t count;
	bool isMutable;
} BCDictionary;

// =========================================================
// MARK: Forward
// =========================================================

static void _DicPutInternal(BCDictionaryEntry* buckets, size_t cap, BCObjectRef key, BCObjectRef val);

// =========================================================
// MARK: Class
// =========================================================

static void DictionaryDeallocImpl(const BCObjectRef obj) {
	const BCDictionaryRef d = (BCDictionaryRef) obj;
	for (size_t i = 0; i < d->capacity; i++) {
		if (d->buckets[i].key) {
			BCRelease(d->buckets[i].key);
			BCRelease(d->buckets[i].value);
		}
	}
	free(d->buckets);
}

BCStringRef DictionaryToStringImpl(const BCObjectRef obj) {
	const BCDictionaryRef d = (BCDictionaryRef) obj;
	return BCStringCreate("BCDictionary(count: %zu)", d->count);
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
	"BCDictionary",
	DictionaryDeallocImpl,
	NULL,
	NULL,
	DictionaryToStringImpl,
	NULL,
	sizeof(BCDictionary)
};

// =========================================================
// MARK: Public
// =========================================================

BCDictionaryRef BCDictionaryCreate() {
	const BCMutableDictionaryRef dic = BCMutableDictionaryCreate();
	dic->isMutable = false;
	return  dic;
}

BCMutableDictionaryRef BCMutableDictionaryCreate() {
	const BCDictionaryRef d = (BCDictionaryRef) BCObjectAlloc((BCClassRef)&kBCDictClass, NULL);
	d->isMutable = true;
	d->capacity = 8;
	d->count = 0;
	d->buckets = calloc(d->capacity, sizeof(BCDictionaryEntry));
	return d;
}

void BCDictionarySet(const BCMutableDictionaryRef d, const BCObjectRef key, const BCObjectRef val) {
	BCDictionary *dict = d;
	if (!dict->isMutable) return;

	// Resize Check
	if (dict->count >= (size_t) ((double) dict->capacity * 0.75)) {
		const size_t newCap = dict->capacity * 2;
		BCDictionaryEntry* newBuckets = calloc(newCap, sizeof(BCDictionaryEntry));
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

BCObjectRef BCDictionaryGet(const BCDictionaryRef d, const BCObjectRef key) {
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

BCArrayRef BCDictionaryKeys(const BCDictionaryRef d) {
	const BCArrayRef arr = BCArrayCreate();
	for (size_t i = 0; i < d->capacity; i++) {
		if (d->buckets[i].key) {
			BCArrayAdd(arr, d->buckets[i].key);
		}
	}
	return arr;
}

BCArrayRef BCDictionaryValues(const BCDictionaryRef d) {
	const BCArrayRef arr = BCArrayCreate();
	for (size_t i = 0; i < d->capacity; i++) {
		if (d->buckets[i].key) {
			BCArrayAdd(arr, d->buckets[i].value);
		}
	}
	return arr;
}

BCDictionaryRef BCDictionaryCreateWithObjects(const bool retain, const size_t count, ...) {
	const BCMutableDictionaryRef d = BCMutableDictionaryCreate();
	va_list args;
	va_start(args, count);
	for (size_t i = 0; i < count/2; i++) {
		const BCObjectRef key = va_arg(args, BCObjectRef);
		const BCObjectRef val = va_arg(args, BCObjectRef);
		BCDictionarySet(d, key, val);
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

static void _DicPutInternal(BCDictionaryEntry* buckets, const size_t cap, const BCObjectRef key, const BCObjectRef val) {
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