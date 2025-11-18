#ifndef BCRUNTIME_BCARRAY_H
#define BCRUNTIME_BCARRAY_H

#include "BCObject.h"

BCArray* BCArrayCreate(void);
void BCArrayAdd(BCArray* arr, BCObject* item);
BCObject* BCArrayGet(BCArray* arr, size_t idx);

#endif //BCRUNTIME_BCARRAY_H
