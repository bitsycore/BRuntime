#include "BCClass.h"

#include "String/BCString.h"

BCStringPooledRef BCClassName(const BCClassId cid) {
	return BCStringPooled(BCClassIdToRef(cid)->name);
}