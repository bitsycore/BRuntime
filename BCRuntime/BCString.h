#ifndef BCRUNTIME_BCSTRING_H
#define BCRUNTIME_BCSTRING_H

#include "BCTypes.h"
#include "Utilities/BCMacro.h"

#include <stddef.h>

// =========================================================
// MARK: Settings
// =========================================================

#define BC_HASH_UNSET 0xFFFFFFFF
#define BC_LEN_UNSET SIZE_MAX
#define BC_STRING_POOL_SIZE 16384

// =========================================================
// MARK: Flags
// =========================================================

#define BC_STRING_FLAG_POOLED (1 << 8)
#define BC_STRING_FLAG_STATIC (1 << 9)

// =========================================================
// MARK: Internal
// =========================================================

static uint32_t ___BCINTERNAL___StringHasher(const char* s) {
	uint32_t hash = 2166136261u;
	while (*s) {
		hash ^= (uint8_t) *s++;
		hash *= 16777619;
	}
	return hash == BC_HASH_UNSET ? 1 : hash;
}

// =========================================================
// MARK: Class
// =========================================================

extern const BCClassRef kBCStringClassRef;

__attribute__((format(printf, 1, 2)))
BCStringRef BCStringCreate(const char* fmt, ...);

BCStringPooledRef BCStringPooled(const char* text);
BCStringPooledRef BCStringPooledWithInfo(const char* text, size_t len, uint32_t hash, bool static_string);
#define		BCStringPooledLiteral(__text__) \
	BCStringPooledWithInfo( BC_REQUIRE_LITERAL(__text__), sizeof(__text__) / sizeof((__text__)[0]) - 1, ___BCINTERNAL___StringHasher((__text__)), true )

size_t BCStringLength(BCStringRef str);
uint32_t BCStringHash(BCStringRef str);
const char* BCStringCPtr(BCStringRef str);

// =========================================================
// MARK: Debug
// =========================================================

void BCStringPoolDebugDump(void);

#endif //BCRUNTIME_BCSTRING_H