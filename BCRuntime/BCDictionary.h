#ifndef BCRUNTIME_BCDICTIONARY_H
#define BCRUNTIME_BCDICTIONARY_H

#include <stdlib.h>

#include "BCObject.h"
#include "BCMacroTools.h"

typedef struct BCDictionary* BCDictionaryRef;
typedef struct BCDictionary* BCMutableDictionaryRef;

BCDictionaryRef BCDictionaryCreate();
BCMutableDictionaryRef BCMutableDictionaryCreate();
BCDictionaryRef ___BCDictionaryCreateWithObjects(size_t count, ...);

#define BCDictionaryCreateWithObjects(count, ...) ({ \
    _Static_assert(count % 2 == 0, "Dictionary requires an even number of arguments"); \
    _Static_assert(count == BC_ARG_COUNT(__VA_ARG__), "Arg Count mismatch"); \
    $VAR ___temp = ___BCDictionaryCreateWithObjects(count, __VA_ARGS__); \
    ___temp; \
})

BCDictionaryRef ___BCDictionaryCreateWithObjectsNoRetain(size_t count, ...);
void BCDictionarySet(BCMutableDictionaryRef d, BCObjectRef key, BCObjectRef val);
BCObjectRef BCDictionaryGet(BCDictionaryRef d, BCObjectRef key);
BCArrayRef BCDictionaryCopyKeys(BCDictionaryRef d);

#endif //BCRUNTIME_BCDICTIONARY_H