#ifndef BCRUNTIME_BCSTRING_H
#define BCRUNTIME_BCSTRING_H

#include "BCTypes.h"

BCString* BCStringCreate(const char* fmt, ...);
BCString* BCStringConst(const char* text);
size_t BCStringLength(BCString* str);
uint32_t BCStringHash(BCString* str);
const char* BCStringGetCString(const BCString* str);

#endif //BCRUNTIME_BCSTRING_H