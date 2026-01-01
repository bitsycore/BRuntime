#ifndef BOBJECT_NUMBER_H
#define BOBJECT_NUMBER_H

#include "../BF_Types.h"

#include <limits.h>

// =========================================================
// MARK: Singletons
// =========================================================

extern BO_BoolRef kBO_True;
extern BO_BoolRef kBO_False;

// =========================================================
// MARK: Class
// =========================================================

typedef enum {
	BO_NumberTypeError = -1,
	BO_NumberTypeInt8 = 0,
	BO_NumberTypeInt16 = 1,
	BO_NumberTypeInt32 = 2,
	BO_NumberTypeInt64 = 3,
	BO_NumberTypeUInt8 = 4,
	BO_NumberTypeUInt16 = 5,
	BO_NumberTypeUInt32 = 6,
	BO_NumberTypeUInt64 = 7,
	BO_NumberTypeFloat = 8,
	BO_NumberTypeDouble = 9,
	BO_NumberTypeBool = 10
} BO_NumberType;

BF_ClassId BO_NumberClassId(BO_NumberType type);
BO_NumberType BO_NumberGetType(BO_NumberRef num);

// =========================================================
// MARK: Constructors
// =========================================================

BO_NumberInt8Ref BO_NumberCreateInt8(int8_t value);
BO_NumberInt16Ref BO_NumberCreateInt16(int16_t value);
BO_NumberInt32Ref BO_NumberCreateInt32(int32_t value);
BO_NumberInt64Ref BO_NumberCreateInt64(int64_t value);
BO_NumberUInt8Ref BO_NumberCreateUInt8(uint8_t value);
BO_NumberUInt16Ref BO_NumberCreateUInt16(uint16_t value);
BO_NumberUInt32Ref BO_NumberCreateUInt32(uint32_t value);
BO_NumberUInt64Ref BO_NumberCreateUInt64(uint64_t value);
BO_NumberFloatRef BO_NumberCreateFloat(float value);
BO_NumberDoubleRef BO_NumberCreateDouble(double value);

static inline BO_BoolRef ___BO_INTERNAL___BoolSelect(const BC_bool val) { return val ? kBO_True : kBO_False; }

#define BCBool(_val_) ((_val_) ? kBO_True : kBO_False)

#if defined(WIN32) || defined(__APPLE__)
#define ___BO_INTERNAL___NUMBER_CREATE_EXTRA
#else
#define ___BO_INTERNAL___NUMBER_CREATE_EXTRA long long: BO_NumberCreateInt64,
#endif

#define BO_NumberCreate(val) _Generic((val), \
    int8_t: BO_NumberCreateInt8, \
    int16_t: BO_NumberCreateInt16, \
    int32_t: BO_NumberCreateInt32, \
    ___BO_INTERNAL___NUMBER_CREATE_EXTRA \
    int64_t: BO_NumberCreateInt64, \
    uint8_t: BO_NumberCreateUInt8, \
    uint16_t: BO_NumberCreateUInt16, \
    uint32_t: BO_NumberCreateUInt32, \
    uint64_t: BO_NumberCreateUInt64, \
    float: BO_NumberCreateFloat, \
    double: BO_NumberCreateDouble, \
    BC_bool: ___BO_INTERNAL___BoolSelect \
)(val)

// =========================================================
// MARK: Type-Specific Macros
// =========================================================

#define BO_NumberCreateByte(val)  BO_NumberCreateInt8(val)
#define BO_NumberCreateShort(val) BO_NumberCreateInt16(val)
#define BO_NumberCreateInt(val)   BO_NumberCreateInt32(val)
#define BO_NumberGetByte(num)  BO_NumberGetInt8(num)
#define BO_NumberGetShort(num) BO_NumberGetInt16(num)
#define BO_NumberGetInt(num)   BO_NumberGetInt32(num)

#if LONG_MAX > 0x7FFFFFFF
    // 64-bit long
    #define BO_NumberCreateLong(val) BO_NumberCreateInt64(val)
    #define BO_NumberGetLong(num)    BO_NumberGetInt64(num)
