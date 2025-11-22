#ifndef BCRUNTIME_BCAUTORELEASEPOOL_H
#define BCRUNTIME_BCAUTORELEASEPOOL_H

#include "BCObject.h"

#include <stdlib.h>
#include <stdio.h>

void BCAutoreleasePoolPush(void);
void BCAutoreleasePoolPop(void);
BCObjectRef BCAutorelease(BCObjectRef obj);

#define ___BCINTERNAL___AutoreleaseScopeImpl(__name__) for ( \
    bool __name__ = (BCAutoreleasePoolPush(), true); \
    __name__; \
    __name__ = false, BCAutoreleasePoolPop() \
)
#define BCAutoreleaseScope() ___BCINTERNAL___AutoreleaseScopeImpl(BC_M_CAT(___temp_once_, __COUNTER__))

#endif //BCRUNTIME_BCAUTORELEASEPOOL_H
