#include "BCNumber.h"
#include "BCObject.h"

#include <stdio.h>

#define BCNumberTypeError (-1)

// =============================================================================
// MARK: Struct
// =============================================================================

typedef struct BCNumber {;
	BCObject super;
} BCNumber;

#define DEFINE_NUMBER_STRUCT(Type, Name) \
    typedef struct BCNumber##Name { \
        BCNumber super;    \
        Type value;         \
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

typedef struct BCBool {
	BCNumber super;
	bool value;
} BCBool;

// =============================================================================
// MARK: Forward
// =============================================================================

static bool isNumber(BCClassRef cls);
static BCClassRef typeToClass(BCNumberType type);
static BCNumberType classToType(BCClassRef cls);

#define DECLARE_CLASS(Name) extern BCClassRef kBCNumber##Name##ClassRef;

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
DECLARE_CLASS(Bool)

// =============================================================================
// MARK: Create
// =============================================================================

#define IMPLEMENT_CREATE(Type, Name, ClassName) \
    BCNumberRef BCNumberCreate##Name(Type value) { \
        BCNumber##Name* obj = (BCNumber##Name*)BCAllocRaw(kBCNumber##ClassName##ClassRef, kBCDefaultAllocator, sizeof(BCNumber##Name) - sizeof(BCObject)); \
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

// Bool is a special case, it is a singleton object that is always allowed.

static BCBool kBCNumberBoolTrue = (BCBool) {
	.super = {
		.super = {
			.cls = NULL, .allocator = NULL, .ref_count = -1
		}
	},
	.value = true
};

static BCBool kBCNumberBoolFalse = (BCBool) {
	.super = {
		.super = {
			.cls = NULL, .allocator = NULL, .ref_count = -1
		}
	},
	.value = false
};

static bool kBcNumberBoolInitialized = false;

BCBoolRef BCNumberGetBool(bool value) {
	BCNumberRef val = value ? (BCNumberRef)&kBCNumberBoolTrue : (BCNumberRef)&kBCNumberBoolFalse;
	if (!kBcNumberBoolInitialized) {
		kBCNumberBoolTrue.super.super.cls = kBCNumberBoolClassRef;
		atomic_init(&kBCNumberBoolTrue.super.super.ref_count, -1);
		kBCNumberBoolFalse.super.super.cls = kBCNumberBoolClassRef;
		atomic_init(&kBCNumberBoolFalse.super.super.ref_count, -1);
		kBcNumberBoolInitialized = true;
	}
	return (BCBoolRef)val;
}

// =============================================================================
// MARK: Class Methods
// =============================================================================

static uint32_t NumberHash(BCObjectRef obj) {
	if (!obj) return 0;
    BCNumberType type = classToType(obj->cls);
    uint64_t v = 0;
    BCNumberGetValueExplicit((BCNumberRef)obj, &v, type);
    return (uint32_t)v;
}

static bool NumberEqual(BCObjectRef a, BCObjectRef b) {
    if (a == b) return true;
	if (!a || !b) return false;
    BCNumberType typeA = classToType(a->cls);
    BCNumberType typeB = classToType(b->cls);
    if (typeB == BCNumberTypeError) return false;

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
    BCNumberType type = classToType(obj->cls);

    // Indent
    for (int i = 0; i < indent; i++) printf("  ");

    switch (type) {
        case BCNumberTypeInt8: printf("Int8(%d)", ((BCNumberInt8*)obj)->value); break;
        case BCNumberTypeInt16: printf("Int16(%d)", ((BCNumberInt16*)obj)->value); break;
        case BCNumberTypeInt32: printf("Int32(%d)", ((BCNumberInt32*)obj)->value); break;
        case BCNumberTypeInt64: printf("Int64(%zd)", ((BCNumberInt64*)obj)->value); break;
        case BCNumberTypeUInt8: printf("UInt8(%u)", ((BCNumberUInt8*)obj)->value); break;
        case BCNumberTypeUInt16: printf("UInt16(%u)", ((BCNumberUInt16*)obj)->value); break;
        case BCNumberTypeUInt32: printf("UInt32(%u)", ((BCNumberUInt32*)obj)->value); break;
        case BCNumberTypeUInt64: printf("UInt64(%zu)", ((BCNumberUInt64*)obj)->value); break;
        case BCNumberTypeFloat: printf("Float(%f)", ((BCNumberFloat*)obj)->value); break;
        case BCNumberTypeDouble: printf("Double(%lf)", ((BCNumberDouble*)obj)->value); break;
        case BCNumberTypeBool: printf(((BCBool*)obj)->value ? "true" : "false"); break;
        default: printf("UnknownNumber()"); break;
    }
}

static BCObjectRef NumberCopy(BCObjectRef obj) { return BCRetain(obj); }

// =============================================================================
// MARK: Class
// =============================================================================

#define INIT_CLASS(StrName) \
    { \
        .name = "BCNumber" #StrName, \
        .dealloc = NULL, \
        .hash = NumberHash, \
        .equal = NumberEqual, \
        .description = NumberDesc, \
        .copy = NumberCopy \
    }

BCClass kClassList[] = {
	INIT_CLASS(Int8),
	INIT_CLASS(Int16),
	INIT_CLASS(Int32),
	INIT_CLASS(Int64),
	INIT_CLASS(UInt8),
	INIT_CLASS(UInt16),
	INIT_CLASS(UInt32),
	INIT_CLASS(UInt64),
	INIT_CLASS(Float),
	INIT_CLASS(Double),
	INIT_CLASS(Bool),
};

#define GLOBAL_CLASS(Name, Index) BCClassRef kBCNumber##Name##ClassRef = &kClassList[Index];

GLOBAL_CLASS(Int8, 0)
GLOBAL_CLASS(Int16, 1)
GLOBAL_CLASS(Int32, 2)
GLOBAL_CLASS(Int64, 3)
GLOBAL_CLASS(UInt8, 4)
GLOBAL_CLASS(UInt16, 5)
GLOBAL_CLASS(UInt32, 6)
GLOBAL_CLASS(UInt64, 7)
GLOBAL_CLASS(Float, 8)
GLOBAL_CLASS(Double, 9)
GLOBAL_CLASS(Bool, 10)

// =============================================================================
// MARK: Public
// =============================================================================

void BCNumberGetValueExplicit(BCNumberRef num, void* value, BCNumberType type) {
	if (!num || !value) return;
	BCNumberType srcType = classToType(num->super.cls);;
	if (srcType == BCNumberTypeError) return;

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
            case BCNumberTypeBool: CONVERT_AND_STORE(SrcType, SrcName, bool); break; \
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
		case BCNumberTypeBool: DISPATCH_DEST(bool, Double); break;
	}
}

BCNumberType BCNumberGetTypeID(BCNumberRef num) {
	if (!num) return BCNumberTypeError;
	return classToType(num->super.cls);
}

// =============================================================================
// MARK: Static Utility
// =============================================================================

#define CLASS_COUNT (sizeof(kClassList) / sizeof(kClassList[0]))

static bool isNumber(BCClassRef cls) { return cls >= kClassList && cls < kClassList + CLASS_COUNT; }

static BCClassRef typeToClass(BCNumberType type) {
	if (type > BCNumberTypeBool || type <= BCNumberTypeError) return NULL;
	return kClassList + type;
}

static BCNumberType classToType(BCClassRef cls) {
	return !isNumber(cls) ? BCNumberTypeError : (BCNumberType) (cls - kClassList);
}