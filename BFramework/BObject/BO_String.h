#ifndef BCRUNTIME_BCSTRING_H
#define BCRUNTIME_BCSTRING_H

#include "BCore/BC_Macro.h"

#include "../BF_Types.h"

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

__attribute__((const, always_inline)) static inline uint32_t
___BF_INTERNAL___StringHasher(const char *s) {
  uint32_t hash = 2166136261u;
  while (*s) {
    hash ^= (uint8_t)*s++;
    hash *= 16777619;
  }
  return hash == BC_HASH_UNSET ? 1 : hash;
}

// =========================================================
// MARK: Class
// =========================================================

BF_ClassId BO_StringClassId();

// =========================================================
// MARK: Constructor
// =========================================================

BO_StringRef BO_StringCreate(const char *fmt, ...);

BO_StringPooledRef BO_StringPooled(const char *text);
BO_StringPooledRef BO_StringPooledWithInfo(const char *text, size_t len,
                                         uint32_t hash, BC_bool static_string);
#define BO_StringPooledLiteral(__text__)                                        \
  BO_StringPooledWithInfo(BC_REQUIRE_LITERAL(__text__),                         \
                         sizeof(__text__) / sizeof((__text__)[0]) - 1,         \
                         ___BF_INTERNAL___StringHasher((__text__)), BC_true)

// =========================================================
// MARK: Properties
// =========================================================

size_t BO_StringLength(BO_StringRef str);
uint32_t BO_StringHash(BO_StringRef str);
const char *BO_StringCPtr(BO_StringRef str);

// =========================================================
// MARK: Debug
// =========================================================

void BO_StringPoolDebugDump(void);

#endif // BCRUNTIME_BCSTRING_H
