#ifndef BC_BC_H
#define BC_BC_H

#include "BCMacroTools.h"

#define $OBJ (BCObjectRef)

#define $VAR __auto_type

#ifdef WIN32
#define ___BC___PLATFORM_EXTRA_$_MACRO
#else
#define ___BC___PLATFORM_EXTRA_$_MACRO long long: BCNumberCreateInt64,
#endif

static inline BCObjectRef ___BCRetain(void* obj) { return (BCObjectRef)obj; }

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
    default: ___BCRetain       			\
)(__val__)


#define MAP_0(F)
#define MAP_1(F, X)      F(X)
#define MAP_2(F, X, ...) F(X), MAP_1(F, __VA_ARGS__)
#define MAP_3(F, X, ...) F(X), MAP_2(F, __VA_ARGS__)
#define MAP_4(F, X, ...) F(X), MAP_3(F, __VA_ARGS__)
#define MAP_5(F, X, ...) F(X), MAP_4(F, __VA_ARGS__)
#define MAP_6(F, X, ...) F(X), MAP_5(F, __VA_ARGS__)
#define MAP_7(F, X, ...) F(X), MAP_6(F, __VA_ARGS__)
#define MAP_8(F, X, ...) F(X), MAP_7(F, __VA_ARGS__)
#define MAP_9(F, X, ...) F(X), MAP_8(F, __VA_ARGS__)
#define MAP_10(F, X, ...) F(X), MAP_9(F, __VA_ARGS__)
#define MAP_11(F, X, ...) F(X), MAP_10(F, __VA_ARGS__)
#define MAP_12(F, X, ...) F(X), MAP_11(F, __VA_ARGS__)
#define MAP_13(F, X, ...) F(X), MAP_12(F, __VA_ARGS__)
#define MAP_14(F, X, ...) F(X), MAP_13(F, __VA_ARGS__)
#define MAP_15(F, X, ...) F(X), MAP_14(F, __VA_ARGS__)
#define MAP_16(F, X, ...) F(X), MAP_15(F, __VA_ARGS__)
#define MAP_17(F, X, ...) F(X), MAP_16(F, __VA_ARGS__)
#define MAP_18(F, X, ...) F(X), MAP_17(F, __VA_ARGS__)
#define MAP_19(F, X, ...) F(X), MAP_18(F, __VA_ARGS__)
#define MAP_20(F, X, ...) F(X), MAP_19(F, __VA_ARGS__)

#define GET_MAP_NAME(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,N,...) MAP_##N

#define MAP(F, ...) \
    __VA_OPT__( \
        GET_MAP_NAME(__VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)(F, __VA_ARGS__) \
    )

/* Re-using the counter logic from the previous answer for the size_t argument */
#define ARG_COUNT(...) \
    ARG_COUNT_INNER(__VA_ARGS__ __VA_OPT__(,) 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define ARG_COUNT_INNER(...) ARG_COUNT_SELECT(__VA_ARGS__)
#define ARG_COUNT_SELECT(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,N, ...) N

/* --- 5. The Final API Macro --- */

#define $ARR(...) \
    ___BCArrayCreateWithObjectsNoRetain( \
        ARG_COUNT(__VA_ARGS__), \
        MAP($, __VA_ARGS__) \
    )

#endif //BC_BC_H
