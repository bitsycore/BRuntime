#ifndef BCRUNTIME_BCCLASS_H
#define BCRUNTIME_BCCLASS_H

#include "../BCTypes.h"

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

BCStringPooledRef BCClassName(BCClassId cid);

#endif //BCRUNTIME_BCCLASS_H