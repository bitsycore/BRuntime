#ifndef BCRUNTIME_BCTYPES_H
#define BCRUNTIME_BCTYPES_H

#include <stdint.h>

typedef struct BCObject* BCObjectRef;
typedef struct BCAllocator* BCAllocatorRef;
typedef struct BCString* BCStringRef;
typedef struct BCArray* BCArrayRef;
typedef struct BCClass* BCClassRef;
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

typedef void (* BCDeallocFunc)(BCObjectRef obj);
typedef uint32_t (* BCHashFunc)(BCObjectRef obj);
typedef bool (* BCEqualFunc)(BCObjectRef a, BCObjectRef b);
typedef void (* BCDescriptionFunc)(BCObjectRef obj, int indent);
typedef BCObjectRef (* BCCopyFunc)(BCObjectRef);

#endif //BCRUNTIME_BCTYPES_H
