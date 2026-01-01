#ifndef BOBJECT_BYTES_ARRAY_H
#define BOBJECT_BYTES_ARRAY_H

#include "BCore/BC_Range.h"

#include "../BF_Types.h"

#include <stddef.h>
#include <stdint.h>

// =========================================================
// MARK: Class
// =========================================================

BF_ClassId BO_BytesArrayClassId(void);

// =========================================================
// MARK: Constructors
// =========================================================

BO_BytesArrayRef BO_BytesArrayCreate(size_t count);
BO_BytesArrayRef BO_BytesArrayCreateWithBytes(size_t count, const uint8_t* bytes);

// =========================================================
// MARK: Methods
// =========================================================

void BO_BytesArraySet(BO_BytesArrayRef arr, size_t idx, uint8_t byte);
void BO_BytesArraySetRange(BO_BytesArrayRef arr, BC_Range range, uint8_t byte);
void BO_BytesArrayFill(BO_BytesArrayRef arr, uint8_t byte);

uint8_t BO_BytesArrayGet(BO_BytesArrayRef arr, size_t idx);
size_t BO_BytesArrayCount(BO_BytesArrayRef arr);
uint8_t* BO_BytesArrayBytes(BO_BytesArrayRef arr);

int8_t BO_BytesArrayCompare(BO_BytesArrayRef arr1, BO_BytesArrayRef arr2);

#endif //BOBJECT_BYTES_ARRAY_H
