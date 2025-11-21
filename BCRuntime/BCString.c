#include "BCString.h"

#include <stdatomic.h>
#include <threads.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "BCObject.h"

#define BC_HASH_Unset 0
#define BC_LEN_Unset SIZE_MAX
#define POOL_SIZE 1024

// =========================================================
// MARK: Struct
// =========================================================

typedef struct BCString {
	BCObject base;
	char* buffer;
	atomic_size_t _length;
	atomic_uint_fast32_t _hash;
	bool _isPooled;
} BCString;

// =========================================================
// MARK: Class Methods
// =========================================================

void StringDeallocImpl(const BCObjectRef obj) {
	const BCStringRef s = (BCStringRef) obj;
	if (s->_isPooled) return;
	if (s->buffer) free(s->buffer);
}

uint32_t StringHashImpl(const BCObjectRef obj) {
	return BCStringHash((BCStringRef) obj);
}

bool StringEqualImpl(const BCObjectRef a, const BCObjectRef b) {
	const BCStringRef s1 = (BCStringRef) a;
	const BCStringRef s2 = (BCStringRef) b;

	if (s1 == s2) return true;

	// Optimization: If both are pooled and pointers differ, they are NOT equal
	if (s1->_isPooled && s2->_isPooled) return false;

	// Check Hash Cache
	const uint32_t h1 = atomic_load(&s1->_hash);
	const uint32_t h2 = atomic_load(&s2->_hash);
	if (h1 != BC_HASH_Unset && h2 != BC_HASH_Unset && h1 != h2) return false;

	// Check Length
	if (BCStringLength(s1) != BCStringLength(s2)) return false;

	// Byte Compare
	return memcmp(s1->buffer, s2->buffer, BCStringLength(s1)) == 0;
}

void StringDescriptionImpl(const BCObjectRef obj, const int indent) {
	for (int i = 0; i < indent; i++) printf("  ");
	printf("\"%s\"", ((const BCStringRef) obj)->buffer);
}

BCObject* StringCopyImpl(const BCObjectRef obj) {
	return BCRetain(obj);
}

// =========================================================
// MARK: Class
// =========================================================

static const BCClass kBCStringClass = {
	.name = "BCString",
	.dealloc = StringDeallocImpl,
	.hash = StringHashImpl,
	.equal = StringEqualImpl,
	.description = StringDescriptionImpl,
	.copy = StringCopyImpl,
	.bytes_size = sizeof(BCString)
};

// =========================================================
// MARK: Pool
// =========================================================

typedef struct StringPoolNode {
	BCStringRef str;
	struct StringPoolNode* next;
} StringPoolNode;

static struct {
	mtx_t lock;
	once_flag flag;
	StringPoolNode* buckets[POOL_SIZE];
} _BCStringPool;

static void PoolInit(void) {
	mtx_init(&_BCStringPool.lock, mtx_plain);
	memset(_BCStringPool.buckets, 0, sizeof(_BCStringPool.buckets));
}

static uint32_t RawHash(const char* s) {
	uint32_t hash = 2166136261u;
	while (*s) {
		hash ^= (uint8_t) *s++;
		hash *= 16777619;
	}
	return (hash == BC_HASH_Unset) ? 1 : hash;
}

static BCStringRef StringPoolGetOrInsert(const char* text, size_t len, uint32_t hash) {
	call_once(&_BCStringPool.flag, PoolInit);

	const uint32_t idx = hash % POOL_SIZE;

	mtx_lock(&_BCStringPool.lock);

	// Lookup
	const StringPoolNode* node = _BCStringPool.buckets[idx];
	while (node) {
		// Safe to read hash non-atomically inside lock as pooled strings are settled
		if (atomic_load(&node->str->_hash) == hash) {
			if (strcmp(node->str->buffer, text) == 0) {
				const BCStringRef ret = (BCStringRef) BCRetain((BCObject*) node->str);
				mtx_unlock(&_BCStringPool.lock);
				return ret;
			}
		}
		node = node->next;
	}

	// Insert
	const BCStringRef newStr = (BCStringRef) BCObjectAlloc((BCClassRef) &kBCStringClass, NULL);
	newStr->buffer = malloc(len + 1);
	memcpy(newStr->buffer, text, len + 1);
	newStr->_length = len;
	newStr->_hash = hash;
	newStr->_isPooled = true;

	StringPoolNode* newNode = malloc(sizeof(StringPoolNode));
	newNode->str = newStr; // Pool holds ownership
	newNode->next = _BCStringPool.buckets[idx];
	_BCStringPool.buckets[idx] = newNode;

	mtx_unlock(&_BCStringPool.lock);

	return (BCStringRef) BCRetain((BCObject*) newStr); // Retain for caller
}

// =========================================================
// MARK: Public
// =========================================================

BCStringRef BCStringCreate(const char* fmt, ...) {

	va_list args, copy;
	va_start(args, fmt);
	va_copy(copy, args);
	const int len = vsnprintf(NULL, 0, fmt, copy);
	va_end(copy);
	const BCStringRef str = (BCStringRef) BCObjectAlloc((BCClassRef) &kBCStringClass, NULL);
	str->buffer = malloc(len + 1);
	vsnprintf(str->buffer, len + 1, fmt, args);
	va_end(args);

	str->_length = BC_LEN_Unset;
	str->_hash = BC_HASH_Unset;

	str->_isPooled = false;
	return str;
}

BCStringRef BCStringConst(const char* text) {
	if (!text) return NULL;
	return StringPoolGetOrInsert(text, strlen(text), RawHash(text));
}

size_t BCStringLength(const BCStringRef str) {
	if (!str) return 0;
	size_t len = atomic_load_explicit(&str->_length, memory_order_relaxed);
	if (len == BC_LEN_Unset) {
		len = strlen(str->buffer);
		atomic_store_explicit(&str->_length, len, memory_order_relaxed);
	}
	return len;
}

uint32_t BCStringHash(const BCStringRef str) {
	if (!str) return 0;
	uint32_t hash = atomic_load_explicit(&str->_hash, memory_order_relaxed);
	if (hash == BC_HASH_Unset) {
		hash = RawHash(str->buffer);
		atomic_store_explicit(&str->_hash, hash, memory_order_relaxed);
	}
	return hash;
}