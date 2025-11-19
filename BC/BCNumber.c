#include "BCNumber.h"
#include "BCObject.h"
#include "BCString.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// =============================================================================
// MARK: Struct
// =============================================================================

#define DEFINE_NUMBER_STRUCT(Type, Name) \
    typedef struct BCNumber##Name { \
        BCObject header; \
        Type value; \
    } BCNumber##Name;

DEFINE_NUMBER_STRUCT(int8_t, Int8)
DEFINE_NUMBER_STRUCT(int16_t, Int16)
DEFINE_NUMBER_STRUCT(int32_t, Int32)
DEFINE_NUMBER_STRUCT(int64_t, Int64)
DEFINE_NUMBER_STRUCT(uint8_t, UInt8)
DEFINE_NUMBER_STRUCT(uint16_t, UInt16)
DEFINE_NUMBER_STRUCT(uint32_t, UInt32)
DEFINE_NUMBER_STRUCT(uint64_t, UInt64)
DEFINE_NUMBER_STRUCT(float, Float)
DEFINE_NUMBER_STRUCT(double, Double)

// =============================================================================
// MARK: Forward
// =============================================================================

#define DECLARE_CLASS(Name) \
    extern BCClass kBCNumber##Name##Class;

DECLARE_CLASS(Int8)
DECLARE_CLASS(Int16)
DECLARE_CLASS(Int32)
DECLARE_CLASS(Int64)
DECLARE_CLASS(UInt8)
DECLARE_CLASS(UInt16)
DECLARE_CLASS(UInt32)
DECLARE_CLASS(UInt64)
DECLARE_CLASS(Float)
DECLARE_CLASS(Double)

// =============================================================================
// Helpers
// =============================================================================

static BCNumberType GetTypeID(BCObjectRef obj) {
    if (!obj) return 0;
    if (obj->cls == &kBCNumberInt32Class) return BCNumberTypeInt32;
    else if (obj->cls == &kBCNumberInt8Class) return BCNumberTypeInt8;
	else if (obj->cls == &kBCNumberInt16Class) return BCNumberTypeInt16;
	else if (obj->cls == &kBCNumberInt64Class) return BCNumberTypeInt64;
    else if (obj->cls == &kBCNumberUInt8Class) return BCNumberTypeUInt8;
    else if (obj->cls == &kBCNumberUInt16Class) return BCNumberTypeUInt16;
    else if (obj->cls == &kBCNumberUInt32Class) return BCNumberTypeUInt32;
    else if (obj->cls == &kBCNumberUInt64Class) return BCNumberTypeUInt64;
    else if (obj->cls == &kBCNumberFloatClass) return BCNumberTypeFloat;
    else if (obj->cls == &kBCNumberDoubleClass) return BCNumberTypeDouble;
	else return 0;
}

BCNumberType BCNumberGetTypeID(BCNumberRef num) {
    return GetTypeID((BCObjectRef)num);
}

// =============================================================================
// Creation
// =============================================================================

#define IMPLEMENT_CREATE(Type, Name, ClassName) \
    BCNumberRef BCNumberCreate##Name(Type value) { \
        BCNumber##Name* obj = (BCNumber##Name*)BCAllocRaw(&kBCNumber##ClassName##Class, kBCDefaultAllocator, sizeof(BCNumber##Name) - sizeof(BCObject)); \
        if (obj) { \
            obj->value = value; \
        } \
        return (BCNumberRef)obj; \
    }

IMPLEMENT_CREATE(int8_t, Int8, Int8)
IMPLEMENT_CREATE(int16_t, Int16, Int16)
IMPLEMENT_CREATE(int32_t, Int32, Int32)
IMPLEMENT_CREATE(int64_t, Int64, Int64)
IMPLEMENT_CREATE(uint8_t, UInt8, UInt8)
IMPLEMENT_CREATE(uint16_t, UInt16, UInt16)
IMPLEMENT_CREATE(uint32_t, UInt32, UInt32)
IMPLEMENT_CREATE(uint64_t, UInt64, UInt64)
IMPLEMENT_CREATE(float, Float, Float)
IMPLEMENT_CREATE(double, Double, Double)

// =============================================================================
// GetValue
// =============================================================================

