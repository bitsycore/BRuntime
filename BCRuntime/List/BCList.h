#ifndef BCRUNTIME_BCARRAY_H
#define BCRUNTIME_BCARRAY_H

#include "../BCObject.h"

extern const BCClassRef kBCListClassRef;

BCListRef BCListCreate(void);
BCListRef BCListCreateWithObjects(BC_bool retain, size_t count, ...);

void BCListAdd(BCListRef list, BCObjectRef obj);
BCObjectRef BCListGet(BCListRef list, size_t index);

#endif //BCRUNTIME_BCARRAY_H