#include "BCClass.h"

#include "BCClassRegistry.h"
#include "../String/BCString.h"

BCStringPooledRef BCClassName(const BCClassId cid) {
	return BCStringPooled(BCClassIdToRef(cid)->name);
}