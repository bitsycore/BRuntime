#include "BCString.h"

#include <stdarg.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

#include "BCObject.h"

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
	if (s1->_isPooled && s2->_isPooled) return false;

	// Check Hash Cache
	const uint32_t h1 = atomic_load(&s1->_hash);
	const uint32_t h2 = atomic_load(&s2->_hash);
	if (h1 != BC_HASH_UNSET && h2 != BC_HASH_UNSET && h1 != h2) return false;

	if (BCStringLength(s1) != BCStringLength(s2)) return false;

	return memcmp(s1->buffer, s2->buffer, BCStringLength(s1)) == 0;
}

BCStringRef StringToStringImpl(const BCObjectRef obj) {
	return (BCStringRef)BCRetain(obj);
}

BCObjectRef StringCopyImpl(const BCObjectRef obj) {
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
	.toString = StringToStringImpl,
	.copy = StringCopyImpl,
	.allocSize = sizeof(BCString)
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
} StringPool;

static void StringPoolInit(void) {
	mtx_init(&StringPool.lock, mtx_plain);
	memset(StringPool.buckets, 0, sizeof(StringPool.buckets));
}

uint32_t ___BCInternalStringHasher(const char* s) {
	uint32_t hash = 2166136261u;
	while (*s) {
		hash ^= (uint8_t) *s++;
		hash *= 16777619;
	}
	return hash == BC_HASH_UNSET ? 1 : hash;
}

static BCStringRef StringPoolGetOrInsert(const char* text, const size_t len, const uint32_t hash, bool static_string) {
	call_once(&StringPool.flag, StringPoolInit);

	const uint32_t idx = hash % POOL_SIZE;

	mtx_lock(&StringPool.lock);

	// Lookup
	const StringPoolNode* node = StringPool.buckets[idx];
	while (node) {
		// Safe to read hash non-atomically inside lock as pooled strings are settled
		if (atomic_load(&node->str->_hash) == hash) {
			if (len != BCStringLength(node->str)) {
				node = node->next;
				continue;
			}
			if (strcmp(node->str->buffer, text) == 0) {
				const BCStringRef ret = (BCStringRef) BCRetain((BCObjectRef) node->str);
				mtx_unlock(&StringPool.lock);
				return ret;
			}
		}
		node = node->next;
	}

	// Insert
	const BCStringRef newStr = (BCStringRef) BCAllocObject((BCClassRef) &kBCStringClass, NULL);
	newStr->buffer = malloc(len + 1);
	if (!static_string) {
		memcpy(newStr->buffer, text, len + 1);
	} else {
		newStr->buffer = (char*)text;
	}
	newStr->_length = len;
	newStr->_hash = hash;
	newStr->_isPooled = true;

	StringPoolNode* newNode = malloc(sizeof(StringPoolNode));
	newNode->str = newStr; // Pool holds ownership
	newNode->next = StringPool.buckets[idx];
	StringPool.buckets[idx] = newNode;

	mtx_unlock(&StringPool.lock);

	return (BCStringRef) BCRetain((BCObjectRef) newStr); // Retain for caller
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
	const BCStringRef str = (BCStringRef) BCAllocObject((BCClassRef) &kBCStringClass, NULL);
	str->buffer = malloc(len + 1);
	vsnprintf(str->buffer, len + 1, fmt, args);
	va_end(args);

	str->_length = BC_LEN_UNSET;
	str->_hash = BC_HASH_UNSET;

	str->_isPooled = false;
	return str;
}

BCStringRef BCStringPooled(const char* text) {
	if (!text) return NULL;
	return StringPoolGetOrInsert(text, strlen(text), ___BCInternalStringHasher(text), false);
}

BCStringRef BCStringPooledWithInfo(const char* text, const size_t len, const uint32_t hash, const bool static_string) {
	if (!text) return NULL;
	return StringPoolGetOrInsert(text, len, hash, static_string);
}

size_t BCStringLength(const BCStringRef str) {
	if (!str) return 0;
	size_t len = atomic_load_explicit(&str->_length, memory_order_relaxed);
	if (len == BC_LEN_UNSET) {
		len = strlen(str->buffer);
		atomic_store_explicit(&str->_length, len, memory_order_relaxed);
	}
	return len;
}

uint32_t BCStringHash(const BCStringRef str) {
	if (!str) return 0;
	uint32_t hash = atomic_load_explicit(&str->_hash, memory_order_relaxed);
	if (hash == BC_HASH_UNSET) {
		hash = ___BCInternalStringHasher(str->buffer);
		atomic_store_explicit(&str->_hash, hash, memory_order_relaxed);
	}
	return hash;
}

const char* BCStringCPtr(const BCStringRef str) {
	return str->buffer;
}