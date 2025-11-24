#ifndef BCRUNTIME_BCARRAY_H
#define BCRUNTIME_BCARRAY_H

#include "../BCObject.h"

BCVectorRef BCVectorCreate(void);
BCVectorRef BCVectorCreateWithObjects(bool retain, size_t count, ...);
void BCVectorAdd(BCVectorRef arr, BCObjectRef item);
BCObjectRef BCVectorGet(BCVectorRef arr, size_t idx);

#endif //BCRUNTIME_BCARRAY_H