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

#endif //BCRUNTIME_BCDICTIONARY_H