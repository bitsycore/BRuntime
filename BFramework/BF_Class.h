#ifndef BFRAMEWORK_CLASS_H
#define BFRAMEWORK_CLASS_H

#include "BF_Types.h"

#include <stddef.h>

typedef struct BF_Class {
	const char* name;
	BF_ClassId id;
	BFDeallocFunc dealloc;
	BO_HashFunc hash;
	BO_EqualFunc equal;
	BFToStringFunc toString;
	BFCopyFunc copy;
	size_t allocSize;
} BF_Class;

// =========================================================
// MARK: Class Id
// =========================================================

BO_StringPooledRef BF_ClassIdName(BF_ClassId cid);
BF_Class* BF_ClassIdGetRef(BF_ClassId cid);

// =========================================================
// MARK: Class Ref
// =========================================================

BF_ClassId BF_DebugClassFindId(const BF_Class* cls);

// =========================================================
// MARK: Registry
// =========================================================

BF_ClassId BF_ClassRegistryGetCount(void);
BF_ClassId BF_ClassRegistryInsert(BF_Class* cls);

#endif //BFRAMEWORK_CLASS_H
