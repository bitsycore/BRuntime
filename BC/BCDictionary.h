#ifndef BCRUNTIME_BCDICTIONARY_H
#define BCRUNTIME_BCDICTIONARY_H

#include <stdlib.h>

#include "BCObject.h"

typedef struct BCDictionary* BCDictionaryRef;
typedef struct BCDictionary* BCMutableDictionaryRef;

BCDictionaryRef BCDictionaryCreate();
BCMutableDictionaryRef BCMutableDictionaryCreate();
void BCDictionarySet(BCMutableDictionaryRef d, BCObjectRef key, BCObjectRef val);
BCObjectRef BCDictionaryGet(BCDictionaryRef d, BCObjectRef key);
BCArrayRef BCDictionaryKeys(BCDictionaryRef d);

#endif //BCRUNTIME_BCDICTIONARY_H