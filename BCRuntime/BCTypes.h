#ifndef BCRUNTIME_BCTYPES_H
#define BCRUNTIME_BCTYPES_H

#include <stdint.h>

// =========================================================
// MARK: Primitive Types
// =========================================================

typedef _Bool BC_bool;
#define BC_true ((BC_bool)(1))
#define BC_false ((BC_bool)(0))

// =========================================================
// MARK: Core Types
// =========================================================

typedef struct BCClass *BCClassRef;
typedef struct BCObject *BCObjectRef;
typedef struct BCAllocator *BCAllocatorRef;

#define BC_CLASS_ID_INVALID UINT16_MAX
typedef uint16_t BCClassId;
typedef BCClassId BCClassIdSize;

#define $OBJ (BCObjectRef)

// =========================================================
// MARK: Strings
// =========================================================

typedef struct BCString *BCStringRef;
typedef struct BCString *BCStringPooledRef;
typedef struct BCStringBuilder *BCStringBuilderRef;

// =========================================================
// MARK: Numbers
// =========================================================

typedef struct BCNumber *BCNumberRef;
typedef struct BCNumber *BCNumberInt8Ref;
typedef struct BCNumber *BCNumberInt16Ref;
typedef struct BCNumber *BCNumberInt32Ref;
typedef struct BCNumber *BCNumberInt64Ref;
typedef struct BCNumber *BCNumberUInt8Ref;
typedef struct BCNumber *BCNumberUInt16Ref;
typedef struct BCNumber *BCNumberUInt32Ref;
typedef struct BCNumber *BCNumberUInt64Ref;
typedef struct BCNumber *BCNumberFloatRef;
typedef struct BCNumber *BCNumberDoubleRef;
typedef struct BCNumber *BCBoolRef;

// =========================================================
// MARK: Containers
// =========================================================

typedef struct BCList *BCListRef;
typedef struct BCBytesArray *BCBytesArrayRef;
typedef struct BCMap *BCMapRef;
typedef struct BCMap *BCMutableMapRef;

// =========================================================
// MARK: Function Types
// =========================================================

typedef void (*BCDeallocFunc)(BCObjectRef obj);
typedef uint32_t (*BCHashFunc)(BCObjectRef obj);
typedef BC_bool (*BCEqualFunc)(BCObjectRef a, BCObjectRef b);
typedef BCStringRef (*BCToStringFunc)(BCObjectRef obj);
typedef BCObjectRef (*BCCopyFunc)(BCObjectRef);

#endif // BCRUNTIME_BCTYPES_H
