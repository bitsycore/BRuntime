#ifndef BOBJECT_SET_H
#define BOBJECT_SET_H

#include "BCore/BC_Macro.h"

#include "BO_Object.h"

// =========================================================
// MARK: Class
// =========================================================

typedef struct BO_Set* BO_SetRef;
typedef struct BO_Set* BO_MutableSetRef;

BF_ClassId BO_SetClassId(void);

#define BO_SET_FLAG_MUTABLE (1 << 8)

// =========================================================
// MARK: Constructors
// =========================================================

BO_SetRef BO_SetCreate(void);
BO_MutableSetRef BCMutableSetCreate(void);
BO_SetRef BO_SetCreateWithObjects(BC_bool retain, size_t count, ...);

// =========================================================
// MARK: Methods
// =========================================================

void BO_SetAdd(BO_MutableSetRef set, BO_ObjectRef obj);
void BO_SetRemove(BO_MutableSetRef set, BO_ObjectRef obj);
BC_bool BO_SetContains(BO_SetRef set, BO_ObjectRef obj);
size_t BO_SetCount(BO_SetRef set);
BO_ListRef BO_SetToList(BO_SetRef set);
void BO_SetForEach(BO_SetRef set, void (*block)(BO_ObjectRef item));

#endif //BOBJECT_SET_H
