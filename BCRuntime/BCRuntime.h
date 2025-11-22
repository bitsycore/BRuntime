#ifndef BC_BCRUNTIME_H
#define BC_BCRUNTIME_H

#include "BCObject.h"
#include "BCTypes.h"
#include "Utilities/BCKeywords.h"
#include "Utilities/BCMacro.h"

// ================================================
// MARK: BOXING
// ================================================

#ifdef WIN32
#define ___BC___PLATFORM_EXTRA_$_MACRO
#else
#define ___BC___PLATFORM_EXTRA_$_MACRO long long: BCNumberCreateInt64,
#endif

static inline BCObjectRef ___BCRetain(void* obj) { return BCRetain(obj); }

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
    bool: ___BCBoolSelect,             \
    char*: BCStringCreate,             \
    const char*: BCStringCreate,       \
    BCStringRef: ___BCRetain,          \
    BCNumberRef: ___BCRetain,          \
    BCArrayRef: ___BCRetain,           \
    BCAllocatorRef: ___BCRetain,       \
    BCDictionaryRef: ___BCRetain,      \
    BCObjectRef: ___BCRetain           \
)(__val__)

#define __BC_$$_IMPL(__result__, __type__) ( (__type__) BCAutorelease($OBJ (__result__)) )
#define $$(__val__) __BC_$$_IMPL($(__val__), $TYPE ( $(__val__) ))

#define __BC_ARR_IMPL(_counter_, ...) ({ \
    BCAutoreleasePoolPush(); \
    BCArrayRef BC_M_CAT(___temp_arr_impl___,_counter_) = BCArrayCreateWithObjects( \
		false, /*NO RETAIN*/ \
        BC_ARG_COUNT(__VA_ARGS__), \
        BC_ARG_MAP($, __VA_ARGS__) \
    ); \
    BCAutoreleasePoolPop(); \
    BC_M_CAT(___temp_arr_impl___,_counter_); \
})

#define $ARR(...) __BC_ARR_IMPL(BC_M_CAT(___temp_arr_impl___,__COUNTER__), __VA_ARGS__)
#define $$ARR(...) ( (BCArrayRef) BCAutorelease( $OBJ $ARR(__VA_ARGS__) ) )

#define __BC_DIC_IMPL(_name_, ...) ({ \
    BCAutoreleasePoolPush();\
    _Static_assert(((BC_ARG_COUNT(__VA_ARGS__)) % 2) == 0, "DIC requires an even number of arguments"); \
    BCDictionaryRef _name_ = BCDictionaryCreateWithObjects( \
		false, /*NO RETAIN*/ \
        BC_ARG_COUNT(__VA_ARGS__), \
        BC_ARG_MAP($, __VA_ARGS__) \
    ); \
    BCAutoreleasePoolPop(); \
    _name_; \
})

#define $DIC(...) __BC_DIC_IMPL(BC_M_CAT(___temp_dic_impl___,__COUNTER__), __VA_ARGS__)
#define $$DIC(...) ( (BCDictionaryRef) BCAutorelease( $OBJ $DIC(__VA_ARGS__) ) )

// ================================================
// MARK: RUNTIME INITIALIZATION
// ================================================

void __internal_BCInitialize(void);
void __internal_BCUninitialize(void);

__attribute__((constructor))
static inline void __inline_BCInitialize(void) {
	__internal_BCInitialize();
}

__attribute__((destructor))
static inline void __inline_BCUninitialize(void) {
	__internal_BCUninitialize();
}

#endif //BC_BCRUNTIME_H
