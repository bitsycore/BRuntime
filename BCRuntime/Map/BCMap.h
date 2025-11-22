#ifndef BCRUNTIME_BCDICTIONARY_H
#define BCRUNTIME_BCDICTIONARY_H

#include "../BCObject.h"
#include "../Utilities/BCMacro.h"

typedef struct BCMap* BCMapRef;
typedef struct BCMap* BCMutableMapRef;

BCMapRef BCMapCreate();
BCMutableMapRef BCMutableMapCreate();
BCMapRef BCMapCreateWithObjects(bool retain, size_t count, ...);
void BCMapSet(BCMutableMapRef d, BCObjectRef key, BCObjectRef val);
BCObjectRef BCMapGet(BCMapRef d, BCObjectRef key);
BCArrayRef BCMapKeys(BCMapRef d);
BCArrayRef BCMapValues(BCMapRef d);

#endif //BCRUNTIME_BCDICTIONARY_H