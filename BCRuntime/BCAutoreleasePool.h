#ifndef BCRUNTIME_BCAUTORELEASEPOOL_H
#define BCRUNTIME_BCAUTORELEASEPOOL_H

#include <stdlib.h>
#include <stdio.h>
#include "BCObject.h"

void BCPoolPush(void);
void BCPoolPop(void);
BCObject* BCAutorelease(BCObject* obj);

#endif //BCRUNTIME_BCAUTORELEASEPOOL_H
