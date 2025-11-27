#ifndef BC_BCRUNTIME_H
#define BC_BCRUNTIME_H

#include "BCNumber.h"
#include "BCObject.h"
#include "BCTypes.h"
#include "List/BCList.h"
#include "Map/BCMap.h"
#include "Pool/BCAutoreleasePool.h"
#include "Pool/BCReleasePool.h"
#include "String/BCString.h"
#include "Utilities/BC_Keywords.h"
#include "Utilities/BC_Macro.h"

// ================================================
// MARK: RUNTIME INITIALIZATION
// ================================================

void BCInitialize(int argc, char** argv);
void BCDeinitialize(void);

#ifdef BC_RUNTIME_MAIN

int BCMain(void);

int main(int argc, char** argv) {
	BCInitialize(argc, argv);
	BCMain();
	BCDeinitialize();
}

#endif

// ================================================
// MARK: BOXING $
// ================================================

#if defined(WIN32) || defined(__APPLE__)
#define ___BCINTERNAL___$_EXTRA
#else
#define ___BCINTERNAL___$_EXTRA long long : BCNumberCreateInt64,
#endif

static inline BCObjectRef ___BCINTERNAL___Retain(void* obj) { return BCRetain(obj); }

#define $(...)                                                                                                                                                                     \
	_Generic((BC_ARG_FIRST(__VA_ARGS__)),                                                                                                                                          \
		int8_t: BCNumberCreateInt8,                                                                                                                                                \
		int16_t: BCNumberCreateInt16,                                                                                                                                              \
		int32_t: BCNumberCreateInt32,                                                                                                                                              \
		int64_t: BCNumberCreateInt64,                                                                                                                                              \
		uint8_t: BCNumberCreateUInt8,                                                                                                                                              \
		uint16_t: BCNumberCreateUInt16,                                                                                                                                            \
		uint32_t: BCNumberCreateUInt32,                                                                                                                                            \
		uint64_t: BCNumberCreateUInt64,                                                                                                                                            \
		___BCINTERNAL___$_EXTRA float: BCNumberCreateFloat,                                                                                                                        \
		double: BCNumberCreateDouble,                                                                                                                                              \
		char*: BCStringCreate,                                                                                                                                                     \
		const char*: BCStringCreate,                                                                                                                                               \
		BC_bool: ___BCINTERNAL___BoolSelect,                                                                                                                                       \
		BCStringRef: ___BCINTERNAL___Retain,                                                                                                                                       \
		BCNumberRef: ___BCINTERNAL___Retain,                                                                                                                                       \
		BCListRef: ___BCINTERNAL___Retain,                                                                                                                                         \
		BCAllocatorRef: ___BCINTERNAL___Retain,                                                                                                                                    \
		BCMapRef: ___BCINTERNAL___Retain,                                                                                                                                          \
		BCObjectRef: ___BCINTERNAL___Retain)(__VA_ARGS__)

#define ___BCINTERNAL___$$_IMPL(__result__, __type__) ((__type__)BCAutorelease($OBJ(__result__)))
#define $$(...) ___BCINTERNAL___$$_IMPL($(__VA_ARGS__), $TYPE($(__VA_ARGS__)))

// ================================================
// MARK: ARRAY
// ================================================

#define ___BCINTERNAL___ARR_IMPL(_name_, ...)                                                                                                                                      \
	({                                                                                                                                                                             \
		BCAutoreleasePoolPush();                                                                                                                                                   \
		BCListRef _name_ = BCListCreateWithObjects(BC_false, /*NO RETAIN*/                                                                                                         \
												   BC_ARG_COUNT(__VA_ARGS__), BC_ARG_MAP($, __VA_ARGS__));                                                                         \
		BCAutoreleasePoolPop();                                                                                                                                                    \
		_name_;                                                                                                                                                                    \
	})

#define $LIST(...) ___BCINTERNAL___ARR_IMPL(BC_M_CAT(___temp_arr_impl___, __COUNTER__), __VA_ARGS__)
#define $$LIST(...) ((BCListRef)BCAutorelease($OBJ $LIST(__VA_ARGS__)))

// ================================================
// MARK: MAP
// ================================================

#define ___BCINTERNAL___MAP_IMPL(_name_, ...)                                                                                                                                      \
	({                                                                                                                                                                             \
		BCAutoreleasePoolPush();                                                                                                                                                   \
		_Static_assert(((BC_ARG_COUNT(__VA_ARGS__)) % 2) == 0, "Map requires an even number of arguments");                                                                        \
		BCMapRef _name_ = BCMapCreateWithObjects(BC_false, /*NO RETAIN*/                                                                                                           \
												 BC_ARG_COUNT(__VA_ARGS__), BC_ARG_MAP($, __VA_ARGS__));                                                                           \
		BCAutoreleasePoolPop();                                                                                                                                                    \
		_name_;                                                                                                                                                                    \
	})

#define $MAP(...) ___BCINTERNAL___MAP_IMPL(BC_M_CAT(___temp_dic_impl___, __COUNTER__), __VA_ARGS__)
#define $$MAP(...) ((BCMapRef)BCAutorelease($OBJ $MAP(__VA_ARGS__)))

// ================================================
// MARK: SET
// ================================================

#include "Set/BCSet.h"

#define ___BCINTERNAL___SET_IMPL(_name_, ...)                                                                                                                                      \
	({                                                                                                                                                                             \
		BCAutoreleasePoolPush();                                                                                                                                                   \
		BCSetRef _name_ = BCSetCreateWithObjects(BC_false, /*NO RETAIN*/                                                                                                           \
												 BC_ARG_COUNT(__VA_ARGS__), BC_ARG_MAP($, __VA_ARGS__));                                                                           \
		BCAutoreleasePoolPop();                                                                                                                                                    \
		_name_;                                                                                                                                                                    \
	})

#define $SET(...) ___BCINTERNAL___SET_IMPL(BC_M_CAT(___temp_set_impl___, __COUNTER__), __VA_ARGS__)
#define $$SET(...) ((BCSetRef)BCAutorelease($OBJ $SET(__VA_ARGS__)))

#endif // BC_BCRUNTIME_H
