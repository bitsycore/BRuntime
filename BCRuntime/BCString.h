#ifndef BCRUNTIME_BCSTRING_H
#define BCRUNTIME_BCSTRING_H

#include "BCTypes.h"

#include <stddef.h>

#define BC_HASH_UNSET 0
#define BC_LEN_UNSET SIZE_MAX

__attribute__((format(printf, 1, 2)))
BCStringRef BCStringCreate(const char* fmt, ...);
BCStringRef BCStringConst(const char* text);
size_t BCStringLength(BCStringRef str);
uint32_t BCStringHash(BCStringRef str);
const char* BCStringGetCString(BCStringRef str);

#endif //BCRUNTIME_BCSTRING_H