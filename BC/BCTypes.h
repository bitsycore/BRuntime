#ifndef BCRUNTIME_BCTYPES_H
#define BCRUNTIME_BCTYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct BCObject* BCObjectRef;
typedef struct BCAllocator* BCAllocatorRef;
typedef struct BCString* BCStringRef;
typedef struct BCArray* BCArrayRef;
typedef struct BCClass* BCClassRef;

typedef void (* BCDeallocFunc)(BCObjectRef obj);
typedef uint32_t (* BCHashFunc)(BCObjectRef obj);
typedef bool (* BCEqualFunc)(BCObjectRef a, BCObjectRef b);
typedef void (* BCDescFunc)(BCObjectRef obj, int indent);
typedef struct BCObject* (* BCCopyFunc)(BCObjectRef);

#endif //BCRUNTIME_BCTYPES_H
