#include "BO_String.h"

#include "BCore/Memory/BC_Memory.h"
#include "BCore/Thread/BC_Threads.h"

#include "BO_Object.h"
#include "../BF_Class.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// =========================================================
// MARK: Struct
// =========================================================

typedef struct BO_String {
	BO_Object base;
	BC_atomic_size length;
	BC_atomic_uint_fast32 hash;
	char *buffer;
} BO_String;

// =========================================================
// MARK: Impl
// =========================================================

uint32_t IMPL_StringHash(const BO_ObjectRef obj) {
	return BO_StringHash((BO_StringRef) obj);
}

BC_bool IMPL_StringEqual(const BO_ObjectRef a, const BO_ObjectRef b) {
	const BO_StringRef s1 = (BO_StringRef) a;
	const BO_StringRef s2 = (BO_StringRef) b;

	if (s1 == s2) return BC_true;
	if (BC_FLAG_HAS(s1->base.flags, BC_STRING_FLAG_POOLED) &&
	    BC_FLAG_HAS(s2->base.flags, BC_STRING_FLAG_POOLED))
		return BC_false;

	// Check Hash Cache
	const uint32_t h1 = BC_atomic_load(&s1->hash);
	const uint32_t h2 = BC_atomic_load(&s2->hash);
	if (h1 != BC_HASH_UNSET && h2 != BC_HASH_UNSET && h1 != h2) return BC_false;

	if (BO_StringLength(s1) != BO_StringLength(s2)) return BC_false;

	return memcmp(s1->buffer, s2->buffer, BO_StringLength(s1)) == 0;
}

BO_StringRef IMPL_StringToString(const BO_ObjectRef obj) {
	return (BO_StringRef) BO_Retain(obj);
}

BO_ObjectRef IMPL_StringCopy(const BO_ObjectRef obj) { return BO_Retain(obj); }

// =========================================================
// MARK: Class
// =========================================================

static BF_Class kBO_StringClass = {
	.name = "BO_String",
	.dealloc = NULL,
	.hash = IMPL_StringHash,
	.equal = IMPL_StringEqual,
	.toString = IMPL_StringToString,
	.copy = IMPL_StringCopy,
	.allocSize = sizeof(BO_String)
};

BF_ClassId BO_StringClassId() { return kBO_StringClass.id; }

void INTERNAL_BO_StringInitialize(void) {
	BF_ClassRegistryInsert(&kBO_StringClass);
}

// =========================================================
// MARK: Pool
// =========================================================

typedef struct StringPoolNode {
	BO_StringRef str;
	struct StringPoolNode *next;
} StringPoolNode;

static struct {
	BC_SPINLOCK_MAYBE(lock);
	StringPoolNode *buckets[BC_STRING_POOL_SIZE];
} StringPool;

void INTERNAL_BO_StringPoolInitialize(void) {
	BC_SpinlockInit(&StringPool.lock);
	memset(StringPool.buckets, 0, sizeof(StringPool.buckets));
}

void INTERNAL_BO_StringPoolDeinitialize(void) {
	BC_SpinlockDestroy(&StringPool.lock);
	for (size_t i = 0; i < BC_STRING_POOL_SIZE; i++) {
		const StringPoolNode *node = StringPool.buckets[i];
		while (node) {
			const StringPoolNode *next = node->next;
			BC_Free(node->str);
			node = next;
			if (next == NULL) StringPool.buckets[i] = NULL;
		}
	}
}

static BO_StringRef PRIV_StringPoolGetOrInsert(
	const char *text,
	const size_t len,
	const uint32_t hash,
	const BC_bool static_string
) {
	const uint32_t idx = hash % BC_STRING_POOL_SIZE;

	BC_SpinlockLock(&StringPool.lock);

	// Lookup
	const StringPoolNode *node = StringPool.buckets[idx];
	while (node) {
		// Fast path for static literal strings
		if (static_string && node->str->buffer == text) {
			const BO_StringRef ret = (BO_StringRef) BO_Retain((BO_ObjectRef) node->str);
			BC_SpinlockUnlock(&StringPool.lock);
			return ret;
		}

		if (BC_atomic_load(&node->str->hash) == hash) {
			// CHECK IF LEN SET
			if (len != BO_StringLength(node->str)) {
				node = node->next;
				continue;
			}
			if (strcmp(node->str->buffer, text) == 0) {
				const BO_StringRef ret = (BO_StringRef) BO_Retain((BO_ObjectRef) node->str);
				BC_SpinlockUnlock(&StringPool.lock);
				return ret;
			}
		}

		node = node->next;
	}

	// ============================================
	// Insert
	const size_t extraAlloc = static_string
		                          ? sizeof(StringPoolNode)
		                          : sizeof(StringPoolNode) + (len + 1);
	const BO_StringRef newStr = (BO_StringRef) BO_ObjectAllocWithConfig(
		NULL,
		kBO_StringClass.id,
		extraAlloc,
		static_string
			? BC_STRING_FLAG_POOLED | BC_STRING_FLAG_STATIC
			: BC_STRING_FLAG_POOLED
	);

	if (!static_string) {
		newStr->buffer = (char *) (&newStr->buffer + 1);
		memcpy(newStr->buffer, text, len + 1);
	} else {
		newStr->buffer = (char *) text;
	}

	newStr->length = len;
	newStr->hash = hash;

	StringPoolNode *newNode =
			(StringPoolNode *) ((char *) newStr + sizeof(BO_String) +
			                    (static_string ? 0 : len + 1));
	newNode->str = newStr;
	newNode->next = StringPool.buckets[idx];
	StringPool.buckets[idx] = newNode;

	BC_SpinlockUnlock(&StringPool.lock);

	newStr->base.ref_count = 0;

	return newStr;
}

