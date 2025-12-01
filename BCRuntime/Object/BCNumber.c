#include "BCNumber.h"

#include "BCObject.h"
#include "BCString.h"
#include "../Core/BCClass.h"

#include <stdio.h>

// =============================================================================
// MARK: Struct
// =============================================================================

// Common Type
// Same layout as BCObject
typedef struct BCNumber {
	BCObject base;
} BCNumber;

// Macro for types that use class_reserved (Bool, Int8, UInt8, Int16, UInt16)
#define DEFINE_NUMBER_STRUCT_INLINE(Type, Name)   \
    typedef struct BCNumber##Name {               \
        BCObject base;                            \
    } BCNumber##Name;

// Macro for types that need separate value field
#define DEFINE_NUMBER_STRUCT(Type, Name)   \
    typedef struct BCNumber##Name {        \
        BCObject base;                     \
        Type value;                        \
    } BCNumber##Name;

// Types stored in class_reserved (16-bit or less)
DEFINE_NUMBER_STRUCT_INLINE(BC_bool, Bool)
DEFINE_NUMBER_STRUCT_INLINE(int8_t, Int8)
DEFINE_NUMBER_STRUCT_INLINE(uint8_t, UInt8)
DEFINE_NUMBER_STRUCT_INLINE(int16_t, Int16)
DEFINE_NUMBER_STRUCT_INLINE(uint16_t, UInt16)

// Types that need separate storage
DEFINE_NUMBER_STRUCT(int32_t, Int32)
DEFINE_NUMBER_STRUCT(int64_t, Int64)
DEFINE_NUMBER_STRUCT(uint32_t, UInt32)
DEFINE_NUMBER_STRUCT(uint64_t, UInt64)
DEFINE_NUMBER_STRUCT(float, Float)
DEFINE_NUMBER_STRUCT(double, Double)

// =============================================================================
// MARK: Forward
// =============================================================================

extern BCClass kClassList[];

static BC_bool IsNumber(BCClassRef cls);
static BCClassRef TypeToClass(BCNumberType type);
static BCNumberType ClassToType(BCClassRef cls);

// =============================================================================
// MARK: Create
// =============================================================================

// For types stored in class_reserved (Bool, Int8, UInt8, Int16, UInt16)
#define IMPLEMENT_CREATE_INLINE(Type, _Name_) \
    BCNumber##_Name_##Ref BCNumberCreate##_Name_(const Type value) { \
        BCNumber##_Name_* obj = (BCNumber##_Name_*)BCObjectAlloc( NULL, kClassList[BCNumberType##_Name_].id ); \
        if (obj) { \
            obj->base.class_reserved = (uint16_t)value; \
        } \
        return (BCNumberRef)obj; \
    }

// For types that need separate storage
#define IMPLEMENT_CREATE(Type, _Name_) \
    BCNumber##_Name_##Ref BCNumberCreate##_Name_(const Type value) { \
        BCNumber##_Name_* obj = (BCNumber##_Name_*)BCObjectAlloc( NULL, kClassList[BCNumberType##_Name_].id ); \
        if (obj) { \
            obj->value = value; \
        } \
        return (BCNumberRef)obj; \
    }

IMPLEMENT_CREATE_INLINE(int8_t, Int8)
IMPLEMENT_CREATE_INLINE(uint8_t, UInt8)
IMPLEMENT_CREATE_INLINE(int16_t, Int16)
IMPLEMENT_CREATE_INLINE(uint16_t, UInt16)

IMPLEMENT_CREATE(int32_t, Int32)
IMPLEMENT_CREATE(int64_t, Int64)
IMPLEMENT_CREATE(uint32_t, UInt32)
IMPLEMENT_CREATE(uint64_t, UInt64)
IMPLEMENT_CREATE(float, Float)
IMPLEMENT_CREATE(double, Double)

// Bool is a special case, it is a singleton object that is always allocated.
// Bool uses class_reserved, so it's just a BCObject with no extra fields
static BCNumberBool kBCNumberBoolTrue;
static BCNumberBool kBCNumberBoolFalse;

// =============================================================================
// MARK: Class Methods
// =============================================================================

static uint32_t NumberHashImpl(const BCObjectRef obj) {
	if (!obj) return 0;
	const BCNumberType type = ClassToType(BCClassIdGetRef(obj->cls));
	uint64_t v = 0;
	BCNumberGetExplicit((BCNumberRef)obj, &v, type);
	return (uint32_t)v;
}

static BC_bool NumberEqualImpl(const BCObjectRef a, const BCObjectRef b) {
	if (a == b) return BC_true;
	if (!a || !b) return BC_false;
	const BCNumberType typeA = ClassToType(BCClassIdGetRef(a->cls));
	const BCNumberType typeB = ClassToType(BCClassIdGetRef(b->cls));
	if (typeB == BCNumberTypeError) return BC_false;

	// Compare as double if either is float/double
	const BC_bool isFloatA = typeA == BCNumberTypeFloat || typeA == BCNumberTypeDouble;
	const BC_bool isFloatB = typeB == BCNumberTypeFloat || typeB == BCNumberTypeDouble;

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
	switch (ClassToType(BCClassIdGetRef(obj->cls))) {
	// Types stored in class_reserved
	case BCNumberTypeBool: return BCStringCreate(obj->class_reserved ? "true" : "false");
	case BCNumberTypeInt8: return BCStringCreate("%d", (int8_t)obj->class_reserved);
	case BCNumberTypeUInt8: return BCStringCreate("%u", (uint8_t)obj->class_reserved);
	case BCNumberTypeInt16: return BCStringCreate("%d", (int16_t)obj->class_reserved);
	case BCNumberTypeUInt16: return BCStringCreate("%u", (uint16_t)obj->class_reserved);
	// Types with separate storage
	case BCNumberTypeInt32: return BCStringCreate("%d", ((BCNumberInt32*)obj)->value);
	case BCNumberTypeInt64: return BCStringCreate("%lld", ((BCNumberInt64*)obj)->value);
	case BCNumberTypeUInt32: return BCStringCreate("%u", ((BCNumberUInt32*)obj)->value);
	case BCNumberTypeUInt64: return BCStringCreate("%llu", ((BCNumberUInt64*)obj)->value);
	case BCNumberTypeFloat: return BCStringCreate("%f", ((BCNumberFloat*)obj)->value);
	case BCNumberTypeDouble: return BCStringCreate("%lf", ((BCNumberDouble*)obj)->value);
	default: return BCStringCreate("<Number Error>");
	}
}

