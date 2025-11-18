#ifndef BCRUNTIME_BCDICTIONARY_H
#define BCRUNTIME_BCDICTIONARY_H

#include <stdlib.h>

#include "BCObject.h"

typedef struct BCDictionary BCDictionary;
typedef struct BCMutableDictionary BCMutableDictionary;

BCDictionary* BCDictionaryCreate();
BCMutableDictionary* BCMutableDictionaryCreate();
void BCDictionarySet(BCMutableDictionary * d, BCObject* key, BCObject* val);
BCObject* BCDictionaryGet(BCDictionary* d, BCObject* key);
BCArray* BCDictionaryKeys(BCDictionary* d);

#endif //BCRUNTIME_BCDICTIONARY_H