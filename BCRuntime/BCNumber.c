#include "BCNumber.h"
#include "BCObject.h"

#include <stdio.h>

#include "BCClass.h"
#include "BCString.h"

// =============================================================================
// MARK: Struct
// =============================================================================

// Common Type
// Same layout as BCObject
typedef struct BCNumber {
	BCObject super;
} BCNumber;

#define DEFINE_NUMBER_STRUCT(Type, Name)   \
    typedef struct BCNumber##Name {        \
        BCObject super;                    \
        Type value;                        \
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
DEFINE_NUMBER_STRUCT(BC_bool, Bool)

// =============================================================================
// MARK: Forward
// =============================================================================

extern BCClass kClassList[];

static BC_bool isNumber(BCClassRef cls);
static BCClassRef typeToClass(BCNumberType type);
static BCNumberType classToType(BCClassRef cls);

// =============================================================================
// MARK: Create
// =============================================================================

#define IMPLEMENT_CREATE(Type, _Name_) \
    BCNumber##_Name_##Ref BCNumberCreate##_Name_(Type value) { \
        BCNumber##_Name_* obj = (BCNumber##_Name_*)BCObjectAlloc( (BCClassRef) &kClassList[BCNumberType##_Name_], NULL); \
        if (obj) { \
            obj->value = value; \
        } \
        return (BCNumberRef)obj; \
    }

IMPLEMENT_CREATE(int8_t, Int8)
IMPLEMENT_CREATE(int16_t, Int16)
IMPLEMENT_CREATE(int32_t, Int32)
IMPLEMENT_CREATE(int64_t, Int64)
IMPLEMENT_CREATE(uint8_t, UInt8)
IMPLEMENT_CREATE(uint16_t, UInt16)
IMPLEMENT_CREATE(uint32_t, UInt32)
IMPLEMENT_CREATE(uint64_t, UInt64)
IMPLEMENT_CREATE(float, Float)
IMPLEMENT_CREATE(double, Double)

// Bool is a special case, it is a singleton object that is always allocated.
static BCNumberBool kBCNumberBoolTrue;
static BCNumberBool kBCNumberBoolFalse;

// =============================================================================
// MARK: Class Methods
// =============================================================================

static uint32_t NumberHashImpl(const BCObjectRef obj) {
	if (!obj) return 0;
    const BCNumberType type = classToType(obj->cls);
    uint64_t v = 0;
    BCNumberGetExplicit((BCNumberRef)obj, &v, type);
    return (uint32_t)v;
}

static BC_bool NumberEqualImpl(const BCObjectRef a, const BCObjectRef b) {
    if (a == b) return BC_true;
	if (!a || !b) return BC_false;
    const BCNumberType typeA = classToType(a->cls);
    const BCNumberType typeB = classToType(b->cls);
    if (typeB == BCNumberTypeError) return BC_false;

    // Compare as double if either is float/double
    const BC_bool isFloatA = (typeA == BCNumberTypeFloat || typeA == BCNumberTypeDouble);
    const BC_bool isFloatB = (typeB == BCNumberTypeFloat || typeB == BCNumberTypeDouble);

    if (isFloatA || isFloatB) {
        double valA, valB;
        BCNumberGetExplicit((BCNumberRef)a, &valA, BCNumberTypeDouble);
        BCNumberGetExplicit((BCNumberRef)b, &valB, BCNumberTypeDouble);
        return valA == valB; // Exact match for now
    }

	// For simplicity, cast both to Int64 for now
	// Careful with UInt64 large values && Int64 negative values.
	// If one is signed and one is unsigned...
	int64_t valA, valB;
	BCNumberGetExplicit((BCNumberRef)a, &valA, BCNumberTypeInt64);
	BCNumberGetExplicit((BCNumberRef)b, &valB, BCNumberTypeInt64);
	return valA == valB;
}

static BCStringRef NumberToStringImpl(const BCObjectRef obj) {
	switch (classToType(obj->cls)) {
		case BCNumberTypeInt8: return BCStringCreate("%d", ((BCNumberInt8*)obj)->value);
		case BCNumberTypeInt16: return BCStringCreate("%d", ((BCNumberInt16*)obj)->value);
		case BCNumberTypeInt32: return BCStringCreate("%d", ((BCNumberInt32*)obj)->value);
		case BCNumberTypeInt64: return BCStringCreate("%zd", ((BCNumberInt64*)obj)->value);
		case BCNumberTypeUInt8: return BCStringCreate("%u", ((BCNumberUInt8*)obj)->value);
		case BCNumberTypeUInt16: return BCStringCreate("%u", ((BCNumberUInt16*)obj)->value);
		case BCNumberTypeUInt32: return BCStringCreate("%u", ((BCNumberUInt32*)obj)->value);
		case BCNumberTypeUInt64: return BCStringCreate("%zu", ((BCNumberUInt64*)obj)->value);
		case BCNumberTypeFloat: return BCStringCreate("%f", ((BCNumberFloat*)obj)->value);
		case BCNumberTypeDouble: return BCStringCreate("%lf", ((BCNumberDouble*)obj)->value);
		case BCNumberTypeBool: return BCStringCreate(((BCNumberBool*)obj)->value ? "true" : "false");
		default: return BCStringCreate("<Number Error>");
	}
}

