#include <malloc.h>
#include <stdio.h>

#include "BCArray.h"

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

static void _BCArrDealloc(BCObjectRef obj) {
	BCArrayRef arr = (BCArrayRef) obj;
	for (size_t i = 0; i < arr->count; i++) BCRelease(arr->items[i]);
	free(arr->items);
}

static void _BCArrDesc(BCObjectRef obj, int indent) {
	BCArrayRef arr = (BCArrayRef) obj;
	for (int i = 0; i < indent; i++) printf("  ");
	printf("[\n");

	for (size_t i = 0; i < arr->count; i++) {
		arr->items[i]->cls->description(arr->items[i], indent +1);
		printf(",\n");
	}

	for (int i = 0; i < indent; i++) printf("  ");
	printf("]");
}

static const BCClass kBCArrayClass = {
	"BCArray",
	_BCArrDealloc,
	NULL,
	NULL,
	_BCArrDesc,
	NULL
};

// =========================================================
// MARK: Public
// =========================================================

BCArrayRef BCArrayCreate(void) {
	BCArrayRef arr = (BCArrayRef) BCAllocRaw((BCClassRef) &kBCArrayClass, NULL, sizeof(BCArray) - sizeof(BCObject));
	arr->capacity = 8;
	arr->items = calloc(arr->capacity, sizeof(BCObjectRef));
	return arr;
}

void BCArrayAdd(BCArrayRef arr, BCObjectRef item) {
	if (arr->count == arr->capacity) {
		arr->capacity *= 2;
		void* newBuff = realloc(arr->items, arr->capacity * sizeof(BCObjectRef));
		if (!newBuff) return;
		arr->items = newBuff;
	}
	arr->items[arr->count++] = BCRetain(item);
}

BCObjectRef BCArrayGet(BCArrayRef arr, size_t idx) {
	if (idx >= arr->count) return NULL;
	return arr->items[idx];
}