#ifndef BC_BCMACROTOOLS_H
#define BC_BCMACROTOOLS_H

#define ___BC___MAP_0(F)
#define ___BC___MAP_1(F, X)      F(X)
#define ___BC___MAP_2(F, X, ...) F(X), ___BC___MAP_1(F, __VA_ARGS__)
#define ___BC___MAP_3(F, X, ...) F(X), ___BC___MAP_2(F, __VA_ARGS__)
#define ___BC___MAP_4(F, X, ...) F(X), ___BC___MAP_3(F, __VA_ARGS__)
#define ___BC___MAP_5(F, X, ...) F(X), ___BC___MAP_4(F, __VA_ARGS__)
#define ___BC___MAP_6(F, X, ...) F(X), ___BC___MAP_5(F, __VA_ARGS__)
#define ___BC___MAP_7(F, X, ...) F(X), ___BC___MAP_6(F, __VA_ARGS__)
#define ___BC___MAP_8(F, X, ...) F(X), ___BC___MAP_7(F, __VA_ARGS__)
#define ___BC___MAP_9(F, X, ...) F(X), ___BC___MAP_8(F, __VA_ARGS__)
#define ___BC___MAP_10(F, X, ...) F(X), ___BC___MAP_9(F, __VA_ARGS__)
#define ___BC___MAP_11(F, X, ...) F(X), ___BC___MAP_10(F, __VA_ARGS__)
#define ___BC___MAP_12(F, X, ...) F(X), ___BC___MAP_11(F, __VA_ARGS__)
#define ___BC___MAP_13(F, X, ...) F(X), ___BC___MAP_12(F, __VA_ARGS__)
#define ___BC___MAP_14(F, X, ...) F(X), ___BC___MAP_13(F, __VA_ARGS__)
#define ___BC___MAP_15(F, X, ...) F(X), ___BC___MAP_14(F, __VA_ARGS__)
#define ___BC___MAP_16(F, X, ...) F(X), ___BC___MAP_15(F, __VA_ARGS__)
#define ___BC___MAP_17(F, X, ...) F(X), ___BC___MAP_16(F, __VA_ARGS__)
#define ___BC___MAP_18(F, X, ...) F(X), ___BC___MAP_17(F, __VA_ARGS__)
#define ___BC___MAP_19(F, X, ...) F(X), ___BC___MAP_18(F, __VA_ARGS__)
#define ___BC___MAP_20(F, X, ...) F(X), ___BC___MAP_19(F, __VA_ARGS__)

#define ___BC___GET_MAP_NAME(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,N,...) ___BC___MAP_##N

#define MAP(F, ...) \
    __VA_OPT__( \
        ___BC___GET_MAP_NAME(__VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)(F, __VA_ARGS__) \
    )

#define ARG_COUNT(...) \
    ___BC___ARG_COUNT_INNER(__VA_ARGS__ __VA_OPT__(,) 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define ___BC___ARG_COUNT_INNER(...) ___BC___ARG_COUNT_SELECT(__VA_ARGS__)
#define ___BC___ARG_COUNT_SELECT(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,N, ...) N

#endif //BC_BCMACROTOOLS_H