static BCObjectRef NumberCopyImpl(const BCObjectRef obj) { return BCRetain(obj); }

// =============================================================================
// MARK: Class
// =============================================================================

// For types stored in class_reserved (no extra fields)
#define INIT_CLASS_INLINE(StrName) { \
    .name = "BCNumber" #StrName, \
    .id = BC_CLASS_ID_INVALID, \
    .dealloc = NULL, \
    .hash = NumberHashImpl, \
    .equal = NumberEqualImpl, \
    .toString = NumberToStringImpl, \
    .copy = NumberCopyImpl, \
    .allocSize = sizeof(BCNumber) \
}

// For types with separate value field
#define INIT_CLASS(StrName) { \
    .name = "BCNumber" #StrName, \
    .id = BC_CLASS_ID_INVALID, \
    .dealloc = NULL, \
    .hash = NumberHashImpl, \
    .equal = NumberEqualImpl, \
    .toString = NumberToStringImpl, \
    .copy = NumberCopyImpl, \
    .allocSize = sizeof(BCNumber##StrName) \
}

BCClass kClassList[] = {
	INIT_CLASS_INLINE(Int8),
	INIT_CLASS_INLINE(Int16),
	INIT_CLASS(Int32),
	INIT_CLASS(Int64),
	INIT_CLASS_INLINE(UInt8),
	INIT_CLASS_INLINE(UInt16),
	INIT_CLASS(UInt32),
	INIT_CLASS(UInt64),
	INIT_CLASS(Float),
	INIT_CLASS(Double),
	INIT_CLASS_INLINE(Bool),
};

BCBoolRef kBCTrue = (BCBoolRef)&kBCNumberBoolTrue;
BCBoolRef kBCFalse = (BCBoolRef)&kBCNumberBoolFalse;

#define CLASS_COUNT (sizeof(kClassList) / sizeof(kClassList[0]))

BCClassId BCNumberClassId(const BCNumberType type) {
	const BCClassRef cls = TypeToClass(type);
	return cls ? cls->id : BC_CLASS_ID_INVALID;
}

void ___BCINTERNAL___NumberInitialize(void) {
	for (size_t i = 0; i < CLASS_COUNT; i++) { BCClassRegistryInsert(&kClassList[i]); }
	const uint32_t flags = BC_OBJECT_FLAG_CONSTANT;
	const BCClassId boolClassId = kClassList[BCNumberTypeBool].id;
	kBCNumberBoolTrue = (BCNumberBool){
		.base = {
			.cls = boolClassId,
			.flags = flags,
			.class_reserved = BC_true
		}
	};
	kBCNumberBoolFalse = (BCNumberBool){
		.base = {
			.cls = boolClassId,
			.flags = flags,
			.class_reserved = BC_false
		}
	};
}

// =============================================================================
// MARK: Methods
// =============================================================================

#define DEFINE_NUMBER_GET(Type, Name) \
	Type BCNumberGet##Name(BCNumberRef num) { \
		if (!num) return 0; \
		switch (ClassToType(BCClassIdGetRef(num->base.cls))) { \
			/* Inline types stored in class_reserved */ \
			case BCNumberTypeBool:   return (Type)num->base.class_reserved; \
			case BCNumberTypeInt8:   return (Type)(int8_t)num->base.class_reserved; \
			case BCNumberTypeUInt8:  return (Type)(uint8_t)num->base.class_reserved; \
			case BCNumberTypeInt16:  return (Type)(int16_t)num->base.class_reserved; \
			case BCNumberTypeUInt16: return (Type)(uint16_t)num->base.class_reserved; \
			/* Types with separate value field */ \
			case BCNumberTypeInt32:  return (Type)((BCNumberInt32*)num)->value; \
			case BCNumberTypeInt64:  return (Type)((BCNumberInt64*)num)->value; \
			case BCNumberTypeUInt32: return (Type)((BCNumberUInt32*)num)->value; \
			case BCNumberTypeUInt64: return (Type)((BCNumberUInt64*)num)->value; \
			case BCNumberTypeFloat:  return (Type)((BCNumberFloat*)num)->value; \
			case BCNumberTypeDouble: return (Type)((BCNumberDouble*)num)->value; \
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
	return ClassToType(BCClassIdGetRef(num->base.cls));
}

// =============================================================================
// MARK: Internal
// =============================================================================

static inline BC_bool IsNumber(const BCClassRef cls) { return cls >= kClassList && cls < kClassList + CLASS_COUNT; }

static inline BCClassRef TypeToClass(const BCNumberType type) {
	if (type > BCNumberTypeBool || type <= BCNumberTypeError) return NULL;
	return &kClassList[type];
}

static inline BCNumberType ClassToType(const BCClassRef cls) {
	return !IsNumber(cls) ? BCNumberTypeError : (BCNumberType)(cls - kClassList);
}
