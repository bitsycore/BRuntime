#ifndef BC_BC_H
#define BC_BC_H

#include "BCMacroTools.h"
#include "BCTypes.h"

// ================================================
// MARK: SHORTCUT
// ================================================

#define $OBJ (BCObjectRef)
#define $VAR __auto_type

// ================================================
// MARK: BOXING
// ================================================

#ifdef WIN32
#define ___BC___PLATFORM_EXTRA_$_MACRO
#else
#define ___BC___PLATFORM_EXTRA_$_MACRO long long: BCNumberCreateInt64,
#endif

static inline BCObjectRef ___BCCastToObject(void* obj) { return (BCObjectRef)obj; }

#define $(__val__) _Generic((__val__), \
    int8_t: BCNumberCreateInt8,        \
    int16_t: BCNumberCreateInt16,      \
    int32_t: BCNumberCreateInt32,      \
    int64_t: BCNumberCreateInt64,      \
    uint8_t: BCNumberCreateUInt8,      \
    uint16_t: BCNumberCreateUInt16,    \
    uint32_t: BCNumberCreateUInt32,    \
    uint64_t: BCNumberCreateUInt64,    \
    ___BC___PLATFORM_EXTRA_$_MACRO     \
    float: BCNumberCreateFloat,        \
    double: BCNumberCreateDouble,      \
    bool: BCNumberGetBool,             \
    char*: BCStringCreate,             \
    const char*: BCStringCreate,       \
    BCStringRef: ___BCCastToObject,    \
    BCNumberRef: ___BCCastToObject,    \
    BCArrayRef: ___BCCastToObject,     \
    BCAllocatorRef: ___BCCastToObject, \
    BCDictionaryRef: ___BCCastToObject,\
    BCObjectRef: ___BCCastToObject     \
)(__val__)

#define $ARR(...) \
    ___BCArrayCreateWithObjectsNoRetain( \
        BC_ARG_COUNT(__VA_ARGS__), \
        BC_ARG_MAP($, __VA_ARGS__) \
    )

#define REQUIRE_EVEN_ARG_COUNT(count) \


#define $DIC(...) ({\
    _Static_assert(((BC_ARG_COUNT(__VA_ARGS__)) % 2) == 0, "DIC requires an even number of arguments");\
    $VAR ___temp = ___BCDictionaryCreateWithObjectsNoRetain( \
        BC_ARG_COUNT(__VA_ARGS__), \
        BC_ARG_MAP($, __VA_ARGS__) \
    ); \
    ___temp; \
})

#endif //BC_BC_H
