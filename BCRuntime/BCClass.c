#include "BCClass.h"

#include "BCString.h"

BCStringPooledRef BCClassName(const BCClassRef cls) {
	return BCStringPooled(cls->name);
}