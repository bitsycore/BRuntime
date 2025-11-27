#ifndef BCRUNTIME_BCSET_H
#define BCRUNTIME_BCSET_H

#include "BCObject.h"
#include "../Utilities/BC_Macro.h"

// =========================================================
// MARK: Class
// =========================================================

typedef struct BCSet* BCSetRef;
typedef struct BCSet* BCMutableSetRef;

BCClassId BCSetClassId(void);

#define BC_SET_FLAG_MUTABLE (1 << 8)

// =========================================================
// MARK: Constructors
// =========================================================

BCSetRef BCSetCreate(void);
BCMutableSetRef BCMutableSetCreate(void);
BCSetRef BCSetCreateWithObjects(BC_bool retain, size_t count, ...);

// =========================================================
// MARK: Methods
// =========================================================

void BCSetAdd(BCMutableSetRef set, BCObjectRef obj);
void BCSetRemove(BCMutableSetRef set, BCObjectRef obj);
BC_bool BCSetContains(BCSetRef set, BCObjectRef obj);
size_t BCSetCount(BCSetRef set);
BCListRef BCSetToList(BCSetRef set);
void BCSetForEach(BCSetRef set, void (*block)(BCObjectRef item));

#endif //BCRUNTIME_BCSET_H
