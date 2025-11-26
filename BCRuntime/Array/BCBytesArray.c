#include "BCBytesArray.h"

#include <string.h>

#include "../BCClass.h"
#include "../BCObject.h"

// =========================================================
// MARK: Struct
// =========================================================

typedef struct BCBytesArray {
	BCObject base;
	size_t count;
	uint8_t bytes[];
} BCBytesArray;

// =========================================================
// MARK: Class
// =========================================================

static BCClass kBCBytesArrayClass = {
	.name = "BCBytesArray",
	.id = BC_CLASS_ID_INVALID,
	.dealloc = NULL,
	.hash =	NULL,
	.equal = NULL,
	.toString = NULL,
	.copy = NULL,
	.allocSize = sizeof(BCBytesArray)
};

BCClassId BCBytesArrayClassId(void) {
	return kBCBytesArrayClass.id;
}

void ___BCINTERNAL___BytesArrayClassInit(void) {
	BCClassRegister(&kBCBytesArrayClass);
}

// =========================================================
// MARK: Constructors
// =========================================================

BCBytesArrayRef BCBytesArrayCreate(const size_t count) {
	const BCBytesArrayRef arr = (BCBytesArrayRef) BCObjectAllocWithConfig(NULL, kBCBytesArrayClass.id, count * sizeof(uint8_t), BC_OBJECT_FLAG_REFCOUNT);
	arr->count = count;
	memset(arr->bytes, 0, count);
	return arr;
}

BCBytesArrayRef BCBytesArrayCreateWithBytes(const size_t count, const uint8_t* bytes) {
	const BCBytesArrayRef arr = (BCBytesArrayRef) BCObjectAllocWithConfig(NULL, kBCBytesArrayClass.id, count * sizeof(uint8_t), BC_OBJECT_FLAG_REFCOUNT);
	arr->count = count;
	memcpy(arr->bytes, bytes, count);
	return arr;
}

// =========================================================
// MARK: Methods
// =========================================================

uint8_t BCBytesArrayGet(const BCBytesArrayRef arr, const size_t idx) {
	return arr->bytes[idx];
}

size_t BCBytesArrayCount(const BCBytesArrayRef arr) {
	return arr->count;
}

uint8_t* BCBytesArrayBytes(const BCBytesArrayRef arr) {
	return arr->bytes;
}