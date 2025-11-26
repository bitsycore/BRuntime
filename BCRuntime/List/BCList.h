#ifndef BCRUNTIME_BCARRAY_H
#define BCRUNTIME_BCARRAY_H

#include "../BCObject.h"

// =========================================================
// MARK: Class
// =========================================================

BCClassId BCListClassId(void);

// =========================================================
// MARK: Constructors
// =========================================================

BCListRef BCListCreate(void);
BCListRef BCListCreateWithObjects(BC_bool retain, size_t count, ...);

// =========================================================
// MARK: Methods
// =========================================================

void BCListAdd(BCListRef list, BCObjectRef obj);
BCObjectRef BCListGet(BCListRef list, size_t index);

#endif //BCRUNTIME_BCARRAY_H