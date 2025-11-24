#include "BCString.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "BCClass.h"
#include "BCObject.h"
#include "Utilities/BCMemory.h"
#include "Utilities/BCThreads.h"

// =========================================================
// MARK: Struct
// =========================================================

typedef struct BCString {
	BCObject base;
	BC_atomic_size length;
	BC_atomic_uint_fast32 hash;
	char* buffer;
} BCString;

// =========================================================
// MARK: Class Methods
// =========================================================

uint32_t StringHashImpl(const BCObjectRef obj) {
	return BCStringHash((BCStringRef) obj);
}

BC_bool StringEqualImpl(const BCObjectRef a, const BCObjectRef b) {
	const BCStringRef s1 = (BCStringRef) a;
	const BCStringRef s2 = (BCStringRef) b;

	if (s1 == s2) return BC_true;
	if ( BC_FLAG_HAS(s1->base.flags, BC_STRING_FLAG_POOLED) && BC_FLAG_HAS( s2->base.flags, BC_STRING_FLAG_POOLED )) return BC_false;

	// Check Hash Cache
	const uint32_t h1 = BC_atomic_load(&s1->hash);
	const uint32_t h2 = BC_atomic_load(&s2->hash);
	if (h1 != BC_HASH_UNSET && h2 != BC_HASH_UNSET && h1 != h2) return BC_false;

	if (BCStringLength(s1) != BCStringLength(s2)) return BC_false;

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
	BC_MUTEX_MAYBE(lock);
	StringPoolNode* buckets[BC_STRING_POOL_SIZE];
} StringPool;

void ___BCINTERNAL___StringPoolInitialize(void) {
	BCMutexInit(&StringPool.lock);
	memset(StringPool.buckets, 0, sizeof(StringPool.buckets));
}

void ___BCINTERNAL___StringPoolDeinitialize(void) {
	BCMutexDestroy(&StringPool.lock);
	for (size_t i = 0; i < BC_STRING_POOL_SIZE; i++) {
		const StringPoolNode* node = StringPool.buckets[i];
		while (node) {
			const StringPoolNode* next = node->next;
			BCFree( node->str );
			node = next;
			if (next == NULL) StringPool.buckets[i] = NULL;
		}
	}
}

static BCStringRef StringPoolGetOrInsert(const char* text, const size_t len, const uint32_t hash, const BC_bool static_string) {
	const uint32_t idx = hash % BC_STRING_POOL_SIZE;

	BCMutexLock(&StringPool.lock);

	// Lookup
	const StringPoolNode* node = StringPool.buckets[idx];
	while (node) {

		// Fast path for static literal strings
		if (static_string && node->str->buffer == text) {
			const BCStringRef ret = (BCStringRef) BCRetain((BCObjectRef) node->str);
			BCMutexUnlock(&StringPool.lock);
			return ret;
		}

		if (BC_atomic_load(&node->str->hash) == hash) {
			// CHECK IF LEN SET
			if (len != BCStringLength(node->str)) {
				node = node->next;
				continue;
			}
			if (strcmp(node->str->buffer, text) == 0) {
				const BCStringRef ret = (BCStringRef) BCRetain((BCObjectRef) node->str);
				BCMutexUnlock(&StringPool.lock);
				return ret;
			}
		}

		node = node->next;
	}

	// ============================================
	// Insert
	const size_t extraAlloc = static_string ? sizeof(StringPoolNode) : sizeof(StringPoolNode) + (len + 1);
	const BCStringRef newStr = (BCStringRef)BCObjectAllocWithConfig(
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

	BCMutexUnlock(&StringPool.lock);

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
	const BCStringRef str = (BCStringRef) BCObjectAllocWithConfig((BCClassRef) &kBCStringClass, NULL, len + 1, BC_OBJECT_FLAG_REFCOUNT);
	str->buffer = (char*) ( &str->buffer + 1 );
	vsnprintf(str->buffer, len + 1, fmt, args);
	va_end(args);

	str->length = BC_LEN_UNSET;
	str->hash = BC_HASH_UNSET;

	return str;
}

BCStringRef BCStringPooled(const char* text) {
	if (!text) return NULL;
	return StringPoolGetOrInsert(text, strlen(text), ___BCINTERNAL___StringHasher(text), BC_false);
}

BCStringRef BCStringPooledWithInfo(const char* text, const size_t len, const uint32_t hash, const BC_bool static_string) {
	if (!text) return NULL;
	return StringPoolGetOrInsert(text, len, hash, static_string);
}

size_t BCStringLength(const BCStringRef str) {
	if (!str) return 0;
	size_t len = BC_atomic_load(&str->length);
	if (len == BC_LEN_UNSET) {
		len = strlen(str->buffer);
		BC_atomic_store(&str->length, len);
	}
	return len;
}

uint32_t BCStringHash(const BCStringRef str) {
	if (!str) return 0;
	uint32_t hash = BC_atomic_load(&str->hash);
	if (hash == BC_HASH_UNSET) {
		hash = ___BCINTERNAL___StringHasher(str->buffer);
		BC_atomic_store(&str->hash, hash);
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
	BCMutexLock(&StringPool.lock);
	const clock_t start = clock();

	// --------------------------------------------------------------------------
	// FOOTER
	printf("\n"
		"                                          "BOLD"String Pool Dump"RESET"\n"
		"┌"          "────────┬────────────────────────────────────────────────┬────────────┬────────┬──────────────────"     "┐\n"
		"│"BLACK BOLD" Bucket "RESET BLACK"│"BOLD"                     Value                      "RESET BLACK"│"
		BOLD"    Hash    "RESET BLACK"│"BOLD" Length "RESET BLACK"│"BOLD"       Next       "RESET"│\n"
		"├"DGRAY     "────────┼────────────────────────────────────────────────┼────────────┼────────┼──────────────────"RESET"┤\n"
	);

	// Print entries
	size_t count = 0;
	for (size_t i = 0; i < BC_STRING_POOL_SIZE; i++) {
		const StringPoolNode* node = StringPool.buckets[i];
		while (node) {
			const BCStringRef str = node->str;
			const char* value = str->buffer;
			const uint32_t hash = BC_atomic_load(&str->hash);
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

	BCMutexUnlock(&StringPool.lock);

	const clock_t end = clock();
	const double elapsed = (double)(end - start) / CLOCKS_PER_SEC * 1000;

	// --------------------------------------------------------------------------
	// FOOTER
	printf(
		"└────────┴────────────────────────────────────────────────┴────────────┴────────┴──────────────────┘\n"
		"    %zu entr%s (%fms)\n\n", count, count == 1 ? "y" : "ies", elapsed
	);
}