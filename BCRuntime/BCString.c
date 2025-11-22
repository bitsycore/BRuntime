#include "BCString.h"

#include <stdarg.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

#include "BCObject.h"

// =========================================================
// MARK: Struct
// =========================================================

typedef struct BCString {
	BCObject base;
	atomic_size_t length;
	atomic_uint_fast32_t hash;
	char* buffer;
} BCString;

// =========================================================
// MARK: Class Methods
// =========================================================

uint32_t StringHashImpl(const BCObjectRef obj) {
	return BCStringHash((BCStringRef) obj);
}

bool StringEqualImpl(const BCObjectRef a, const BCObjectRef b) {
	const BCStringRef s1 = (BCStringRef) a;
	const BCStringRef s2 = (BCStringRef) b;

	if (s1 == s2) return true;
	if ( BC_FLAG_HAS(s1->base.flags, BC_STRING_FLAG_POOLED) && BC_FLAG_HAS( s2->base.flags, BC_STRING_FLAG_POOLED )) return false;

	// Check Hash Cache
	const uint32_t h1 = atomic_load(&s1->hash);
	const uint32_t h2 = atomic_load(&s2->hash);
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
	.dealloc = NULL,
	.hash = StringHashImpl,
	.equal = StringEqualImpl,
	.toString = StringToStringImpl,
	.copy = StringCopyImpl,
	.allocSize = sizeof(BCString)
};

const BCClassRef kBCStringClassRef = (BCClassRef) &kBCStringClass;

// =========================================================
// MARK: Pool
// =========================================================

typedef struct StringPoolNode {
	BCStringRef str;
	struct StringPoolNode* next;
} StringPoolNode;

static struct {
	mtx_t lock;
	StringPoolNode* buckets[BC_STRING_POOL_SIZE];
} StringPool;

void ___BCINTERNAL___StringPoolInit(void) {
	mtx_init(&StringPool.lock, mtx_plain);
	memset(StringPool.buckets, 0, sizeof(StringPool.buckets));
}

void ___BCINTERNAL___StringPoolDeinit(void) {
	mtx_destroy(&StringPool.lock);
	for (size_t i = 0; i < BC_STRING_POOL_SIZE; i++) {
		const StringPoolNode* node = StringPool.buckets[i];
		while (node) {
			const StringPoolNode* next = node->next;
			free( node->str );
			node = next;
			if (next == NULL) StringPool.buckets[i] = NULL;
		}
	}
}

