#ifndef BCRUNTIME_BCCLASS_H
#define BCRUNTIME_BCCLASS_H

#include "BCTypes.h"

#include <stddef.h>

typedef struct BCClass {
	const char* name;
	BCClassId id;
	BCDeallocFunc dealloc;
	BCHashFunc hash;
	BCEqualFunc equal;
	BCToStringFunc toString;
	BCCopyFunc copy;
	size_t allocSize;
} BCClass;

BCClassId BCClassRegister(BCClassRef cls);
BCStringPooledRef BCClassName(BCClassId cid);

BCClassRef BCClassIdToRef(BCClassId cid);
BCClassId BCClassRefToId(BCClassRef cls);

#endif //BCRUNTIME_BCCLASS_H