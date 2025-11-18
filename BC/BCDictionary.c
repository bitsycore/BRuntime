#include "BCDictionary.h"
#include "BCArray.h"

#include <stdio.h>

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

static void _Indent(int level);
static void _DictPutInternal(BCDictionaryEntry* buckets, size_t cap, BCObjectRef key, BCObjectRef val);

// =========================================================
// MARK: Class
// =========================================================

void _BCDictDealloc(BCObjectRef obj) {
	BCDictionaryRef d = (BCDictionaryRef) obj;
	for (size_t i = 0; i < d->capacity; i++) {
		if (d->buckets[i].key) {
			BCRelease(d->buckets[i].key);
			BCRelease(d->buckets[i].value);
		}
	}
	free(d->buckets);
}

void _BCDictDesc(BCObjectRef obj, int indent) {
	BCDictionaryRef d = (BCDictionaryRef) obj;
	printf("{\n");
	for (size_t i = 0; i < d->capacity; i++) {
		if (d->buckets[i].key) {
			_Indent(indent + 1);
			BCLog(d->buckets[i].key, indent + 1);
			printf(" : ");
			d->buckets[i].value->cls->description(d->buckets[i].value, indent + 1);
			printf(",\n");
		}
	}
	_Indent(indent);
	printf("}");
}

static const BCClass kBCDictClass = {
	"BCDictionary", _BCDictDealloc, NULL, NULL, _BCDictDesc, NULL
};

// =========================================================
// MARK: Public
// =========================================================

BCDictionaryRef BCDictionaryCreate() {
	BCMutableDictionaryRef dic = BCMutableDictionaryCreate();
	dic->isMutable = false;
	return  dic;
}

BCMutableDictionaryRef BCMutableDictionaryCreate() {
	BCDictionaryRef d = (BCDictionaryRef) BCAllocRaw((BCClassRef)&kBCDictClass, NULL, sizeof(BCDictionary) - sizeof(BCObject));
	d->isMutable = true;
	d->capacity = 8;
	d->buckets = calloc(d->capacity, sizeof(BCDictionaryEntry));
	return (BCMutableDictionaryRef)d;
}

void BCDictionarySet(BCMutableDictionaryRef d, BCObjectRef key, BCObjectRef val) {
	BCDictionary *dict = (BCDictionaryRef)d;
	if (!dict->isMutable) return;

	// Resize Check
	if (dict->count >= (size_t) ((double) dict->capacity * 0.75)) {
		size_t newCap = dict->capacity * 2;
		BCDictionaryEntry* newBuckets = calloc(newCap, sizeof(BCDictionaryEntry));
		if (!newBuckets) return;
		for (size_t i = 0; i < dict->capacity; i++) {
			if (dict->buckets[i].key) {
				_DictPutInternal(newBuckets, newCap, dict->buckets[i].key, dict->buckets[i].value);
			}
		}
		free(dict->buckets);
		dict->buckets = newBuckets;
		dict->capacity = newCap;
	}

	// Insert
	uint32_t hash = BCHash(key);
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
	dict->buckets[idx].key = BCCopy(key);
	dict->buckets[idx].value = BCRetain(val);
	dict->count++;
}

BCObjectRef BCDictionaryGet(BCDictionaryRef d, BCObjectRef key) {
	uint32_t hash = BCHash(key);
	size_t idx = hash % d->capacity;
	size_t start = idx;
	while (d->buckets[idx].key) {
		if (BCEqual(d->buckets[idx].key, key)) {
			return d->buckets[idx].value;
		}
		idx = (idx + 1) % d->capacity;
		if (idx == start) break;
	}
	return NULL;
}

BCArrayRef BCDictionaryKeys(BCDictionaryRef d) {
	BCArrayRef arr = BCArrayCreate();
	for (size_t i = 0; i < d->capacity; i++) {
		if (d->buckets[i].key) {
			BCArrayAdd(arr, d->buckets[i].key);
		}
	}
	return arr;
}

// =========================================================
// MARK: Internal
// =========================================================

static void _Indent(int level) {
	for (int i = 0; i < level; i++) printf(" ");
}

static void _DictPutInternal(BCDictionaryEntry* buckets, size_t cap, BCObjectRef key, BCObjectRef val) {
	uint32_t hash = BCHash(key);
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