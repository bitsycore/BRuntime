#ifndef BCRUNTIME_BCDICTIONARY_H
#define BCRUNTIME_BCDICTIONARY_H

#include "../BCObject.h"
#include "../Utilities/BCMacro.h"

typedef struct BCMap* BCMapRef;
typedef struct BCMap* BCMutableMapRef;

extern const BCClassRef kBCDictClassRef;

BCMapRef BCMapCreate();
BCMutableMapRef BCMutableMapCreate();
BCMapRef BCMapCreateWithObjects(BC_bool retain, size_t count, ...);
void BCMapSet(BCMutableMapRef d, BCObjectRef key, BCObjectRef val);
BCObjectRef BCMapGet(BCMapRef d, BCObjectRef key);
BCListRef BCMapKeys(BCMapRef d);
BCListRef BCMapValues(BCMapRef d);

#endif //BCRUNTIME_BCDICTIONARY_H