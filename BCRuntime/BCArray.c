#include <malloc.h>
#include <stdio.h>
#include <stdarg.h>

#include "BCArray.h"

#include "BCString.h"

// =========================================================
// MARK: Struct
// =========================================================

typedef struct BCArray {
	BCObject base;
	BCObjectRef* items;
	size_t count;
	size_t capacity;
} BCArray;

// =========================================================
// MARK: Class
// =========================================================

static void ArrayDeallocImpl(const BCObjectRef obj) {
	const BCArrayRef arr = (BCArrayRef) obj;
	for (size_t i = 0; i < arr->count; i++) BCRelease(arr->items[i]);
	free(arr->items);
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
	const BCArrayRef arr = (BCArrayRef) BCObjectAlloc((BCClassRef) &kBCArrayClass, NULL);
	arr->capacity = 8;
	arr->count = 0;
	arr->items = calloc(arr->capacity, sizeof(BCObjectRef));
	return arr;
}

void ___BCArrayAdd(const BCArrayRef arr, const BCObjectRef item, const bool retain) {
	if (arr->count == arr->capacity) {
		arr->capacity *= 2;
		void* newBuff = realloc(arr->items, arr->capacity * sizeof(BCObjectRef));
		if (!newBuff) return;
		arr->items = newBuff;
	}
	arr->items[arr->count++] = retain ? BCRetain(item) : item;
}

void BCArrayAdd(const BCArrayRef arr, const BCObjectRef item) {
	___BCArrayAdd(arr, item, true);
}

BCObjectRef BCArrayGet(BCArrayRef arr, size_t idx) {
	if (idx >= arr->count) return NULL;
	return arr->items[idx];
}

BCArrayRef BCArrayCreateWithObjects(const bool retain, const size_t count, ...) {
	BCArrayRef arr = BCArrayCreate();
	va_list args;
	va_start(args, count);
	for (size_t i = 0; i < count; i++) {
		BCObjectRef item = va_arg(args, BCObjectRef);
		___BCArrayAdd(arr, item, retain);
	}
	va_end(args);
	return arr;
}