void BCNumberGetValueExplicit(BCNumberRef num, void* value, BCNumberType type) {
    if (!num || !value) return;

    BCNumberType srcType = GetTypeID((BCObjectRef)num);
    if (srcType == 0) return;

    // Read value as double (intermediate) to simplify conversion?
    // Or use a massive switch? Massive switch is safer for precision.

    // First, get the value from the object into a union or largest type.
    // Let's use double for floating point and int64/uint64 for integers.
    // Actually, let's just read into a local variable of the correct source type, then cast.

    #define CONVERT_AND_STORE(SrcType, SrcName, DstType) \
        { \
            SrcType srcVal = ((BCNumber##SrcName*)num)->value; \
            *(DstType*)value = (DstType)srcVal; \
        }

    #define DISPATCH_DEST(SrcType, SrcName) \
        switch (type) { \
            case BCNumberTypeInt8: CONVERT_AND_STORE(SrcType, SrcName, int8_t); break; \
            case BCNumberTypeInt16: CONVERT_AND_STORE(SrcType, SrcName, int16_t); break; \
            case BCNumberTypeInt32: CONVERT_AND_STORE(SrcType, SrcName, int32_t); break; \
            case BCNumberTypeInt64: CONVERT_AND_STORE(SrcType, SrcName, int64_t); break; \
            case BCNumberTypeUInt8: CONVERT_AND_STORE(SrcType, SrcName, uint8_t); break; \
            case BCNumberTypeUInt16: CONVERT_AND_STORE(SrcType, SrcName, uint16_t); break; \
            case BCNumberTypeUInt32: CONVERT_AND_STORE(SrcType, SrcName, uint32_t); break; \
            case BCNumberTypeUInt64: CONVERT_AND_STORE(SrcType, SrcName, uint64_t); break; \
            case BCNumberTypeFloat: CONVERT_AND_STORE(SrcType, SrcName, float); break; \
            case BCNumberTypeDouble: CONVERT_AND_STORE(SrcType, SrcName, double); break; \
        }

    switch (srcType) {
        case BCNumberTypeInt8: DISPATCH_DEST(int8_t, Int8); break;
        case BCNumberTypeInt16: DISPATCH_DEST(int16_t, Int16); break;
        case BCNumberTypeInt32: DISPATCH_DEST(int32_t, Int32); break;
        case BCNumberTypeInt64: DISPATCH_DEST(int64_t, Int64); break;
        case BCNumberTypeUInt8: DISPATCH_DEST(uint8_t, UInt8); break;
        case BCNumberTypeUInt16: DISPATCH_DEST(uint16_t, UInt16); break;
        case BCNumberTypeUInt32: DISPATCH_DEST(uint32_t, UInt32); break;
        case BCNumberTypeUInt64: DISPATCH_DEST(uint64_t, UInt64); break;
        case BCNumberTypeFloat: DISPATCH_DEST(float, Float); break;
        case BCNumberTypeDouble: DISPATCH_DEST(double, Double); break;
    }
}

// =============================================================================
// MARK: Class
// =============================================================================

static uint32_t NumberHash(BCObjectRef obj) {
    BCNumberType type = GetTypeID(obj);
    uint64_t v = 0;
    BCNumberGetValueExplicit((BCNumberRef)obj, &v, BCNumberTypeUInt64);
    return (uint32_t)v;
}

static bool NumberEqual(BCObjectRef a, BCObjectRef b) {
    if (a == b) return true;
    BCNumberType typeA = GetTypeID(a);
    BCNumberType typeB = GetTypeID(b);
    if (typeB == 0) return false;

    // Compare as double if either is float/double
    bool isFloatA = (typeA == BCNumberTypeFloat || typeA == BCNumberTypeDouble);
    bool isFloatB = (typeB == BCNumberTypeFloat || typeB == BCNumberTypeDouble);

    if (isFloatA || isFloatB) {
        double valA, valB;
        BCNumberGetValueExplicit((BCNumberRef)a, &valA, BCNumberTypeDouble);
        BCNumberGetValueExplicit((BCNumberRef)b, &valB, BCNumberTypeDouble);
        return valA == valB; // Exact match for now
    } else {
        // Both integers. Compare as Int64?
        // Careful with UInt64 large values vs Int64 negative values.
        // If one is signed and one is unsigned...
        // For simplicity, cast both to Int64 for now, or check signs.
        // Let's use Int64 for now.
        int64_t valA, valB;
        BCNumberGetValueExplicit((BCNumberRef)a, &valA, BCNumberTypeInt64);
        BCNumberGetValueExplicit((BCNumberRef)b, &valB, BCNumberTypeInt64);
        return valA == valB;
    }
}

static void NumberDesc(BCObjectRef obj, int indent) {
    BCNumberType type = GetTypeID(obj);

    // Indent
    for (int i = 0; i < indent; i++) printf("\t");

    switch (type) {
        case BCNumberTypeInt8: printf("Int8: %d\n", ((BCNumberInt8*)obj)->value); break;
        case BCNumberTypeInt16: printf("Int16: %d\n", ((BCNumberInt16*)obj)->value); break;
        case BCNumberTypeInt32: printf("Int32: %d\n", ((BCNumberInt32*)obj)->value); break;
        case BCNumberTypeInt64: printf("Int64: %ld\n", ((BCNumberInt64*)obj)->value); break;
        case BCNumberTypeUInt8: printf("UInt8: %u\n", ((BCNumberUInt8*)obj)->value); break;
        case BCNumberTypeUInt16: printf("UInt16: %u\n", ((BCNumberUInt16*)obj)->value); break;
        case BCNumberTypeUInt32: printf("UInt32: %u\n", ((BCNumberUInt32*)obj)->value); break;
        case BCNumberTypeUInt64: printf("UInt64: %lu\n", ((BCNumberUInt64*)obj)->value); break;
        case BCNumberTypeFloat: printf("Float: %f\n", ((BCNumberFloat*)obj)->value); break;
        case BCNumberTypeDouble: printf("Double: %lf\n", ((BCNumberDouble*)obj)->value); break;
        default: printf("Unknown Number\n"); break;
    }
}

static BCObjectRef NumberCopy(BCObjectRef obj) { return BCRetain(obj); }

#define DEFINE_CLASS(Name, StrName) \
    BCClass kBCNumber##Name##Class = { \
        .name = "BCNumber" #StrName, \
        .dealloc = NULL, \
        .hash = NumberHash, \
        .equal = NumberEqual, \
        .description = NumberDesc, \
        .copy = NumberCopy \
    };

DEFINE_CLASS(Int8, Int8)
DEFINE_CLASS(Int16, Int16)
DEFINE_CLASS(Int32, Int32)
DEFINE_CLASS(Int64, Int64)
DEFINE_CLASS(UInt8, UInt8)
DEFINE_CLASS(UInt16, UInt16)
DEFINE_CLASS(UInt32, UInt32)
DEFINE_CLASS(UInt64, UInt64)
DEFINE_CLASS(Float, Float)
DEFINE_CLASS(Double, Double)
