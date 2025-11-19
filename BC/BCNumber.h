#ifndef BCRUNTIME_BCNUMBER_H
#define BCRUNTIME_BCNUMBER_H

#include "BCTypes.h"

typedef enum {
    BCNumberTypeInt8 = 1,
    BCNumberTypeInt16,
    BCNumberTypeInt32,
    BCNumberTypeInt64,
    BCNumberTypeUInt8,
    BCNumberTypeUInt16,
    BCNumberTypeUInt32,
    BCNumberTypeUInt64,
    BCNumberTypeFloat,
    BCNumberTypeDouble
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
    default: BCNumberCreateInt32 \
)(val)

void BCNumberGetValueExplicit(BCNumberRef num, void* value, BCNumberType type);
BCNumberType BCNumberGetTypeID(BCNumberRef num);

#define BCNumberGetValue(num, outPtr) _Generic((outPtr), \
    int8_t*: BCNumberGetValueExplicit(num, outPtr, BCNumberTypeInt8), \
    int16_t*: BCNumberGetValueExplicit(num, outPtr, BCNumberTypeInt16), \
    int32_t*: BCNumberGetValueExplicit(num, outPtr, BCNumberTypeInt32), \
    int64_t*: BCNumberGetValueExplicit(num, outPtr, BCNumberTypeInt64), \
    uint8_t*: BCNumberGetValueExplicit(num, outPtr, BCNumberTypeUInt8), \
    uint16_t*: BCNumberGetValueExplicit(num, outPtr, BCNumberTypeUInt16), \
    uint32_t*: BCNumberGetValueExplicit(num, outPtr, BCNumberTypeUInt32), \
    uint64_t*: BCNumberGetValueExplicit(num, outPtr, BCNumberTypeUInt64), \
    float*: BCNumberGetValueExplicit(num, outPtr, BCNumberTypeFloat), \
    double*: BCNumberGetValueExplicit(num, outPtr, BCNumberTypeDouble) \
)

#endif //BCRUNTIME_BCNUMBER_H
