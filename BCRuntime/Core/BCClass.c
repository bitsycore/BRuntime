#include "BCClass.h"

#include "../Object/BCString.h"

BCStringPooledRef BCClassIdName(const BCClassId cid) {
	return BCStringPooled(BCClassIdGetRef(cid)->name);
}