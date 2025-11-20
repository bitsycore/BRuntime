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

BCNumberRef BCNumberCreateInt8(int8_t value);
BCNumberRef BCNumberCreateInt16(int16_t value);
BCNumberRef BCNumberCreateInt32(int32_t value);
BCNumberRef BCNumberCreateInt64(int64_t value);
BCNumberRef BCNumberCreateUInt8(uint8_t value);
BCNumberRef BCNumberCreateUInt16(uint16_t value);
BCNumberRef BCNumberCreateUInt32(uint32_t value);
BCNumberRef BCNumberCreateUInt64(uint64_t value);
BCNumberRef BCNumberCreateFloat(float value);
BCNumberRef BCNumberCreateDouble(double value);
BCBoolRef BCNumberGetBool(bool value);

extern BCBoolRef BCTrue;
extern BCBoolRef BCFalse;

#ifdef WIN32

#define BCNumberCreate(val) _Generic((val), \
    int8_t: BCNumberCreateInt8, \
    int16_t: BCNumberCreateInt16, \
    int32_t: BCNumberCreateInt32, \
    int64_t: BCNumberCreateInt64, \
    uint8_t: BCNumberCreateUInt8, \
    uint16_t: BCNumberCreateUInt16, \
    uint32_t: BCNumberCreateUInt32, \
    uint64_t: BCNumberCreateUInt64, \
    float: BCNumberCreateFloat, \
    double: BCNumberCreateDouble, \
    bool: BCNumberGetBool, \
    default: BCNumberCreateInt32 \
)(val)

#else

#define BCNumberCreate(val) _Generic((val), \
    int8_t: BCNumberCreateInt8, \
    int16_t: BCNumberCreateInt16, \
    int32_t: BCNumberCreateInt32, \
    long long: BCNumberCreateInt64, \
    int64_t: BCNumberCreateInt64, \
    uint8_t: BCNumberCreateUInt8, \
    uint16_t: BCNumberCreateUInt16, \
    uint32_t: BCNumberCreateUInt32, \
    uint64_t: BCNumberCreateUInt64, \
    float: BCNumberCreateFloat, \
    double: BCNumberCreateDouble, \
    bool: BCNumberGetBool, \
    default: BCNumberCreateInt32 \
)(val)

#endif

void BCNumberGetValueExplicit(BCNumberRef num, void* value, BCNumberType dstType);
BCNumberType BCNumberGetTypeID(BCNumberRef num);

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
