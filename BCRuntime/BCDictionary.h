#ifndef BCRUNTIME_BCDICTIONARY_H
#define BCRUNTIME_BCDICTIONARY_H

#include "BCObject.h"
#include "Utilities/BCMacro.h"

typedef struct BCDictionary* BCDictionaryRef;
typedef struct BCDictionary* BCMutableDictionaryRef;

BCDictionaryRef BCDictionaryCreate();
BCMutableDictionaryRef BCMutableDictionaryCreate();
BCDictionaryRef BCDictionaryCreateWithObjects(bool retain, size_t count, ...);
void BCDictionarySet(BCMutableDictionaryRef d, BCObjectRef key, BCObjectRef val);
BCObjectRef BCDictionaryGet(BCDictionaryRef d, BCObjectRef key);
BCArrayRef BCDictionaryKeys(BCDictionaryRef d);
BCArrayRef BCDictionaryValues(BCDictionaryRef d);

#endif //BCRUNTIME_BCDICTIONARY_H