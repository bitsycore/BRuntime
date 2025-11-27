#ifndef BCRUNTIME_BCARRAY_H
#define BCRUNTIME_BCARRAY_H

#include <stdio.h>

#include "BCObject.h"

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

size_t BCListCount(BCListRef list);
BC_bool BCListIsEmpty(BCListRef list);
void BCListClear(BCListRef list);

void BCListRemove(BCListRef list, BCObjectRef obj);
void BCListRemoveAt(BCListRef list, size_t index);

BC_bool BCListContains(BCListRef list, BCObjectRef obj);
size_t BCListIndexOf(BCListRef list, BCObjectRef obj);

BCObjectRef BCListFirst(BCListRef list);
BCObjectRef BCListLast(BCListRef list);

void BCListForEach(BCListRef list, void (*block)(BCObjectRef item, size_t index));
BCListRef BCListMap(BCListRef list, BCObjectRef (*transform)(BCObjectRef item, size_t index, void* ctx), void* ctx);
BCListRef BCListFilter(BCListRef list, BC_bool (*predicate)(BCObjectRef item, size_t index));

#endif // BCRUNTIME_BCARRAY_H