static BCObjectRef NumberCopyImpl(const BCObjectRef obj) { return BCRetain(obj); }

// =============================================================================
// MARK: Class
// =============================================================================

#define INIT_CLASS(StrName) { \
    .name = "BCNumber" #StrName, \
    .dealloc = NULL, \
    .hash = NumberHashImpl, \
    .equal = NumberEqualImpl, \
    .toString = NumberToStringImpl, \
    .copy = NumberCopyImpl, \
    .allocSize = sizeof(BCNumber##StrName) \
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

// =============================================================================
// MARK: Public
// =============================================================================

#define DEFINE_NUMBER_GET(Type, Name) \
	Type BCNumberGet##Name(BCNumberRef num) { \
		if (!num) return 0; \
		switch (classToType(num->super.cls)) { \
			case BCNumberTypeInt8:   return (Type)((BCNumberInt8*)num)->value; \
			case BCNumberTypeInt16:  return (Type)((BCNumberInt16*)num)->value; \
			case BCNumberTypeInt32:  return (Type)((BCNumberInt32*)num)->value; \
			case BCNumberTypeInt64:  return (Type)((BCNumberInt64*)num)->value; \
			case BCNumberTypeUInt8:  return (Type)((BCNumberUInt8*)num)->value; \
			case BCNumberTypeUInt16: return (Type)((BCNumberUInt16*)num)->value; \
			case BCNumberTypeUInt32: return (Type)((BCNumberUInt32*)num)->value; \
			case BCNumberTypeUInt64: return (Type)((BCNumberUInt64*)num)->value; \
			case BCNumberTypeFloat:  return (Type)((BCNumberFloat*)num)->value; \
			case BCNumberTypeDouble: return (Type)((BCNumberDouble*)num)->value; \
			case BCNumberTypeBool:   return (Type)((BCNumberBool*)num)->value; \
			default: return 0; \
		}\
	}

DEFINE_NUMBER_GET(int8_t, Int8)
DEFINE_NUMBER_GET(int16_t, Int16)
DEFINE_NUMBER_GET(int32_t, Int32)
DEFINE_NUMBER_GET(int64_t, Int64)
DEFINE_NUMBER_GET(uint8_t, UInt8)
DEFINE_NUMBER_GET(uint16_t, UInt16)
DEFINE_NUMBER_GET(uint32_t, UInt32)
DEFINE_NUMBER_GET(uint64_t, UInt64)
DEFINE_NUMBER_GET(float, Float)
DEFINE_NUMBER_GET(double, Double)
DEFINE_NUMBER_GET(BC_bool, Bool)

BCNumberType BCNumberGetType(const BCNumberRef num) {
	if (!num) return BCNumberTypeError;
	return classToType(num->super.cls);
}

// =============================================================================
// MARK: Static Utility
// =============================================================================

#define CLASS_COUNT (sizeof(kClassList) / sizeof(kClassList[0]))

static inline BC_bool isNumber(const BCClassRef cls) { return cls >= kClassList && cls < kClassList + CLASS_COUNT; }

static inline BCClassRef typeToClass(const BCNumberType type) {
	if (type > BCNumberTypeBool || type <= BCNumberTypeError) return NULL;
	return &kClassList[type];
}

static inline BCNumberType classToType(const BCClassRef cls) {
	return !isNumber(cls) ? BCNumberTypeError : (BCNumberType) (cls - kClassList);
}

// =============================================================================
// MARK: Init
// =============================================================================

BCBoolRef kBCTrue = (BCBoolRef) &kBCNumberBoolTrue;
BCBoolRef kBCFalse = (BCBoolRef) &kBCNumberBoolFalse;

void ___BCINTERNAL___NumberInitialize(void) {
	const uint32_t flags = BC_OBJECT_FLAG_CONSTANT;
	kBCNumberBoolTrue = (BCNumberBool) {
		.super = {
			.cls = &kClassList[BCNumberTypeBool],
			.flags = flags
		},
		.value = BC_true
	};
	kBCNumberBoolFalse = (BCNumberBool) {
		.super = {
			.cls = &kClassList[BCNumberTypeBool],
			.flags = flags,
		},
		.value = BC_false,
	};
}