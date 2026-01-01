#ifndef BFRAMEWORK_BOX_MACRO_H
#define BFRAMEWORK_BOX_MACRO_H

#include "BCore/BC_Keywords.h"

#include "BF_Types.h"
#include "BObject/BO_Object.h"

// ================================================
// MARK: BOXING $
// ================================================

#if defined(WIN32) || defined(__APPLE__)
#define INTERNAL_BF_$_EXTRA
#else
#define INTERNAL_BF_$_EXTRA long long : BO_NumberCreateInt64,
#endif

static inline BO_ObjectRef INTERNAL_BF_Retain(void* obj) { return BO_Retain(obj); }

#define $(...)                                                                                                                                                                     \
	_Generic((BC_ARG_FIRST(__VA_ARGS__)),                                                                                                                                          \
		int8_t: BO_NumberCreateInt8,                                                                                                                                                \
		int16_t: BO_NumberCreateInt16,                                                                                                                                              \
		int32_t: BO_NumberCreateInt32,                                                                                                                                              \
		int64_t: BO_NumberCreateInt64,                                                                                                                                              \
		uint8_t: BO_NumberCreateUInt8,                                                                                                                                              \
		uint16_t: BO_NumberCreateUInt16,                                                                                                                                            \
		uint32_t: BO_NumberCreateUInt32,                                                                                                                                            \
		uint64_t: BO_NumberCreateUInt64,                                                                                                                                            \
		INTERNAL_BF_$_EXTRA float: BO_NumberCreateFloat,                                                                                                                        \
		double: BO_NumberCreateDouble,                                                                                                                                              \
		char*: BO_StringCreate,                                                                                                                                                     \
		const char*: BO_StringCreate,                                                                                                                                               \
		BC_bool: INTERNAL_BO_BoolSelect,                                                                                                                                       \
		BO_StringRef: INTERNAL_BF_Retain,                                                                                                                                       \
		BO_NumberRef: INTERNAL_BF_Retain,                                                                                                                                       \
		BO_ListRef: INTERNAL_BF_Retain,                                                                                                                                         \
		BC_AllocatorRef: INTERNAL_BF_Retain,                                                                                                                                    \
		BO_MapRef: INTERNAL_BF_Retain,                                                                                                                                          \
		BO_ObjectRef: INTERNAL_BF_Retain)(__VA_ARGS__)

#define INTERNAL_BF_$$_IMPL(__result__, __type__) ((__type__)BF_Autorelease($OBJ(__result__)))
#define $$(...) INTERNAL_BF_$$_IMPL($(__VA_ARGS__), BC_TYPE($(__VA_ARGS__)))

// ================================================
// MARK: ARRAY
// ================================================

#define INTERNAL_BF_ARR_IMPL(_name_, ...) ({ \
	BF_AutoreleasePoolPush(); \
	BO_ListRef _name_ = BO_ListCreateWithObjects( \
		BC_false, /*NO RETAIN*/ \
		BC_ARG_COUNT(__VA_ARGS__), \
		BC_ARG_MAP($, __VA_ARGS__)); \
	BF_AutoreleasePoolPop(); \
	_name_; \
})

#define $LIST(...) INTERNAL_BF_ARR_IMPL(BC_M_CAT(___temp_arr_impl___, __COUNTER__), __VA_ARGS__)
#define $$LIST(...) ((BO_ListRef)BF_Autorelease($OBJ $LIST(__VA_ARGS__)))

// ================================================
// MARK: MAP
// ================================================

#define INTERNAL_BF_MAP_IMPL(_name_, ...) ({ \
	BF_AutoreleasePoolPush(); \
	_Static_assert(((BC_ARG_COUNT(__VA_ARGS__)) % 2) == 0, "Map requires an even number of arguments"); \
	BO_MapRef _name_ = BO_MapCreateWithObjects( \
		BC_false, /*NO RETAIN*/ \
		BC_ARG_COUNT(__VA_ARGS__), BC_ARG_MAP($, __VA_ARGS__)); \
	BF_AutoreleasePoolPop(); \
	_name_; \
})

#define $MAP(...) INTERNAL_BF_MAP_IMPL(BC_M_CAT(___temp_dic_impl___, __COUNTER__), __VA_ARGS__)
#define $$MAP(...) ((BO_MapRef)BF_Autorelease($OBJ $MAP(__VA_ARGS__)))

// ================================================
// MARK: SET
// ================================================

#define INTERNAL_BF_SET_IMPL(_name_, ...) ({                                                                                                                                                                             \
	BF_AutoreleasePoolPush(); \
	BO_SetRef _name_ = BO_SetCreateWithObjects( \
		BC_false, /*NO RETAIN*/ \
		BC_ARG_COUNT(__VA_ARGS__), \
		BC_ARG_MAP($, __VA_ARGS__)); \
	BF_AutoreleasePoolPop();                                                                                                                                                    \
	_name_;                                                                                                                                                                    \
})

#define $SET(...) INTERNAL_BF_SET_IMPL(BC_M_CAT(___temp_set_impl___, __COUNTER__), __VA_ARGS__)
#define $$SET(...) ((BO_SetRef)BF_Autorelease($OBJ $SET(__VA_ARGS__)))

#endif //BFRAMEWORK_BOX_MACRO_H