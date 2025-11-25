#include "BCClass.h"

#include "String/BCString.h"

BCStringPooledRef BCClassName(const BCClassRef cls) {
	return BCStringPooled(cls->name);
}