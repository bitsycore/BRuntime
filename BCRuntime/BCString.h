#ifndef BCRUNTIME_BCSTRING_H
#define BCRUNTIME_BCSTRING_H

#include "BCTypes.h"

#include <stddef.h>

#define BC_HASH_UNSET 0xFFFFFFFF
#define BC_LEN_UNSET SIZE_MAX
#define BC_STRING_POOL_SIZE 16384

static uint32_t ___BCINTERNAL___StringHasher(const char* s) {
	uint32_t hash = 2166136261u;
	while (*s) {
		hash ^= (uint8_t) *s++;
		hash *= 16777619;
	}
	return hash == BC_HASH_UNSET ? 1 : hash;
}

#ifdef __RESHARPER__
#define ___BCINTERNAL___REQUIRE_LITERAL_ASSERT(x)
#else
#define ___BCINTERNAL___REQUIRE_LITERAL_ASSERT(x) _Static_assert(__builtin_constant_p(x), "Must be a literal")
#endif

#define BC_REQUIRE_LITERAL(x) ({ \
	___BCINTERNAL___REQUIRE_LITERAL_ASSERT(x); \
	("" x ""); \
	(x); \
})

__attribute__((format(printf, 1, 2)))
BCStringRef BCStringCreate(const char* fmt, ...);

BCStringRef BCStringPooled(const char* text);
BCStringRef BCStringPooledWithInfo(const char* text, size_t len, uint32_t hash, bool static_string);
#define		BCStringPooledLiteral(__text__) BCStringPooledWithInfo( BC_REQUIRE_LITERAL(__text__), sizeof(__text__) / sizeof((__text__)[0]) - 1, ___BCINTERNAL___StringHasher((__text__)), true )

size_t BCStringLength(BCStringRef str);
uint32_t BCStringHash(BCStringRef str);
const char* BCStringCPtr(BCStringRef str);

void BCStringPoolDebugDump(void);

#endif //BCRUNTIME_BCSTRING_H