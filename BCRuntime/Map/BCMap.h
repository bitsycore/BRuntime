#ifndef BCRUNTIME_BCDICTIONARY_H
#define BCRUNTIME_BCDICTIONARY_H

#include "../BCObject.h"
#include "../Utilities/BCMacro.h"

// =========================================================
// MARK: Class
// =========================================================

typedef struct BCMap* BCMapRef;
typedef struct BCMap* BCMutableMapRef;

BCClassId BCMapClassId();

#define BC_MAP_FLAG_MUTABLE (1 << 8)

// =========================================================
// MARK: Constructors
// =========================================================

BCMapRef BCMapCreate();
BCMutableMapRef BCMutableMapCreate();
BCMapRef BCMapCreateWithObjects(BC_bool retain, size_t count, ...);

// =========================================================
// MARK: Methods
// =========================================================

void BCMapSet(BCMutableMapRef d, BCObjectRef key, BCObjectRef val);
BCObjectRef BCMapGet(BCMapRef d, BCObjectRef key);
BCListRef BCMapKeys(BCMapRef d);
BCListRef BCMapValues(BCMapRef d);

size_t BCMapCount(BCMapRef d);
BC_bool BCMapIsEmpty(BCMapRef d);
void BCMapClear(BCMutableMapRef d);

void BCMapRemove(BCMutableMapRef d, BCObjectRef key);

BC_bool BCMapContainsKey(BCMapRef d, BCObjectRef key);
BC_bool BCMapContainsValue(BCMapRef d, BCObjectRef val);

BCObjectRef BCMapGetOrDefault(BCMapRef d, BCObjectRef key, BCObjectRef defaultValue);

void BCMapForEach(BCMapRef d, void (*block)(BCObjectRef key, BCObjectRef value));

#endif // BCRUNTIME_BCDICTIONARY_H
