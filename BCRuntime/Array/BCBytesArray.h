#ifndef BCRUNTIME_BCBYTESARRAY_H
#define BCRUNTIME_BCBYTESARRAY_H

#include "../BCTypes.h"

#include <stddef.h>
#include <stdint.h>

extern const BCClassRef kBCBytesArrayClassRef;

BCBytesArrayRef BCBytesArrayCreate(size_t count);
BCBytesArrayRef BCBytesArrayCreateWithBytes(size_t count, const uint8_t* bytes);

void BCBytesArraySet(BCBytesArrayRef arr, uint8_t byte);
uint8_t BCBytesArrayGet(BCBytesArrayRef arr, size_t idx);
size_t BCBytesArrayCount(BCBytesArrayRef arr);
uint8_t* BCBytesArrayBytes(BCBytesArrayRef arr);

#endif //BCRUNTIME_BCBYTESARRAY_H