#include "BO_BytesArray.h"

#include "BCore/BC_Range.h"

#include "BO_Object.h"
#include "BO_StringBuilder.h"
#include "../BF_Class.h"

#include <string.h>

// =========================================================
// MARK: Struct
// =========================================================

typedef struct BO_BytesArray {
	BO_Object base;
	size_t size;
	uint8_t bytes[];
} BO_BytesArray;

// =========================================================
// MARK: Class Methods
// =========================================================

static BC_bool IMPL_BytesArrayEqual(const BO_ObjectRef self, const BO_ObjectRef other) {
	if (self == other) return BC_true;
	if (!self || !other) return BC_false;
	if (self->cls != other->cls) return BC_false;

	const BO_BytesArrayRef a = (BO_BytesArrayRef) self;
	const BO_BytesArrayRef b = (BO_BytesArrayRef) other;

	if (a->size != b->size) return BC_false;

	return memcmp(a->bytes, b->bytes, a->size) == 0 ? BC_true : BC_false;
}

static uint32_t IMPL_BytesArrayHash(const BO_ObjectRef self) {
	const BO_BytesArrayRef selfCast = (BO_BytesArrayRef) self;
	uint64_t hash = 5381;
	for (size_t i = 0; i < selfCast->size; i++) {
		hash = (hash << 5) + hash + selfCast->bytes[i];
	}
	return hash;
}

static BO_StringRef IMPL_BytesArrayToString(const BO_ObjectRef self) {
	const BO_StringBuilderRef builder = BO_StringBuilderCreate(NULL);
	const BO_BytesArrayRef selfCast = (BO_BytesArrayRef) self;
	if (selfCast->size == 0) BO_StringBuilderAppendFormat(builder, "(0) <Empty>");
	else BO_StringBuilderAppendFormat(builder, "(%zu) 0x", selfCast->size);
	for (size_t i = 0; i < selfCast->size; i++) {
		BO_StringBuilderAppendFormat(builder, "%02X", selfCast->bytes[i]);
	}
	const BO_StringRef result = BO_StringBuilderFinish(builder, BC_true);
	BO_Release($OBJ builder);
	return result;
}

static BO_ObjectRef IMPL_BytesArrayCopy(const BO_ObjectRef self) {
	const BO_BytesArrayRef selfCast = (BO_BytesArrayRef) self;
	return (BO_ObjectRef) BO_BytesArrayCreateWithBytes(selfCast->size, selfCast->bytes);
}

// =========================================================
// MARK: Class
// =========================================================

static BF_Class kBO_BytesArrayClass = {
	.name = "BO_BytesArray",
	.id = BF_CLASS_ID_INVALID,
	.dealloc = NULL,
	.hash = IMPL_BytesArrayHash,
	.equal = IMPL_BytesArrayEqual,
	.toString = IMPL_BytesArrayToString,
	.copy = IMPL_BytesArrayCopy,
	.allocSize = sizeof(BO_BytesArray)
};

BF_ClassId BO_BytesArrayClassId(void) {
	return kBO_BytesArrayClass.id;
}

void INTERNAL_BO_BytesArrayInitialize(void) {
	BF_ClassRegistryInsert(&kBO_BytesArrayClass);
}

// =========================================================
// MARK: Constructors
// =========================================================

BO_BytesArrayRef BO_BytesArrayCreate(const size_t size) {
	const BO_BytesArrayRef arr = (BO_BytesArrayRef) BO_ObjectAllocWithConfig(NULL, kBO_BytesArrayClass.id, size * sizeof(uint8_t), BC_OBJECT_DEFAULT_FLAGS);
	arr->size = size;
	memset(arr->bytes, 0, size);
	return arr;
}

BO_BytesArrayRef BO_BytesArrayCreateWithBytes(const size_t size, const uint8_t *bytes) {
	const BO_BytesArrayRef arr = (BO_BytesArrayRef) BO_ObjectAllocWithConfig(NULL, kBO_BytesArrayClass.id, size * sizeof(uint8_t), BC_OBJECT_DEFAULT_FLAGS);
	arr->size = size;
	memcpy(arr->bytes, bytes, size);
	return arr;
}

// =========================================================
// MARK: Methods
// =========================================================

void BO_BytesArraySet(const BO_BytesArrayRef arr, const size_t idx, const uint8_t byte) {
	if (idx < arr->size) {
		arr->bytes[idx] = byte;
	}
}

void BO_BytesArraySetRange(const BO_BytesArrayRef arr, const BC_Range range, const uint8_t byte) {
	if (!arr) return;
	const size_t start = range.start;
	const size_t len = range.length;
	if (start > arr->size || len > arr->size - start) return;
	memset(arr->bytes + start, byte, len);
}

void BO_BytesArrayFill(const BO_BytesArrayRef arr, const uint8_t byte) {
	memset(arr->bytes, byte, arr->size);
}

uint8_t BO_BytesArrayGet(const BO_BytesArrayRef arr, const size_t idx) {
	if (idx >= arr->size) return 0;
	return arr->bytes[idx];
}

size_t BO_BytesArraySize(const BO_BytesArrayRef arr) {
	return arr->size;
}

uint8_t *BO_BytesArrayBytes(const BO_BytesArrayRef arr) {
	return arr->bytes;
}

int8_t BO_BytesArrayCompare(const BO_BytesArrayRef arr1, const BO_BytesArrayRef arr2) {
	const size_t minSize = arr1->size < arr2->size ? arr1->size : arr2->size;
	return memcmp(arr1->bytes, arr2->bytes, minSize);
}
