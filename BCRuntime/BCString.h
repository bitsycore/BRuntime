#ifndef BCRUNTIME_BCSTRING_H
#define BCRUNTIME_BCSTRING_H

#include "BCTypes.h"

#include <stddef.h>

#define BC_HASH_UNSET 0
#define BC_LEN_UNSET SIZE_MAX

uint32_t ___BCInternalStringHasher(const char* s);

#ifdef __RESHARPER__
#define ___BC___REQUIRE_LITERAL_ASSERT(x)
#else
#define ___BC___REQUIRE_LITERAL_ASSERT(x) _Static_assert(__builtin_constant_p(x), "Must be a literal")
#endif

#define BC_REQUIRE_LITERAL(x) ({ \
	___BC___REQUIRE_LITERAL_ASSERT(x); \
	("" x ""); \
	(x); \
})

__attribute__((format(printf, 1, 2)))
BCStringRef BCStringCreate(const char* fmt, ...);

BCStringRef BCStringPooled(const char* text);
BCStringRef BCStringPooledWithInfo(const char* text, size_t len, uint32_t hash, bool static_string);
#define		BCStringPooledLiteral(__text__) BCStringPooledWithInfo( BC_REQUIRE_LITERAL(__text__), sizeof(__text__) / sizeof((__text__)[0]) - 1, ___BCInternalStringHasher((__text__)), true )

size_t BCStringLength(BCStringRef str);
uint32_t BCStringHash(BCStringRef str);
const char* BCStringCPtr(BCStringRef str);

#endif //BCRUNTIME_BCSTRING_H