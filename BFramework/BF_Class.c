#include "BF_Class.h"

#include "BObject/BO_String.h"

BO_StringPooledRef BF_ClassIdName(const BF_ClassId cid) {
	return BO_StringPooled(BF_ClassIdGetRef(cid)->name);
}