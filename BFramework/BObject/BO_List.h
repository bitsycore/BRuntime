#ifndef BOBJECT_LIST_H
#define BOBJECT_LIST_H

#include <stdio.h>

#include "BO_Object.h"

// =========================================================
// MARK: Class
// =========================================================

BF_ClassId BO_ListClassId(void);

// =========================================================
// MARK: Constructors
// =========================================================

BO_ListRef BO_ListCreate(void);
BO_ListRef BO_ListCreateWithObjects(BC_bool retain, size_t count, ...);

// =========================================================
// MARK: Methods
// =========================================================

void BO_ListAdd(BO_ListRef list, BO_ObjectRef obj);
BO_ObjectRef BO_ListGet(BO_ListRef list, size_t index);

size_t BO_ListCount(BO_ListRef list);
BC_bool BO_ListIsEmpty(BO_ListRef list);
void BO_ListClear(BO_ListRef list);

void BO_ListRemove(BO_ListRef list, BO_ObjectRef obj);
void BO_ListRemoveAt(BO_ListRef list, size_t index);

BC_bool BO_ListContains(BO_ListRef list, BO_ObjectRef obj);
size_t BO_ListIndexOf(BO_ListRef list, BO_ObjectRef obj);

BO_ObjectRef BO_ListFirst(BO_ListRef list);
BO_ObjectRef BO_ListLast(BO_ListRef list);

void BO_ListForEach(BO_ListRef list, void (*block)(BO_ObjectRef item, size_t index));
BO_ListRef BO_ListMap(BO_ListRef list, BO_ObjectRef (*transform)(BO_ObjectRef item, size_t index, void* ctx), void* ctx);
BO_ListRef BO_ListFilter(BO_ListRef list, BC_bool (*predicate)(BO_ObjectRef item, size_t index));

#endif //BOBJECT_LIST_H
