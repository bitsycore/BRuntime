#ifndef BCRUNTIME_BCAUTORELEASEPOOL_H
#define BCRUNTIME_BCAUTORELEASEPOOL_H

#include "BCObject.h"
#include "Utilities/BCMacro.h"

void BCAutoreleasePoolPush(void);
void BCAutoreleasePoolPop(void);
BCObjectRef BCAutorelease(BCObjectRef obj);

#define ___BCINTERNAL___AutoreleaseImpl(...) BC_ARG_MAP(BCAutorelease, __VA_ARGS__)
#define BCAutoreleaseAll(first, ...) ___BCINTERNAL___AutoreleaseImpl(first, __VA_ARGS__)

#define ___BCINTERNAL___AutoreleaseScopeImpl(__name__) for ( \
    BC_bool __name__ = (BCAutoreleasePoolPush(), BC_true); \
    __name__; \
    __name__ = BC_false, BCAutoreleasePoolPop() \
)
#define BCAutoreleaseScope() ___BCINTERNAL___AutoreleaseScopeImpl(BC_M_CAT(___temp_once_, __COUNTER__))

#endif //BCRUNTIME_BCAUTORELEASEPOOL_H
