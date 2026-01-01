#ifndef BFRAMEWORK_AUTORELEASE_POOL_H
#define BFRAMEWORK_AUTORELEASE_POOL_H

#include "BFramework/BF_Types.h"

void BF_AutoreleasePoolPush(void);
void BF_AutoreleasePoolPop(void);
BO_ObjectRef BF_Autorelease(BO_ObjectRef obj);

#define ___BF_INTERNAL___AutoreleaseImpl(...) BC_ARG_MAP(BF_Autorelease, __VA_ARGS__)
#define BF_AutoreleaseAll(first, ...) ___BF_INTERNAL___AutoreleaseImpl(first, __VA_ARGS__)

#define ___BF_INTERNAL___AutoreleaseScopeImpl(__name__) for ( \
    BC_bool __name__ = (BF_AutoreleasePoolPush(), BC_true); \
    __name__; \
    __name__ = BC_false, BF_AutoreleasePoolPop() \
)
#define BF_AutoreleaseScope() ___BF_INTERNAL___AutoreleaseScopeImpl(BC_M_CAT(___temp_once_, __COUNTER__))

#endif //BFRAMEWORK_AUTORELEASE_POOL_H