// =========================================================
// MARK: Constructors
// =========================================================

#include "../BF_Format.h"

BO_StringRef BO_StringCreate(const char *fmt, ...) {
	va_list args, copy;
	va_start(args, fmt);
	va_copy(copy, args);
	const int len = BF_PrintStringVa(NULL, 0, fmt, copy);
	va_end(copy);

	const BO_StringRef str = (BO_StringRef) BO_ObjectAllocWithConfig(
		NULL,
		kBO_StringClass.id,
		len + 1,
		BC_OBJECT_FLAG_REFCOUNT
	);
	str->buffer = (char *) (&str->buffer + 1);

	BF_PrintStringVa(str->buffer, len + 1, fmt, args);
	va_end(args);

	str->length = BC_LEN_UNSET;
	str->hash = BC_HASH_UNSET;

	return str;
}

BO_StringRef BO_StringPooled(const char *text) {
	if (!text) return NULL;
	return PRIV_StringPoolGetOrInsert(
		text,
		strlen(text),
		INTERNAL_BO_StringHasher(text),
		BC_false
	);
}

BO_StringRef BO_StringPooledWithInfo(
	const char *text,
	const size_t len,
	const uint32_t hash,
	const BC_bool static_string
) {
	if (!text) return NULL;
	return PRIV_StringPoolGetOrInsert(text, len, hash, static_string);
}

// =========================================================
// MARK: Methods
// =========================================================

size_t BO_StringLength(const BO_StringRef str) {
	if (!str) return 0;
	size_t len = BC_atomic_load(&str->length);
	if (len == BC_LEN_UNSET) {
		len = strlen(str->buffer);
		BC_atomic_store(&str->length, len);
	}
	return len;
}

uint32_t BO_StringHash(const BO_StringRef str) {
	if (!str) return 0;
	uint32_t hash = BC_atomic_load(&str->hash);
	if (hash == BC_HASH_UNSET) {
		hash = INTERNAL_BO_StringHasher(str->buffer);
		BC_atomic_store(&str->hash, hash);
	}
	return hash;
}

const char *BO_StringCPtr(const BO_StringRef str) { return str->buffer; }

// =========================================================
// MARK: Debug
// =========================================================

#define CG "\033[48;5;234m"
#define CB "\033[48;5;235m"
#define R "\033[0m"
#define SB "\033[1m"

void BO_StringPoolDebugDump(void) {
	BC_SpinlockLock(&StringPool.lock);
	const clock_t start = clock();

	// --------------------------------------------------------------------------
	// FOOTER
	printf(
		"\n                                          " SB "String Pool Dump" R "\n"
		"┌────────┬────────────────────────────────────────────────┬────────────┬────────┬──────────────────┐\n"
		"│" CB SB " Bucket " R CB "│" SB "                     Value                      " R CB "│" SB "    Hash    " R CB "│" SB " Length " R CB "│" SB "       Next       " R "│\n"
		"├" CG "────────┼────────────────────────────────────────────────┼────────────┼────────┼──────────────────" R "┤\n"
	);

	// Print entries
	size_t count = 0;
	for (size_t i = 0; i < BC_STRING_POOL_SIZE; i++) {
		const StringPoolNode *node = StringPool.buckets[i];
		while (node) {
			const BO_StringRef str = node->str;
			const char *value = str->buffer;
			const uint32_t hash = BC_atomic_load(&str->hash);
			const size_t length = BO_StringLength(str);

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
				snprintf(nextPtr, sizeof(nextPtr), "%p", (void *)node->next);
			} else {
				snprintf(nextPtr, sizeof(nextPtr), "NULL");
			}

			if (count % 2 == 0)
				printf(
					"│" CB " %-6zu │ %-46s │ 0x%08X │ %-6zu │ %-16s " R "│\n",
					i,
					displayValue,
					hash,
					length,
					nextPtr
				);
			else
				printf(
					"│" CG " %-6zu │ %-46s │ 0x%08X │ %-6zu │ %-16s " R "│\n",
					i,
					displayValue,
					hash,
					length,
					nextPtr
				);

			count++;
			node = node->next;
		}
	}

	BC_SpinlockUnlock(&StringPool.lock);

	const clock_t end = clock();
	const double elapsed = (double) (end - start) / CLOCKS_PER_SEC * 1000;

	// --------------------------------------------------------------------------
	// FOOTER
	printf(
		"└────────┴────────────────────────────────────────────────┴────────────┴────────┴──────────────────┘\n"
		"    %zu entr%s (%fms)\n\n",
		count,
		count == 1 ? "y" : "ies",
		elapsed
	);
}
