#ifndef BC_BC_H
#define BC_BC_H

#include "BCMacroTools.h"
#include "BCTypes.h"
#include "BCObject.h"

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

static inline BCObjectRef ___BCRetain(void* obj) { return (BCObjectRef)BCRetain(obj); }

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
    BCStringRef: ___BCRetain,          \
    BCNumberRef: ___BCRetain,          \
    BCArrayRef: ___BCRetain,           \
    BCAllocatorRef: ___BCRetain,       \
    BCDictionaryRef: ___BCRetain,      \
    BCObjectRef: ___BCRetain           \
)(__val__)

#define __BC_ARR_IMPL(_counter_, __sub__, ...) ({ \
    BCArrayRef BC_M_CAT(___temp_arr_impl___,_counter_) = ___BCArrayCreateWithObjectsNoRetain( \
        BC_ARG_COUNT(__VA_ARGS__), \
        BC_ARG_MAP($, __VA_ARGS__) \
    );                                                 \
	if (__sub__) atomic_fetch_sub(&((BCObjectRef)BC_M_CAT(___temp_arr_impl___,_counter_))->ref_count, 1);\
    BC_M_CAT(___temp_arr_impl___,_counter_); \
})

#define $ARR(...) __BC_ARR_IMPL(__COUNTER__, false, __VA_ARGS__)
#define $$ARR(...) __BC_ARR_IMPL(__COUNTER__, true, __VA_ARGS__)

#define __BC_DIC_IMPL(_counter_, __sub__, ...) ({ \
    _Static_assert(((BC_ARG_COUNT(__VA_ARGS__)) % 2) == 0, "DIC requires an even number of arguments"); \
    BCDictionaryRef BC_M_CAT(___temp_dic_impl___,_counter_) = ___BCDictionaryCreateWithObjectsNoRetain( \
        BC_ARG_COUNT(__VA_ARGS__), \
        BC_ARG_MAP($, __VA_ARGS__) \
    ); \
	if (__sub__) atomic_fetch_sub(&((BCObjectRef)BC_M_CAT(___temp_dic_impl___,_counter_))->ref_count, 1);\
    BC_M_CAT(___temp_dic_impl___,_counter_);      \
})

#define $DIC(...) __BC_DIC_IMPL(__COUNTER__, false, __VA_ARGS__)
#define $$DIC(...) __BC_DIC_IMPL(__COUNTER__, true, __VA_ARGS__)

#endif //BC_BC_H
