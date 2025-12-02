#ifndef BCRUNTIME_BCBYTESARRAY_H
#define BCRUNTIME_BCBYTESARRAY_H

#include "../Core/BCTypes.h"

#include "../Struct/BCRange.h"

#include <stddef.h>
#include <stdint.h>

// =========================================================
// MARK: Class
// =========================================================

BCClassId BCBytesArrayClassId(void);

// =========================================================
// MARK: Constructors
// =========================================================

BCBytesArrayRef BCBytesArrayCreate(size_t count);
BCBytesArrayRef BCBytesArrayCreateWithBytes(size_t count, const uint8_t* bytes);

// =========================================================
// MARK: Methods
// =========================================================

void BCBytesArraySet(BCBytesArrayRef arr, size_t idx, uint8_t byte);
void BCBytesArraySetRange(BCBytesArrayRef arr, BCRange range, uint8_t byte);
void BCBytesArrayFill(BCBytesArrayRef arr, uint8_t byte);

uint8_t BCBytesArrayGet(BCBytesArrayRef arr, size_t idx);
size_t BCBytesArrayCount(BCBytesArrayRef arr);
uint8_t* BCBytesArrayBytes(BCBytesArrayRef arr);

int8_t BCBytesArrayCompare(BCBytesArrayRef arr1, BCBytesArrayRef arr2);

#endif //BCRUNTIME_BCBYTESARRAY_H
