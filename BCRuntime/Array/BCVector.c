#include "BCVector.h"

#include "../BCString.h"
#include "../Utilities/BCMemory.h"

#include <stdarg.h>
#include <stdio.h>

// =========================================================
// MARK: Struct
// =========================================================

typedef struct BCVector {
	BCObject base;
	size_t count;
	size_t capacity;
	BCObjectRef* items;
} BCVector;

// =========================================================
// MARK: Private
// =========================================================

static void ArrayAdd(const BCVectorRef arr, const BCObjectRef item, const bool retain) {
	if (arr->count == arr->capacity) {
		arr->capacity *= 2;
		void* newBuff = BCRealloc(arr->items, arr->capacity * sizeof(BCObjectRef));
		if (!newBuff) return;
		arr->items = newBuff;
	}
	arr->items[arr->count++] = retain ? BCRetain(item) : item;
}

// =========================================================
// MARK: Class
// =========================================================

static void ArrayDeallocImpl(const BCObjectRef obj) {
	const BCVectorRef arr = (BCVectorRef) obj;
	for (size_t i = 0; i < arr->count; i++) BCRelease(arr->items[i]);
	BCFree(arr->items);
}

static BCStringRef ArrayToStringImpl(const BCObjectRef obj) {
	const BCVectorRef arr = (BCVectorRef) obj;
	return BCStringCreate("BCVector(count: %zu)", arr->count);
	printf("[ ");

	for (size_t i = 0; i < arr->count; i++) {
		arr->items[i]->cls->toString(arr->items[i]);
		printf(", ");
	}

	printf("]");
}

static const BCClass kBCVectorClass = {
	"BCVector",
	ArrayDeallocImpl,
	NULL,
	NULL,
	ArrayToStringImpl,
	NULL,
	sizeof(BCVector)
};

// =========================================================
// MARK: Public
// =========================================================

BCVectorRef BCVectorCreate(void) {
	const BCVectorRef arr = (BCVectorRef) BCAllocObject((BCClassRef) &kBCVectorClass, NULL);
	arr->capacity = 8;
	arr->count = 0;
	arr->items = BCCalloc(arr->capacity, sizeof(BCObjectRef));
	return arr;
}

void BCVectorAdd(const BCVectorRef arr, const BCObjectRef item) {
	ArrayAdd(arr, item, true);
}

BCObjectRef BCVectorGet(const BCVectorRef arr, const size_t idx) {
	if (idx >= arr->count) return NULL;
	return arr->items[idx];
}

BCVectorRef BCVectorCreateWithObjects(const bool retain, const size_t count, ...) {
	const BCVectorRef arr = BCVectorCreate();
	va_list args;
	va_start(args, count);
	for (size_t i = 0; i < count; i++) {
		const BCObjectRef item = va_arg(args, BCObjectRef);
		ArrayAdd(arr, item, retain);
	}
	va_end(args);
	return arr;
}
