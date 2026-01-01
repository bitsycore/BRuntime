#include "BO_Number.h"

#include "BO_Object.h"
#include "BO_String.h"
#include "../BF_Class.h"

#include <stdio.h>

// =============================================================================
// MARK: Struct
// =============================================================================

// Common Type
// Same layout as BO_Object
typedef struct BO_Number {
	BO_Object base;
} BO_Number;

// Macro for types that use class_reserved (Bool, Int8, UInt8, Int16, UInt16)
#define DEFINE_NUMBER_STRUCT_INLINE(Type, Name)   \
    typedef struct BO_Number##Name {               \
        BO_Object base;                            \
    } BO_Number##Name;

// Macro for types that need separate value field
#define DEFINE_NUMBER_STRUCT(Type, Name)   \
    typedef struct BO_Number##Name {        \
        BO_Object base;                     \
        Type value;                        \
    } BO_Number##Name;

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

extern BF_Class kClassList[];

static BC_bool PRIV_IsNumber(const BF_Class* cls);
static BF_Class* PRIV_TypeToClass(BO_NumberType type);
static BO_NumberType PRIV_ClassToType(const BF_Class* cls);

// =============================================================================
// MARK: Create
// =============================================================================

// For types stored in class_reserved (Bool, Int8, UInt8, Int16, UInt16)
#define IMPLEMENT_CREATE_INLINE(Type, _Name_) \
    BO_Number##_Name_##Ref BO_NumberCreate##_Name_(const Type value) { \
        BO_Number##_Name_* obj = (BO_Number##_Name_*)BO_ObjectAlloc( NULL, kClassList[BO_NumberType##_Name_].id ); \
        if (obj) { \
            obj->base.class_reserved = (uint16_t)value; \
        } \
        return (BO_NumberRef)obj; \
    }

// For types that need separate storage
#define IMPLEMENT_CREATE(Type, _Name_) \
    BO_Number##_Name_##Ref BO_NumberCreate##_Name_(const Type value) { \
        BO_Number##_Name_* obj = (BO_Number##_Name_*)BO_ObjectAlloc( NULL, kClassList[BO_NumberType##_Name_].id ); \
        if (obj) { \
            obj->value = value; \
        } \
        return (BO_NumberRef)obj; \
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
// Bool uses class_reserved, so it's just a BO_Object with no extra fields
static BO_NumberBool kBO_NumberBoolTrue;
static BO_NumberBool kBO_NumberBoolFalse;

// =============================================================================
// MARK: Class Methods
// =============================================================================

static uint32_t IMPL_NumberHash(const BO_ObjectRef obj) {
	if (!obj) return 0;
	const BO_NumberType type = PRIV_ClassToType(BF_ClassIdGetRef(obj->cls));
	uint64_t v = 0;
	BO_NumberGetExplicit((BO_NumberRef)obj, &v, type);
	return (uint32_t)v;
}

static BC_bool IMPL_NumberEqual(const BO_ObjectRef a, const BO_ObjectRef b) {
	if (a == b) return BC_true;
	if (!a || !b) return BC_false;
	const BO_NumberType typeA = PRIV_ClassToType(BF_ClassIdGetRef(a->cls));
	const BO_NumberType typeB = PRIV_ClassToType(BF_ClassIdGetRef(b->cls));
	if (typeB == BO_NumberTypeError) return BC_false;

	// Compare as double if either is float/double
	const BC_bool isFloatA = typeA == BO_NumberTypeFloat || typeA == BO_NumberTypeDouble;
	const BC_bool isFloatB = typeB == BO_NumberTypeFloat || typeB == BO_NumberTypeDouble;

	if (isFloatA || isFloatB) {
		double valA, valB;
		BO_NumberGetExplicit((BO_NumberRef)a, &valA, BO_NumberTypeDouble);
		BO_NumberGetExplicit((BO_NumberRef)b, &valB, BO_NumberTypeDouble);
		return valA == valB; // Exact match for now
	}

	// For simplicity, cast both to Int64 for now
	// Careful with UInt64 large values && Int64 negative values.
	// If one is signed and one is unsigned...
	int64_t valA, valB;
	BO_NumberGetExplicit((BO_NumberRef)a, &valA, BO_NumberTypeInt64);
	BO_NumberGetExplicit((BO_NumberRef)b, &valB, BO_NumberTypeInt64);
	return valA == valB;
}

static BO_StringRef IMPL_NumberToString(const BO_ObjectRef obj) {
	switch (PRIV_ClassToType(BF_ClassIdGetRef(obj->cls))) {
	// Types stored in class_reserved
	case BO_NumberTypeBool: return BO_StringCreate(obj->class_reserved ? "true" : "false");
	case BO_NumberTypeInt8: return BO_StringCreate("%d", (int8_t)obj->class_reserved);
	case BO_NumberTypeUInt8: return BO_StringCreate("%u", (uint8_t)obj->class_reserved);
	case BO_NumberTypeInt16: return BO_StringCreate("%d", (int16_t)obj->class_reserved);
	case BO_NumberTypeUInt16: return BO_StringCreate("%u", (uint16_t)obj->class_reserved);
	// Types with separate storage
	case BO_NumberTypeInt32: return BO_StringCreate("%d", ((BO_NumberInt32*)obj)->value);
	case BO_NumberTypeInt64: return BO_StringCreate("%lld", ((BO_NumberInt64*)obj)->value);
	case BO_NumberTypeUInt32: return BO_StringCreate("%u", ((BO_NumberUInt32*)obj)->value);
	case BO_NumberTypeUInt64: return BO_StringCreate("%llu", ((BO_NumberUInt64*)obj)->value);
	case BO_NumberTypeFloat: return BO_StringCreate("%f", ((BO_NumberFloat*)obj)->value);
	case BO_NumberTypeDouble: return BO_StringCreate("%lf", ((BO_NumberDouble*)obj)->value);
	default: return BO_StringCreate("<Number Error>");
	}
}

