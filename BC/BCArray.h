#ifndef BCRUNTIME_BCARRAY_H
#define BCRUNTIME_BCARRAY_H

#include "BCObject.h"

BCArrayRef BCArrayCreate(void);
void BCArrayAdd(BCArrayRef arr, BCObject* item);
BCObject* BCArrayGet(BCArrayRef arr, size_t idx);

#endif //BCRUNTIME_BCARRAY_H
