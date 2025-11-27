#ifndef BCRUNTIME_BCNUMBER_H
#define BCRUNTIME_BCNUMBER_H

#include "../BCTypes.h"

// =========================================================
// MARK: Singletons
// =========================================================

extern BCBoolRef kBCTrue;
extern BCBoolRef kBCFalse;

// =========================================================
// MARK: Class
// =========================================================

typedef enum {
	BCNumberTypeError = -1,
	BCNumberTypeInt8 = 0,
	BCNumberTypeInt16 = 1,
	BCNumberTypeInt32 = 2,
	BCNumberTypeInt64 = 3,
	BCNumberTypeUInt8 = 4,
	BCNumberTypeUInt16 = 5,
	BCNumberTypeUInt32 = 6,
	BCNumberTypeUInt64 = 7,
	BCNumberTypeFloat = 8,
	BCNumberTypeDouble = 9,
	BCNumberTypeBool = 10
} BCNumberType;

BCClassId BCNumberClassId(BCNumberType type);
BCNumberType BCNumberGetType(BCNumberRef num);

// =========================================================
// MARK: Constructors
// =========================================================

BCNumberInt8Ref BCNumberCreateInt8(int8_t value);
BCNumberInt16Ref BCNumberCreateInt16(int16_t value);
BCNumberInt32Ref BCNumberCreateInt32(int32_t value);
BCNumberInt64Ref BCNumberCreateInt64(int64_t value);
BCNumberUInt8Ref BCNumberCreateUInt8(uint8_t value);
BCNumberUInt16Ref BCNumberCreateUInt16(uint16_t value);
BCNumberUInt32Ref BCNumberCreateUInt32(uint32_t value);
BCNumberUInt64Ref BCNumberCreateUInt64(uint64_t value);
BCNumberFloatRef BCNumberCreateFloat(float value);
BCNumberDoubleRef BCNumberCreateDouble(double value);

static inline BCBoolRef ___BCINTERNAL___BoolSelect(const BC_bool val) { return val ? kBCTrue : kBCFalse; }

#define BCBool(_val_) ((_val_) ? kBCTrue : kBCFalse)

#if defined(WIN32) || defined(__APPLE__)
#define ___BCINTERNAL___NUMBER_CREATE_EXTRA
#else
#define ___BCINTERNAL___NUMBER_CREATE_EXTRA long long: BCNumberCreateInt64,
#endif

#define BCNumberCreate(val) _Generic((val), \
    int8_t: BCNumberCreateInt8, \
    int16_t: BCNumberCreateInt16, \
    int32_t: BCNumberCreateInt32, \
    ___BCINTERNAL___NUMBER_CREATE_EXTRA \
    int64_t: BCNumberCreateInt64, \
    uint8_t: BCNumberCreateUInt8, \
    uint16_t: BCNumberCreateUInt16, \
    uint32_t: BCNumberCreateUInt32, \
    uint64_t: BCNumberCreateUInt64, \
    float: BCNumberCreateFloat, \
    double: BCNumberCreateDouble, \
    BC_bool: ___BCINTERNAL___BoolSelect \
)(val)

// =========================================================
// MARK: Methods
// =========================================================

#define ___BCINTERNAL___DECLARE_NUMBER_GET(Type, Name) \
	Type BCNumberGet##Name(BCNumberRef num);

___BCINTERNAL___DECLARE_NUMBER_GET(int8_t, Int8)
___BCINTERNAL___DECLARE_NUMBER_GET(int16_t, Int16)
___BCINTERNAL___DECLARE_NUMBER_GET(int32_t, Int32)
___BCINTERNAL___DECLARE_NUMBER_GET(int64_t, Int64)
___BCINTERNAL___DECLARE_NUMBER_GET(uint8_t, UInt8)
___BCINTERNAL___DECLARE_NUMBER_GET(uint16_t, UInt16)
___BCINTERNAL___DECLARE_NUMBER_GET(uint32_t, UInt32)
___BCINTERNAL___DECLARE_NUMBER_GET(uint64_t, UInt64)
___BCINTERNAL___DECLARE_NUMBER_GET(float, Float)
___BCINTERNAL___DECLARE_NUMBER_GET(double, Double)
___BCINTERNAL___DECLARE_NUMBER_GET(BC_bool, Bool)

static void BCNumberGetExplicit(const BCNumberRef num, void* value, const BCNumberType dstType) {
	if (!num || !value) return;
	switch (dstType) {
		case BCNumberTypeInt8: *(int8_t*)value = BCNumberGetInt8(num); break;
		case BCNumberTypeInt16: *(int16_t*)value = BCNumberGetInt16(num); break;
		case BCNumberTypeInt32: *(int32_t*)value = BCNumberGetInt32(num); break;
		case BCNumberTypeInt64: *(int64_t*)value = BCNumberGetInt64(num); break;
		case BCNumberTypeUInt8: *(uint8_t*)value = BCNumberGetUInt8(num); break;
		case BCNumberTypeUInt16: *(uint16_t*)value = BCNumberGetUInt16(num); break;
		case BCNumberTypeUInt32: *(uint32_t*)value = BCNumberGetUInt32(num); break;
		case BCNumberTypeUInt64: *(uint64_t*)value = BCNumberGetUInt64(num); break;
		case BCNumberTypeFloat: *(float*)value = BCNumberGetFloat(num); break;
		case BCNumberTypeDouble: *(double*)value = BCNumberGetDouble(num); break;
		case BCNumberTypeBool: *(BC_bool*)value = BCNumberGetBool(num); break;
		default:break;
	}
}

#define BCNumberGetValue(num, outPtr) _Generic( (outPtr), \
    int8_t*   : BCNumberGetExplicit(num, outPtr, BCNumberTypeInt8), \
    int16_t*  : BCNumberGetExplicit(num, outPtr, BCNumberTypeInt16), \
    int32_t*  : BCNumberGetExplicit(num, outPtr, BCNumberTypeInt32), \
    int64_t*  : BCNumberGetExplicit(num, outPtr, BCNumberTypeInt64), \
    uint8_t*  : BCNumberGetExplicit(num, outPtr, BCNumberTypeUInt8), \
    uint16_t* : BCNumberGetExplicit(num, outPtr, BCNumberTypeUInt16), \
    uint32_t* : BCNumberGetExplicit(num, outPtr, BCNumberTypeUInt32), \
    uint64_t* : BCNumberGetExplicit(num, outPtr, BCNumberTypeUInt64), \
    float*    : BCNumberGetExplicit(num, outPtr, BCNumberTypeFloat), \
    double*   : BCNumberGetExplicit(num, outPtr, BCNumberTypeDouble), \
    BC_bool*  : BCNumberGetExplicit(num, outPtr, BCNumberTypeBool) \
)

#endif //BCRUNTIME_BCNUMBER_H
