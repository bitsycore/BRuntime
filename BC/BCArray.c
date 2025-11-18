#include <malloc.h>
#include <stdio.h>

#include "BCArray.h"

// =========================================================
// MARK: Struct
// =========================================================

typedef struct BCArray {
	BCObject base;
	BCObject** items;
	size_t count;
	size_t capacity;
} BCArray;

// =========================================================
// MARK: Forward
// =========================================================

static void _Indent(int level);

// =========================================================
// MARK: Class
// =========================================================

static void _BCArrDealloc(BCObject* obj) {
	BCArray* arr = (BCArray*)obj;
	for(size_t i=0; i<arr->count; i++) BCRelease(arr->items[i]);
	free(arr->items);
}

static void _BCArrDesc(const BCObject* obj, int indent) {
	BCArray* arr = (BCArray*)obj;
	printf("[\n");
	for(size_t i=0; i<arr->count; i++) {
		_Indent(indent + 1);
		arr->items[i]->isa->description(arr->items[i], indent+1);
		printf(",\n");
	}
	_Indent(indent);
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

BCArray* BCArrayCreate(void) {
	BCArray* arr = (BCArray*)BCAllocRaw(&kBCArrayClass, NULL, sizeof(BCArray) - sizeof(BCObject));
	arr->capacity = 8;
	arr->items = calloc(arr->capacity, sizeof(BCObject*));
	return arr;
}

void BCArrayAdd(BCArray* arr, BCObject* item) {
	if (arr->count == arr->capacity) {
		arr->capacity *= 2;
		void* newBuff = realloc(arr->items, arr->capacity * sizeof(BCObject*));
		if (!newBuff) return;
		arr->items = newBuff;
	}
	arr->items[arr->count++] = BCRetain(item);
}

BCObject* BCArrayGet(BCArray* arr, size_t idx) {
	if (idx >= arr->count) return NULL;
	return arr->items[idx];
}

// =========================================================
// MARK: Internal
// =========================================================

static void _Indent(int level) {
	for(int i=0; i<level; i++) printf("  ");
}