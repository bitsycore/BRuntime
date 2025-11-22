#ifndef BCRUNTIME_BCTYPES_H
#define BCRUNTIME_BCTYPES_H

#include <stdbool.h>
#include <stdint.h>

// =========================================================
// MARK: Core
// =========================================================

typedef struct BCClass* BCClassRef;
typedef struct BCObject* BCObjectRef;
typedef struct BCAllocator* BCAllocatorRef;

#define $OBJ (BCObjectRef)

// =========================================================
// MARK: Basic Types
// =========================================================

typedef struct BCString* BCStringRef;

// =========================================================
// MARK: Numbers
// =========================================================

typedef struct BCNumber* BCNumberRef;
typedef struct BCNumber* BCNumberInt8Ref;
typedef struct BCNumber* BCNumberInt16Ref;
typedef struct BCNumber* BCNumberInt32Ref;
typedef struct BCNumber* BCNumberInt64Ref;
typedef struct BCNumber* BCNumberUInt8Ref;
typedef struct BCNumber* BCNumberUInt16Ref;
typedef struct BCNumber* BCNumberUInt32Ref;
typedef struct BCNumber* BCNumberUInt64Ref;
typedef struct BCNumber* BCNumberFloatRef;
typedef struct BCNumber* BCNumberDoubleRef;
typedef struct BCNumber* BCBoolRef;

// =========================================================
// MARK: Containers
// =========================================================

typedef struct BCArray* BCArrayRef;
typedef struct BCDictionary* BCDictionaryRef;
typedef struct BCDictionary* BCMutableDictionaryRef;

// =========================================================
// MARK: Function Types
// =========================================================

typedef void (* BCDeallocFunc)(BCObjectRef obj);
typedef uint32_t (* BCHashFunc)(BCObjectRef obj);
typedef bool (* BCEqualFunc)(BCObjectRef a, BCObjectRef b);
typedef BCStringRef (* BCDescriptionFunc)(BCObjectRef obj);
typedef BCObjectRef (* BCCopyFunc)(BCObjectRef);

#endif //BCRUNTIME_BCTYPES_H
