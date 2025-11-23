#include "BCArray.h"

#include "../BCString.h"
#include "../Utilities/BCMemory.h"

#include <stdarg.h>
#include <stdio.h>

// =========================================================
// MARK: Struct
// =========================================================

typedef struct BCArray {
	BCObject base;
	size_t count;
	size_t capacity;
	BCObjectRef* items;
} BCArray;

// =========================================================
// MARK: Private
// =========================================================

static void ArrayAdd(const BCArrayRef arr, const BCObjectRef item, const bool retain) {
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
	const BCArrayRef arr = (BCArrayRef) obj;
	for (size_t i = 0; i < arr->count; i++) BCRelease(arr->items[i]);
	BCFree(arr->items);
}

static BCStringRef ArrayToStringImpl(const BCObjectRef obj) {
	const BCArrayRef arr = (BCArrayRef) obj;
	return BCStringCreate("BCArray(count: %zu)", arr->count);
	printf("[ ");

	for (size_t i = 0; i < arr->count; i++) {
		arr->items[i]->cls->toString(arr->items[i]);
		printf(", ");
	}

	printf("]");
}

static const BCClass kBCArrayClass = {
	"BCArray",
	ArrayDeallocImpl,
	NULL,
	NULL,
	ArrayToStringImpl,
	NULL,
	sizeof(BCArray)
};

// =========================================================
// MARK: Public
// =========================================================

BCArrayRef BCArrayCreate(void) {
	const BCArrayRef arr = (BCArrayRef) BCAllocObject((BCClassRef) &kBCArrayClass, NULL);
	arr->capacity = 8;
	arr->count = 0;
	arr->items = BCCalloc(arr->capacity, sizeof(BCObjectRef));
	return arr;
}

void BCArrayAdd(const BCArrayRef arr, const BCObjectRef item) {
	ArrayAdd(arr, item, true);
}

BCObjectRef BCArrayGet(const BCArrayRef arr, const size_t idx) {
	if (idx >= arr->count) return NULL;
	return arr->items[idx];
}

BCArrayRef BCArrayCreateWithObjects(const bool retain, const size_t count, ...) {
	const BCArrayRef arr = BCArrayCreate();
	va_list args;
	va_start(args, count);
	for (size_t i = 0; i < count; i++) {
		const BCObjectRef item = va_arg(args, BCObjectRef);
		ArrayAdd(arr, item, retain);
	}
	va_end(args);
	return arr;
}
