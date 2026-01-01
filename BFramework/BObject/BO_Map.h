#ifndef BOBJECT_MAP_H
#define BOBJECT_MAP_H

#include "BO_Object.h"

// =========================================================
// MARK: Class
// =========================================================

typedef struct BO_Map* BO_MapRef;
typedef struct BO_Map* BO_MutableMapRef;

BF_ClassId BO_MapClassId();

#define BO_MAP_FLAG_MUTABLE (1 << 8)

// =========================================================
// MARK: Constructors
// =========================================================

BO_MapRef BO_MapCreate();
BO_MutableMapRef BO_MutableMapCreate();
BO_MapRef BO_MapCreateWithObjects(BC_bool retain, size_t count, ...);

// =========================================================
// MARK: Methods
// =========================================================

void BO_MapSet(BO_MutableMapRef d, BO_ObjectRef key, BO_ObjectRef val);
BO_ObjectRef BO_MapGet(BO_MapRef d, BO_ObjectRef key);
BO_ListRef BO_MapKeys(BO_MapRef d);
BO_ListRef BO_MapValues(BO_MapRef d);

size_t BO_MapCount(BO_MapRef d);
BC_bool BO_MapIsEmpty(BO_MapRef d);
void BO_MapClear(BO_MutableMapRef d);

void BO_MapRemove(BO_MutableMapRef d, BO_ObjectRef key);

BC_bool BO_MapContainsKey(BO_MapRef d, BO_ObjectRef key);
BC_bool BO_MapContainsValue(BO_MapRef d, BO_ObjectRef val);

BO_ObjectRef BO_MapGetOrDefault(BO_MapRef d, BO_ObjectRef key, BO_ObjectRef defaultValue);

void BO_MapForEach(BO_MapRef d, void (*block)(BO_ObjectRef key, BO_ObjectRef value));

#endif //BOBJECT_MAP_H