#else
    // 32-bit long
    #define BO_NumberCreateLong(val) BO_NumberCreateInt32(val)
    #define BO_NumberGetLong(num)    BO_NumberGetInt32(num)
#endif

// =========================================================
// MARK: Methods
// =========================================================

#define ___BO_INTERNAL___DECLARE_NUMBER_GET(Type, Name) \
	Type BO_NumberGet##Name(BO_NumberRef num);

___BO_INTERNAL___DECLARE_NUMBER_GET(int8_t, Int8)
___BO_INTERNAL___DECLARE_NUMBER_GET(int16_t, Int16)
___BO_INTERNAL___DECLARE_NUMBER_GET(int32_t, Int32)
___BO_INTERNAL___DECLARE_NUMBER_GET(int64_t, Int64)
___BO_INTERNAL___DECLARE_NUMBER_GET(uint8_t, UInt8)
___BO_INTERNAL___DECLARE_NUMBER_GET(uint16_t, UInt16)
___BO_INTERNAL___DECLARE_NUMBER_GET(uint32_t, UInt32)
___BO_INTERNAL___DECLARE_NUMBER_GET(uint64_t, UInt64)
___BO_INTERNAL___DECLARE_NUMBER_GET(float, Float)
___BO_INTERNAL___DECLARE_NUMBER_GET(double, Double)
___BO_INTERNAL___DECLARE_NUMBER_GET(BC_bool, Bool)

static void BO_NumberGetExplicit(const BO_NumberRef num, void* value, const BO_NumberType dstType) {
	if (!num || !value) return;
	switch (dstType) {
		case BO_NumberTypeInt8: *(int8_t*)value = BO_NumberGetInt8(num); break;
		case BO_NumberTypeInt16: *(int16_t*)value = BO_NumberGetInt16(num); break;
		case BO_NumberTypeInt32: *(int32_t*)value = BO_NumberGetInt32(num); break;
		case BO_NumberTypeInt64: *(int64_t*)value = BO_NumberGetInt64(num); break;
		case BO_NumberTypeUInt8: *(uint8_t*)value = BO_NumberGetUInt8(num); break;
		case BO_NumberTypeUInt16: *(uint16_t*)value = BO_NumberGetUInt16(num); break;
		case BO_NumberTypeUInt32: *(uint32_t*)value = BO_NumberGetUInt32(num); break;
		case BO_NumberTypeUInt64: *(uint64_t*)value = BO_NumberGetUInt64(num); break;
		case BO_NumberTypeFloat: *(float*)value = BO_NumberGetFloat(num); break;
		case BO_NumberTypeDouble: *(double*)value = BO_NumberGetDouble(num); break;
		case BO_NumberTypeBool: *(BC_bool*)value = BO_NumberGetBool(num); break;
		default:break;
	}
}

#define BO_NumberGetValue(num, outPtr) _Generic( (outPtr), \
    int8_t*   : BO_NumberGetExplicit(num, outPtr, BO_NumberTypeInt8), \
    int16_t*  : BO_NumberGetExplicit(num, outPtr, BO_NumberTypeInt16), \
    int32_t*  : BO_NumberGetExplicit(num, outPtr, BO_NumberTypeInt32), \
    int64_t*  : BO_NumberGetExplicit(num, outPtr, BO_NumberTypeInt64), \
    uint8_t*  : BO_NumberGetExplicit(num, outPtr, BO_NumberTypeUInt8), \
    uint16_t* : BO_NumberGetExplicit(num, outPtr, BO_NumberTypeUInt16), \
    uint32_t* : BO_NumberGetExplicit(num, outPtr, BO_NumberTypeUInt32), \
    uint64_t* : BO_NumberGetExplicit(num, outPtr, BO_NumberTypeUInt64), \
    float*    : BO_NumberGetExplicit(num, outPtr, BO_NumberTypeFloat), \
    double*   : BO_NumberGetExplicit(num, outPtr, BO_NumberTypeDouble), \
    BC_bool*  : BO_NumberGetExplicit(num, outPtr, BO_NumberTypeBool) \
)

#endif //BOBJECT_NUMBER_H
