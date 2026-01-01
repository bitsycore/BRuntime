#ifndef BFRAMEWORK_TYPES_H
#define BFRAMEWORK_TYPES_H

#include "BCore/BC_Types.h"

#include <stdbool.h>
#include <stdint.h>

#define $VAR __auto_type
#define $LET const __auto_type

// =========================================================
// MARK: Core Types
// =========================================================

typedef struct BO_Object* BO_ObjectRef;

#define BF_CLASS_ID_INVALID UINT16_MAX
typedef uint16_t BF_ClassId;

#define $OBJ (BO_ObjectRef)

// =========================================================
// MARK: Strings
// =========================================================

typedef struct BO_String* BO_StringRef;
typedef struct BO_String* BO_StringPooledRef;
typedef struct BO_StringBuilder* BO_StringBuilderRef;

// =========================================================
// MARK: Numbers
// =========================================================

typedef struct BO_Number* BO_NumberRef;
typedef struct BO_Number* BO_NumberInt8Ref;
typedef struct BO_Number* BO_NumberInt16Ref;
typedef struct BO_Number* BO_NumberInt32Ref;
typedef struct BO_Number* BO_NumberInt64Ref;
typedef struct BO_Number* BO_NumberUInt8Ref;
typedef struct BO_Number* BO_NumberUInt16Ref;
typedef struct BO_Number* BO_NumberUInt32Ref;
typedef struct BO_Number* BO_NumberUInt64Ref;
typedef struct BO_Number* BO_NumberFloatRef;
typedef struct BO_Number* BO_NumberDoubleRef;
typedef struct BO_Number* BO_BoolRef;

// =========================================================
// MARK: Containers
// =========================================================

typedef struct BO_List* BO_ListRef;
typedef struct BO_BytesArray* BO_BytesArrayRef;
typedef struct BO_Map* BO_MapRef;
typedef struct BO_Map* BO_MutableMapRef;

// =========================================================
// MARK: Function Types
// =========================================================

typedef void (*BF_DeallocFunc)(BO_ObjectRef obj);
typedef uint32_t (*BF_HashFunc)(BO_ObjectRef obj);
typedef BC_bool (*BF_EqualFunc)(BO_ObjectRef a, BO_ObjectRef b);
typedef BO_StringRef (*BF_ToStringFunc)(BO_ObjectRef obj);
typedef BO_ObjectRef (*BF_CopyFunc)(BO_ObjectRef);

#endif //BFRAMEWORK_TYPES_H
