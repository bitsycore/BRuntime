#ifndef BFRAMEWORK_AUTORELEASE_POOL_H
#define BFRAMEWORK_AUTORELEASE_POOL_H

#include "BFramework/BF_Types.h"

void BFAutoreleasePoolPush(void);
void BFAutoreleasePoolPop(void);
BO_ObjectRef BFAutorelease(BO_ObjectRef obj);

#define ___BF_INTERNAL___AutoreleaseImpl(...) BC_ARG_MAP(BFAutorelease, __VA_ARGS__)
#define BFAutoreleaseAll(first, ...) ___BF_INTERNAL___AutoreleaseImpl(first, __VA_ARGS__)

#define ___BF_INTERNAL___AutoreleaseScopeImpl(__name__) for ( \
    BC_bool __name__ = (BFAutoreleasePoolPush(), BC_true); \
    __name__; \
    __name__ = BC_false, BFAutoreleasePoolPop() \
)
#define BF_AutoreleaseScope() ___BF_INTERNAL___AutoreleaseScopeImpl(BC_M_CAT(___temp_once_, __COUNTER__))

#endif //BFRAMEWORK_AUTORELEASE_POOL_H