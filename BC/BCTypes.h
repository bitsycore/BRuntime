#ifndef BCRUNTIME_BCTYPES_H
#define BCRUNTIME_BCTYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct BCObject BCObject;
typedef struct BCAllocator BCAllocator;
typedef struct BCString BCString;
typedef struct BCArray BCArray;

typedef void (* BCDeallocProc)(BCObject* obj);
typedef uint32_t (* BCHashProc)(const BCObject* obj);
typedef bool (* BCEqualProc)(const BCObject* a, const BCObject* b);
typedef void (* BCDescProc)(const BCObject* obj, int indent);
typedef struct BCObject* (* BCCopyProc)(const BCObject*);

#endif //BCRUNTIME_BCTYPES_H
