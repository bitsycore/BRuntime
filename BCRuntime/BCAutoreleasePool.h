#ifndef BCRUNTIME_BCAUTORELEASEPOOL_H
#define BCRUNTIME_BCAUTORELEASEPOOL_H

#include <stdlib.h>
#include <stdio.h>
#include "BCObject.h"

void BCAutoreleasePoolPush(void);
void BCAutoreleasePoolPop(void);
BCObject* BCAutorelease(BCObject* obj);

#endif //BCRUNTIME_BCAUTORELEASEPOOL_H
