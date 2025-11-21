#ifndef BCRUNTIME_BCNUMBER_H
#define BCRUNTIME_BCNUMBER_H

#include "BCTypes.h"

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

extern BCBoolRef kBCTrue;
extern BCBoolRef kBCFalse;

static inline BCBoolRef ___BCBoolSelect(const bool val) { return val ? kBCTrue : kBCFalse; }

#ifdef WIN32
#define ___BC___PLATFORM_EXTRA_NUM_CREA_MACRO
#else
#define ___BC___PLATFORM_EXTRA_NUM_CREA_MACRO long long: BCNumberCreateInt64,
#endif

#define BCNumberCreate(val) _Generic((val), \
    int8_t: BCNumberCreateInt8, \
    int16_t: BCNumberCreateInt16, \
    int32_t: BCNumberCreateInt32, \
    ___BC___PLATFORM_EXTRA_NUM_CREA_MACRO \
    int64_t: BCNumberCreateInt64, \
    uint8_t: BCNumberCreateUInt8, \
    uint16_t: BCNumberCreateUInt16, \
    uint32_t: BCNumberCreateUInt32, \
    uint64_t: BCNumberCreateUInt64, \
    float: BCNumberCreateFloat, \
    double: BCNumberCreateDouble, \
    bool: ___BCBoolSelect, \
    default: BCNumberCreateInt32 \
)(val)

void BCNumberGetValueExplicit(BCNumberRef num, void* value, BCNumberType dstType);
BCNumberType BCNumberGetType(BCNumberRef num);

#define BCNumberGetValue(num, outPtr) _Generic( (outPtr), \
    int8_t*		: BCNumberGetValueExplicit(num, outPtr, BCNumberTypeInt8), \
    int16_t*	: BCNumberGetValueExplicit(num, outPtr, BCNumberTypeInt16), \
    int32_t*	: BCNumberGetValueExplicit(num, outPtr, BCNumberTypeInt32), \
    int64_t*	: BCNumberGetValueExplicit(num, outPtr, BCNumberTypeInt64), \
    uint8_t*	: BCNumberGetValueExplicit(num, outPtr, BCNumberTypeUInt8), \
    uint16_t*	: BCNumberGetValueExplicit(num, outPtr, BCNumberTypeUInt16), \
    uint32_t*	: BCNumberGetValueExplicit(num, outPtr, BCNumberTypeUInt32), \
    uint64_t*	: BCNumberGetValueExplicit(num, outPtr, BCNumberTypeUInt64), \
    float*		: BCNumberGetValueExplicit(num, outPtr, BCNumberTypeFloat), \
    double*		: BCNumberGetValueExplicit(num, outPtr, BCNumberTypeDouble), \
    bool*		: BCNumberGetValueExplicit(num, outPtr, BCNumberTypeBool) \
)

#endif //BCRUNTIME_BCNUMBER_H
