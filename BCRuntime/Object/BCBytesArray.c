#include "BCBytesArray.h"

#include "BCObject.h"
#include "BCStringBuilder.h"
#include "../Core/BCClass.h"
#include "../Struct/BCRange.h"

#include <string.h>

// =========================================================
// MARK: Struct
// =========================================================

typedef struct BCBytesArray {
	BCObject base;
	size_t count;
	uint8_t bytes[];
} BCBytesArray;

// =========================================================
// MARK: Class Methods
// =========================================================

static BC_bool BCBytesArrayEqual(const BCObjectRef self, const BCObjectRef other) {
	if (self == other) return BC_true;
	if (!self || !other) return BC_false;
	if (self->cls != other->cls) return BC_false;

	const BCBytesArrayRef a = (BCBytesArrayRef) self;
	const BCBytesArrayRef b = (BCBytesArrayRef) other;

	if (a->count != b->count) return BC_false;

	return memcmp(a->bytes, b->bytes, a->count) == 0 ? BC_true : BC_false;
}

static uint32_t BCBytesArrayHash(const BCObjectRef self) {
	const BCBytesArrayRef selfCast = (BCBytesArrayRef) self;
	uint64_t hash = 5381;
	for (size_t i = 0; i < selfCast->count; i++) {
		hash = (hash << 5) + hash + selfCast->bytes[i];
	}
	return hash;
}

static BCStringRef BCBytesArrayToString(const BCObjectRef self) {
	const BCStringBuilderRef builder = BCStringBuilderCreate(NULL);
	const BCBytesArrayRef selfCast = (BCBytesArrayRef) self;
	if (selfCast->count == 0) BCStringBuilderAppendFormat(builder, "(0) <Empty>");
	else BCStringBuilderAppendFormat(builder, "(%zu) 0x", selfCast->count);
	for (size_t i = 0; i < selfCast->count; i++) {
		BCStringBuilderAppendFormat(builder, "%02X", selfCast->bytes[i]);
	}
	const BCStringRef result = BCStringBuilderFinish(builder, BC_true);
	BCRelease($OBJ builder);
	return result;
}

static BCObjectRef BCBytesArrayCopy(const BCObjectRef self) {
	const BCBytesArrayRef selfCast = (BCBytesArrayRef) self;
	return (BCObjectRef) BCBytesArrayCreateWithBytes(selfCast->count, selfCast->bytes);
}

// =========================================================
// MARK: Class
// =========================================================

static BCClass kBCBytesArrayClass = {
	.name = "BCBytesArray",
	.id = BC_CLASS_ID_INVALID,
	.dealloc = NULL,
	.hash = BCBytesArrayHash,
	.equal = BCBytesArrayEqual,
	.toString = BCBytesArrayToString,
	.copy = BCBytesArrayCopy,
	.allocSize = sizeof(BCBytesArray)
};

BCClassId BCBytesArrayClassId(void) {
	return kBCBytesArrayClass.id;
}

void ___BCINTERNAL___BytesArrayClassInit(void) {
	BCClassRegistryInsert(&kBCBytesArrayClass);
}

// =========================================================
// MARK: Constructors
// =========================================================

BCBytesArrayRef BCBytesArrayCreate(const size_t count) {
	const BCBytesArrayRef arr = (BCBytesArrayRef) BCObjectAllocWithConfig(NULL, kBCBytesArrayClass.id, count * sizeof(uint8_t), BC_OBJECT_DEFAULT_FLAGS);
	arr->count = count;
	memset(arr->bytes, 0, count);
	return arr;
}

BCBytesArrayRef BCBytesArrayCreateWithBytes(const size_t count, const uint8_t *bytes) {
	const BCBytesArrayRef arr = (BCBytesArrayRef) BCObjectAllocWithConfig(NULL, kBCBytesArrayClass.id, count * sizeof(uint8_t), BC_OBJECT_DEFAULT_FLAGS);
	arr->count = count;
	memcpy(arr->bytes, bytes, count);
	return arr;
}

// =========================================================
// MARK: Methods
// =========================================================

void BCBytesArraySet(const BCBytesArrayRef arr, const size_t idx, const uint8_t byte) {
	if (idx < arr->count) {
		arr->bytes[idx] = byte;
	}
}

void BCBytesArraySetRange(const BCBytesArrayRef arr, BCRange range, uint8_t byte) {
	if (!arr) return;
	const size_t start = range.start;
	const size_t len = range.length;
	if (start > arr->count || len > arr->count - start) return;
	memset(arr->bytes + start, byte, len);
}

void BCBytesArrayFill(const BCBytesArrayRef arr, const uint8_t byte) {
	memset(arr->bytes, byte, arr->count);
}

uint8_t BCBytesArrayGet(const BCBytesArrayRef arr, const size_t idx) {
	if (idx >= arr->count) return 0; // Safety check
	return arr->bytes[idx];
}

size_t BCBytesArrayCount(const BCBytesArrayRef arr) {
	return arr->count;
}

uint8_t *BCBytesArrayBytes(const BCBytesArrayRef arr) {
	return arr->bytes;
}

int8_t BCBytesArrayCompare(const BCBytesArrayRef arr1, const BCBytesArrayRef arr2) {
	const size_t minCount = arr1->count < arr2->count ? arr1->count : arr2->count;
	const int cmp = memcmp(arr1->bytes, arr2->bytes, minCount);
	if (cmp < 0) return -1;
	if (cmp > 0) return 1;
	if (arr1->count < arr2->count) return -1;
	if (arr1->count > arr2->count) return 1;
	return 0;
}