static BO_ObjectRef IMPL_NumberCopy(const BO_ObjectRef obj) { return BO_Retain(obj); }

// =============================================================================
// MARK: Class
// =============================================================================

// For types stored in class_reserved (no extra fields)
#define INIT_CLASS_INLINE(StrName) { \
    .name = "BO_Number" #StrName, \
    .id = BF_CLASS_ID_INVALID, \
    .dealloc = NULL, \
    .hash = IMPL_NumberHash, \
    .equal = IMPL_NumberEqual, \
    .toString = IMPL_NumberToString, \
    .copy = IMPL_NumberCopy, \
    .allocSize = sizeof(BO_Number) \
}

// For types with separate value field
#define INIT_CLASS(StrName) { \
    .name = "BO_Number" #StrName, \
    .id = BF_CLASS_ID_INVALID, \
    .dealloc = NULL, \
    .hash = IMPL_NumberHash, \
    .equal = IMPL_NumberEqual, \
    .toString = IMPL_NumberToString, \
    .copy = IMPL_NumberCopy, \
    .allocSize = sizeof(BO_Number##StrName) \
}

BF_Class kClassList[] = {
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

BO_BoolRef kBO_True = (BO_BoolRef)&kBO_NumberBoolTrue;
BO_BoolRef kBO_False = (BO_BoolRef)&kBO_NumberBoolFalse;

#define CLASS_COUNT (sizeof(kClassList) / sizeof(kClassList[0]))

BF_ClassId BO_NumberClassId(const BO_NumberType type) {
	const BF_Class* cls = PRIV_TypeToClass(type);
	return cls ? cls->id : BF_CLASS_ID_INVALID;
}

void INTERNAL_BO_NumberInitialize(void) {
	for (size_t i = 0; i < CLASS_COUNT; i++) { BF_ClassRegistryInsert(&kClassList[i]); }
	const uint32_t flags = BC_OBJECT_FLAG_CONSTANT;
	const BF_ClassId boolClassId = kClassList[BO_NumberTypeBool].id;
	kBO_NumberBoolTrue = (BO_NumberBool){
		.base = {
			.cls = boolClassId,
			.flags = flags,
			.class_reserved = BC_true
		}
	};
	kBO_NumberBoolFalse = (BO_NumberBool){
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
	Type BO_NumberGet##Name(BO_NumberRef num) { \
		if (!num) return 0; \
		switch (PRIV_ClassToType(BF_ClassIdGetRef(num->base.cls))) { \
			/* Inline types stored in class_reserved */ \
			case BO_NumberTypeBool:   return (Type)num->base.class_reserved; \
			case BO_NumberTypeInt8:   return (Type)(int8_t)num->base.class_reserved; \
			case BO_NumberTypeUInt8:  return (Type)(uint8_t)num->base.class_reserved; \
			case BO_NumberTypeInt16:  return (Type)(int16_t)num->base.class_reserved; \
			case BO_NumberTypeUInt16: return (Type)(uint16_t)num->base.class_reserved; \
			/* Types with separate value field */ \
			case BO_NumberTypeInt32:  return (Type)((BO_NumberInt32*)num)->value; \
			case BO_NumberTypeInt64:  return (Type)((BO_NumberInt64*)num)->value; \
			case BO_NumberTypeUInt32: return (Type)((BO_NumberUInt32*)num)->value; \
			case BO_NumberTypeUInt64: return (Type)((BO_NumberUInt64*)num)->value; \
			case BO_NumberTypeFloat:  return (Type)((BO_NumberFloat*)num)->value; \
			case BO_NumberTypeDouble: return (Type)((BO_NumberDouble*)num)->value; \
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

BO_NumberType BO_NumberGetType(const BO_NumberRef num) {
	if (!num) return BO_NumberTypeError;
	return PRIV_ClassToType(BF_ClassIdGetRef(num->base.cls));
}

// =============================================================================
// MARK: Internal
// =============================================================================

static inline BC_bool PRIV_IsNumber(const BF_Class* cls) { return cls >= kClassList && cls < kClassList + CLASS_COUNT; }

static inline BF_Class* PRIV_TypeToClass(const BO_NumberType type) {
	if (type > BO_NumberTypeBool || type <= BO_NumberTypeError) return NULL;
	return &kClassList[type];
}

static inline BO_NumberType PRIV_ClassToType(const BF_Class* cls) { return !PRIV_IsNumber(cls) ? BO_NumberTypeError : (BO_NumberType)(cls - kClassList); }