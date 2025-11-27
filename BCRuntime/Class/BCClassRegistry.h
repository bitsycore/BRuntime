#ifndef BCRUNTIME_BCCLASSREGISTRY_H
#define BCRUNTIME_BCCLASSREGISTRY_H

#include "../BCTypes.h"

BCClassId BCClassRegistryGetCount(void);
BCClassId BCClassRegister(BCClassRef cls);
BCClassRef BCClassIdToRef(BCClassId cid);
BCClassId BCClassRefToId(BCClassRef cls);

#endif // BCRUNTIME_BCCLASSREGISTRY_H
