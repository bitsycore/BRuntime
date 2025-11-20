#ifndef BC_BC_H
#define BC_BC_H

#include "BCMacroTools.h"
#include "BCTypes.h"

#define $OBJ (BCObjectRef)

#define $VAR __auto_type

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
    default: ___BCCastToObject         \
)(__val__)

#define $ARR(...) \
    ___BCArrayCreateWithObjectsNoRetain( \
        ARG_COUNT(__VA_ARGS__), \
        MAP($, __VA_ARGS__) \
    )

void BCInitialize(void);

#endif //BC_BC_H
