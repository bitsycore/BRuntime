#ifndef BCORE_MACRO_H
#define BCORE_MACRO_H

#define INTERNAL_BC_M_STR_IMPL(x) #x
#define BC_M_STR(x) INTERNAL_BC_M_STR_IMPL(x)

#define INTERNAL_BC_M_CAT_IMPL(a, b) a##b
#define BC_M_CAT(a, b)  INTERNAL_BC_M_CAT_IMPL(a, b)

#define INTERNAL_BC_MAP_0(F)
#define INTERNAL_BC_MAP_1(F, X)      F(X)
#define INTERNAL_BC_MAP_2(F, X, ...) F(X), INTERNAL_BC_MAP_1(F, __VA_ARGS__)
#define INTERNAL_BC_MAP_3(F, X, ...) F(X), INTERNAL_BC_MAP_2(F, __VA_ARGS__)
#define INTERNAL_BC_MAP_4(F, X, ...) F(X), INTERNAL_BC_MAP_3(F, __VA_ARGS__)
#define INTERNAL_BC_MAP_5(F, X, ...) F(X), INTERNAL_BC_MAP_4(F, __VA_ARGS__)
#define INTERNAL_BC_MAP_6(F, X, ...) F(X), INTERNAL_BC_MAP_5(F, __VA_ARGS__)
#define INTERNAL_BC_MAP_7(F, X, ...) F(X), INTERNAL_BC_MAP_6(F, __VA_ARGS__)
#define INTERNAL_BC_MAP_8(F, X, ...) F(X), INTERNAL_BC_MAP_7(F, __VA_ARGS__)
#define INTERNAL_BC_MAP_9(F, X, ...) F(X), INTERNAL_BC_MAP_8(F, __VA_ARGS__)
#define INTERNAL_BC_MAP_10(F, X, ...) F(X), INTERNAL_BC_MAP_9(F, __VA_ARGS__)
#define INTERNAL_BC_MAP_11(F, X, ...) F(X), INTERNAL_BC_MAP_10(F, __VA_ARGS__)
#define INTERNAL_BC_MAP_12(F, X, ...) F(X), INTERNAL_BC_MAP_11(F, __VA_ARGS__)
#define INTERNAL_BC_MAP_13(F, X, ...) F(X), INTERNAL_BC_MAP_12(F, __VA_ARGS__)
#define INTERNAL_BC_MAP_14(F, X, ...) F(X), INTERNAL_BC_MAP_13(F, __VA_ARGS__)
#define INTERNAL_BC_MAP_15(F, X, ...) F(X), INTERNAL_BC_MAP_14(F, __VA_ARGS__)
#define INTERNAL_BC_MAP_16(F, X, ...) F(X), INTERNAL_BC_MAP_15(F, __VA_ARGS__)
#define INTERNAL_BC_MAP_17(F, X, ...) F(X), INTERNAL_BC_MAP_16(F, __VA_ARGS__)
#define INTERNAL_BC_MAP_18(F, X, ...) F(X), INTERNAL_BC_MAP_17(F, __VA_ARGS__)
#define INTERNAL_BC_MAP_19(F, X, ...) F(X), INTERNAL_BC_MAP_18(F, __VA_ARGS__)
#define INTERNAL_BC_MAP_20(F, X, ...) F(X), INTERNAL_BC_MAP_19(F, __VA_ARGS__)

#define INTERNAL_BC_GET_MAP_NAME(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,N,...) INTERNAL_BC_MAP_##N

#define BC_ARG_MAP(F, ...) \
    __VA_OPT__( \
        INTERNAL_BC_GET_MAP_NAME(__VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)(F, __VA_ARGS__) \
    )

#define BC_ARG_COUNT(...) \
    INTERNAL_BC_ARG_COUNT_INNER(__VA_ARGS__ __VA_OPT__(,) 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define INTERNAL_BC_ARG_COUNT_INNER(...) INTERNAL_BC_ARG_COUNT_SELECT(__VA_ARGS__)
#define INTERNAL_BC_ARG_COUNT_SELECT(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,N, ...) N

#define INTERNAL_BC_ARG_FIRST_IMPL(first, ...) first
#define BC_ARG_FIRST(...) INTERNAL_BC_ARG_FIRST_IMPL(__VA_ARGS__, X)


#define BC_GB(_x_) (1024*1024*1024*(_x_))
#define BC_MB(_x_) (1024*1024*(_x_))
#define BC_KB(_x_) (1024*(_x_))

#ifdef __RESHARPER__
#define INTERNAL_BC_REQUIRE_LITERAL_ASSERT(x)
#else
#define INTERNAL_BC_REQUIRE_LITERAL_ASSERT(x) _Static_assert(__builtin_constant_p(x), "Must be a literal");
#endif

#define BC_REQUIRE_LITERAL(x) ({ \
	INTERNAL_BC_REQUIRE_LITERAL_ASSERT(x) \
	("" x ""); \
	(x); \
})

#define BC_FLAG_HAS(obj, flag) ((obj) & (flag))
#define BC_FLAG_SET(obj, flag) ((obj) |= (flag))
#define BC_FLAG_CLEAR(obj, flag) ((obj) &= ~(flag))
#define BC_FLAG_TOGGLE(obj, flag) ((obj) ^= (flag))

#endif //BCORE_MACRO_H