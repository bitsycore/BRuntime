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
// MARK: Class
// =========================================================

void _BCStrDealloc(BCObject* obj) {
	BCString* s = (BCString*)obj;
	if (s->_isPooled) return;
	if (s->buffer) free(s->buffer);
}

uint32_t _BCStrHashImpl(const BCObject* obj) {
	return BCStringHash((BCString*)obj);
}

bool _BCStrEqImpl(const BCObject* a, const BCObject* b) {
	BCString* s1 = (BCString*)a;
	BCString* s2 = (BCString*)b;

	if (s1 == s2) return true;

	// Optimization: If both are pooled, and pointers differ, they are NOT equal
	if (s1->_isPooled && s2->_isPooled) return false;

	// Check Hash Cache
	uint32_t h1 = atomic_load(&s1->_hash);
	uint32_t h2 = atomic_load(&s2->_hash);
	if (h1 != BC_HASH_Unset && h2 != BC_HASH_Unset && h1 != h2) return false;

	// Check Length
	if (BCStringLength(s1) != BCStringLength(s2)) return false;

	// Byte Compare
	return memcmp(s1->buffer, s2->buffer, BCStringLength(s1)) == 0;
}

void _BCStrDesc(const BCObject* obj, int indent) {
	printf("\"%s\"", ((const BCString*)obj)->buffer);
}

BCObject* _BCStrCopy(const BCObject* obj) {
	return BCRetain((BCObject*)obj);
}

static const BCClass kBCStringClass = {
	.name = "BCString",
	.dealloc = _BCStrDealloc,
	.hash = _BCStrHashImpl,
	.equal = _BCStrEqImpl,
	.description = _BCStrDesc,
	.copy = _BCStrCopy
};

// =========================================================
// MARK: Pool
// =========================================================

typedef struct StringPoolNode {
	BCString* str;
	struct StringPoolNode* next;
} StringPoolNode;

static struct {
	mtx_t lock;
	once_flag flag;
	StringPoolNode* buckets[POOL_SIZE];
} _BCStringPool;

static void _BCPoolInit(void) {
	mtx_init(&_BCStringPool.lock, mtx_plain);
	memset(_BCStringPool.buckets, 0, sizeof(_BCStringPool.buckets));
}

static uint32_t _RawHash(const char* s) {
	uint32_t hash = 2166136261u;
	while (*s) {
		hash ^= (uint8_t)*s++;
		hash *= 16777619;
	}
	return (hash == BC_HASH_Unset) ? 1 : hash;
}

static BCString* _BCStringPoolGetOrInsert(const char* text, size_t len, uint32_t hash) {
	call_once(&_BCStringPool.flag, _BCPoolInit);

	uint32_t idx = hash % POOL_SIZE;

	mtx_lock(&_BCStringPool.lock);

	// Lookup
	StringPoolNode* node = _BCStringPool.buckets[idx];
	while (node) {
		// Safe to read hash non-atomically inside lock as pooled strings are settled
		if (atomic_load(&node->str->_hash) == hash) {
			if (strcmp(node->str->buffer, text) == 0) {
				BCString* ret = (BCString*)BCRetain((BCObject*)node->str);
				mtx_unlock(&_BCStringPool.lock);
				return ret;
			}
		}
		node = node->next;
	}

	// Insert
	BCString* newStr = (BCString*)BCAllocRaw(&kBCStringClass, NULL, sizeof(BCString) - sizeof(BCObject));
	newStr->buffer = malloc(len + 1);
	memcpy(newStr->buffer, text, len + 1);
	atomic_init(&newStr->_length, len);
	atomic_init(&newStr->_hash, hash);
	newStr->_isPooled = true;

	StringPoolNode* newNode = malloc(sizeof(StringPoolNode));
	newNode->str = newStr; // Pool holds ownership
	newNode->next = _BCStringPool.buckets[idx];
	_BCStringPool.buckets[idx] = newNode;

	mtx_unlock(&_BCStringPool.lock);

	return (BCString*)BCRetain((BCObject*)newStr); // Retain for caller
}

// =========================================================
// MARK: Public
// =========================================================

BCString* BCStringCreate(const char* fmt, ...) {
	va_list args, copy;
	va_start(args, fmt);
	va_copy(copy, args);
	int len = vsnprintf(NULL, 0, fmt, copy);
	va_end(copy);

	BCString* str = (BCString*)BCAllocRaw(&kBCStringClass, NULL, sizeof(BCString) - sizeof(BCObject));
	str->buffer = malloc(len + 1);
	vsnprintf(str->buffer, len + 1, fmt, args);
	va_end(args);

	atomic_init(&str->_length, BC_LEN_Unset);
	atomic_init(&str->_hash, BC_HASH_Unset);
	str->_isPooled = false;
	return str;
}

BCString* BCStringConst(const char* text) {
	if (!text) return NULL;
	return _BCStringPoolGetOrInsert(text, strlen(text), _RawHash(text));
}

size_t BCStringLength(BCString* self) {
	if (!self) return 0;
	size_t len = atomic_load_explicit(&self->_length, memory_order_relaxed);
	if (len == BC_LEN_Unset) {
		len = strlen(self->buffer);
		atomic_store_explicit(&self->_length, len, memory_order_relaxed);
	}
	return len;
}

uint32_t BCStringHash(BCString* self) {
	if (!self) return 0;
	uint32_t hash = atomic_load_explicit(&self->_hash, memory_order_relaxed);
	if (hash == BC_HASH_Unset) {
		hash = _RawHash(self->buffer);
		atomic_store_explicit(&self->_hash, hash, memory_order_relaxed);
	}
	return hash;
}