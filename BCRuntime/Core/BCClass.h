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

// =========================================================
// MARK: Class Id
// =========================================================

BCStringPooledRef BCClassIdName(BCClassId cid);
BCClassRef BCClassIdGetRef(BCClassId cid);

// =========================================================
// MARK: Class Ref
// =========================================================

BCClassId BCDebugClassFindId(BCClassRef cls);

// =========================================================
// MARK: Registry
// =========================================================

BCClassId BCClassRegistryGetCount(void);
BCClassId BCClassRegistryInsert(BCClassRef cls);

#endif //BCRUNTIME_BCCLASS_H