static BCStringRef StringPoolGetOrInsert(const char* text, const size_t len, const uint32_t hash, const bool static_string) {
	const uint32_t idx = hash % BC_STRING_POOL_SIZE;

	mtx_lock(&StringPool.lock);

	// Lookup
	const StringPoolNode* node = StringPool.buckets[idx];
	while (node) {

		// Fast path for static literal strings
		if (static_string && node->str->buffer == text) {
			const BCStringRef ret = (BCStringRef) BCRetain((BCObjectRef) node->str);
			mtx_unlock(&StringPool.lock);
			return ret;
		}

		if (atomic_load(&node->str->hash) == hash) {
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

	// ============================================
	// Insert
	const size_t extraAlloc = static_string ? sizeof(StringPoolNode) : sizeof(StringPoolNode) + (len + 1);
	const BCStringRef newStr = (BCStringRef)BCAllocObjectWithExtra(
		(BCClassRef)&kBCStringClass,
		NULL,
		extraAlloc,
		static_string ? BC_STRING_FLAG_POOLED | BC_STRING_FLAG_STATIC : BC_STRING_FLAG_POOLED
	);

	if (!static_string) {
		newStr->buffer = (char*) ( &newStr->buffer + 1 );
		memcpy(newStr->buffer, text, len + 1);
	} else {
		newStr->buffer = (char*)text;
	}

	newStr->length = len;
	newStr->hash = hash;

	StringPoolNode* newNode = (StringPoolNode*) ( (char*)newStr + sizeof(BCString) + (static_string ? 0 : len + 1) );
	newNode->str = newStr;
	newNode->next = StringPool.buckets[idx];
	StringPool.buckets[idx] = newNode;

	mtx_unlock(&StringPool.lock);

	newStr->base.ref_count = 0;

	return newStr;
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
	const BCStringRef str = (BCStringRef) BCAllocObjectWithExtra((BCClassRef) &kBCStringClass, NULL, len + 1, BC_OBJECT_FLAG_REFCOUNT);
	str->buffer = (char*) ( &str->buffer + 1 );
	vsnprintf(str->buffer, len + 1, fmt, args);
	va_end(args);

	str->length = BC_LEN_UNSET;
	str->hash = BC_HASH_UNSET;

	return str;
}

BCStringRef BCStringPooled(const char* text) {
	if (!text) return NULL;
	return StringPoolGetOrInsert(text, strlen(text), ___BCINTERNAL___StringHasher(text), false);
}

BCStringRef BCStringPooledWithInfo(const char* text, const size_t len, const uint32_t hash, const bool static_string) {
	if (!text) return NULL;
	return StringPoolGetOrInsert(text, len, hash, static_string);
}

size_t BCStringLength(const BCStringRef str) {
	if (!str) return 0;
	size_t len = atomic_load_explicit(&str->length, memory_order_relaxed);
	if (len == BC_LEN_UNSET) {
		len = strlen(str->buffer);
		atomic_store_explicit(&str->length, len, memory_order_relaxed);
	}
	return len;
}

uint32_t BCStringHash(const BCStringRef str) {
	if (!str) return 0;
	uint32_t hash = atomic_load_explicit(&str->hash, memory_order_relaxed);
	if (hash == BC_HASH_UNSET) {
		hash = ___BCINTERNAL___StringHasher(str->buffer);
		atomic_store_explicit(&str->hash, hash, memory_order_relaxed);
	}
	return hash;
}

const char* BCStringCPtr(const BCStringRef str) {
	return str->buffer;
}

// =========================================================
// MARK: Debug
// =========================================================

#define DGRAY "\033[48;5;234m"
#define BLACK "\033[48;5;235m"
#define RESET "\033[0m"
#define BOLD "\033[1m"
void BCStringPoolDebugDump(void) {
	mtx_lock(&StringPool.lock);
	const clock_t start = clock();

	// --------------------------------------------------------------------------
	// FOOTER
	printf("\n"
		"                                          "BOLD"String Pool Dump"RESET"\n"
		"┌"          "────────┬────────────────────────────────────────────────┬────────────┬────────┬──────────────────"     "┐\n"
		"│"DGRAY BOLD" Bucket │                     Value                      │    Hash    │ Length │       Next       "RESET"│\n"
		"├"BLACK     "────────┼────────────────────────────────────────────────┼────────────┼────────┼──────────────────"RESET"┤\n"
	);

	// Print entries
	size_t count = 0;
	for (size_t i = 0; i < BC_STRING_POOL_SIZE; i++) {
		const StringPoolNode* node = StringPool.buckets[i];
		while (node) {
			const BCStringRef str = node->str;
			const char* value = str->buffer;
			const uint32_t hash = atomic_load(&str->hash);
			const size_t length = BCStringLength(str);

			// Truncate long strings for display
			char displayValue[47];
			if (strlen(value) > 43) {
				snprintf(displayValue, sizeof(displayValue), "%.43s...", value);
			} else {
				snprintf(displayValue, sizeof(displayValue), "%s", value);
			}

			// Format next pointer
			char nextPtr[18];
			if (node->next) {
				snprintf(nextPtr, sizeof(nextPtr), "%p", (void*)node->next);
			} else {
				snprintf(nextPtr, sizeof(nextPtr), "NULL");
			}

			if (count % 2 == 0)
				printf( "│"BLACK" %-6zu │ %-46s │ 0x%08X │ %-6zu │ %-16s "RESET"│\n", i, displayValue, hash, length, nextPtr);
			else
				printf( "│"DGRAY" %-6zu │ %-46s │ 0x%08X │ %-6zu │ %-16s "RESET"│\n", i, displayValue, hash, length, nextPtr);

			count++;
			node = node->next;
		}
	}

	mtx_unlock(&StringPool.lock);

	const clock_t end = clock();
	const double elapsed = (double)(end - start) / CLOCKS_PER_SEC * 1000;

	// --------------------------------------------------------------------------
	// FOOTER
	printf(
		"└────────┴────────────────────────────────────────────────┴────────────┴────────┴──────────────────┘\n"
		"    %zu entr%s (%fms)\n\n", count, count == 1 ? "y" : "ies", elapsed
	);
}