#ifndef BFRAMEWORK_BOX_MACRO_H
#define BFRAMEWORK_BOX_MACRO_H

#include "BF_Types.h"
#include "Object/BO_Object.h"

// ================================================
// MARK: BOXING $
// ================================================

#if defined(WIN32) || defined(__APPLE__)
#define ___BF_INTERNAL___$_EXTRA
#else
#define ___BF_INTERNAL___$_EXTRA long long : BO_NumberCreateInt64,
#endif

static inline BO_ObjectRef ___BF_INTERNAL___Retain(void* obj) { return BO_Retain(obj); }

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
		___BF_INTERNAL___$_EXTRA float: BO_NumberCreateFloat,                                                                                                                        \
		double: BO_NumberCreateDouble,                                                                                                                                              \
		char*: BO_StringCreate,                                                                                                                                                     \
		const char*: BO_StringCreate,                                                                                                                                               \
		BC_bool: ___BO_INTERNAL___BoolSelect,                                                                                                                                       \
		BO_StringRef: ___BF_INTERNAL___Retain,                                                                                                                                       \
		BO_NumberRef: ___BF_INTERNAL___Retain,                                                                                                                                       \
		BO_ListRef: ___BF_INTERNAL___Retain,                                                                                                                                         \
		BC_AllocatorRef: ___BF_INTERNAL___Retain,                                                                                                                                    \
		BO_MapRef: ___BF_INTERNAL___Retain,                                                                                                                                          \
		BO_ObjectRef: ___BF_INTERNAL___Retain)(__VA_ARGS__)

#define ___BF_INTERNAL___$$_IMPL(__result__, __type__) ((__type__)BFAutorelease($OBJ(__result__)))
#define $$(...) ___BF_INTERNAL___$$_IMPL($(__VA_ARGS__), BC_TYPE($(__VA_ARGS__)))

// ================================================
// MARK: ARRAY
// ================================================

#define ___BF_INTERNAL___ARR_IMPL(_name_, ...) ({ \
	BFAutoreleasePoolPush(); \
	BO_ListRef _name_ = BO_ListCreateWithObjects( \
		BC_false, /*NO RETAIN*/ \
		BC_ARG_COUNT(__VA_ARGS__), \
		BC_ARG_MAP($, __VA_ARGS__)); \
	BFAutoreleasePoolPop(); \
	_name_; \
})

#define $LIST(...) ___BF_INTERNAL___ARR_IMPL(BC_M_CAT(___temp_arr_impl___, __COUNTER__), __VA_ARGS__)
#define $$LIST(...) ((BO_ListRef)BFAutorelease($OBJ $LIST(__VA_ARGS__)))

// ================================================
// MARK: MAP
// ================================================

#define ___BF_INTERNAL___MAP_IMPL(_name_, ...) ({ \
	BFAutoreleasePoolPush(); \
	_Static_assert(((BC_ARG_COUNT(__VA_ARGS__)) % 2) == 0, "Map requires an even number of arguments"); \
	BO_MapRef _name_ = BO_MapCreateWithObjects( \
		BC_false, /*NO RETAIN*/ \
		BC_ARG_COUNT(__VA_ARGS__), BC_ARG_MAP($, __VA_ARGS__)); \
	BFAutoreleasePoolPop(); \
	_name_; \
})

#define $MAP(...) ___BF_INTERNAL___MAP_IMPL(BC_M_CAT(___temp_dic_impl___, __COUNTER__), __VA_ARGS__)
#define $$MAP(...) ((BO_MapRef)BFAutorelease($OBJ $MAP(__VA_ARGS__)))

// ================================================
// MARK: SET
// ================================================

#define ___BF_INTERNAL___SET_IMPL(_name_, ...) ({                                                                                                                                                                             \
	BFAutoreleasePoolPush(); \
	BO_SetRef _name_ = BO_SetCreateWithObjects( \
		BC_false, /*NO RETAIN*/ \
		BC_ARG_COUNT(__VA_ARGS__), \
		BC_ARG_MAP($, __VA_ARGS__)); \
	BFAutoreleasePoolPop();                                                                                                                                                    \
	_name_;                                                                                                                                                                    \
})

#define $SET(...) ___BF_INTERNAL___SET_IMPL(BC_M_CAT(___temp_set_impl___, __COUNTER__), __VA_ARGS__)
#define $$SET(...) ((BO_SetRef)BFAutorelease($OBJ $SET(__VA_ARGS__)))

#endif //BFRAMEWORK_BOX_MACRO_H