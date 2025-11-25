#include "BCList.h"

#include "../BCClass.h"
#include "../BCString.h"
#include "../Utilities/BCMemory.h"

#include <stdarg.h>
#include <stdio.h>

#include "../BCStringBuilder.h"

// =========================================================
// MARK: Struct
// =========================================================

typedef struct BCList {
	BCObject base;
	size_t count;
	size_t capacity;
	BCObjectRef* items;
} BCList;

// =========================================================
// MARK: Private
// =========================================================

static void ListAdd(const BCListRef arr, const BCObjectRef item, const BC_bool retain) {
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
	const BCListRef arr = (BCListRef) obj;
	for (size_t i = 0; i < arr->count; i++) BCRelease(arr->items[i]);
	BCFree(arr->items);
}

static BCStringRef ArrayToStringImpl(const BCObjectRef obj) {
	const BCListRef arr = (BCListRef) obj;
	const BCStringBuilderRef sb = BCStringBuilderCreate(NULL);
	BCStringBuilderAppendChar(sb, '[');

	for (size_t i = 0; i < arr->count; i++) {
		if (i > 0) {
			BCStringBuilderAppend(sb, ", ");
		}
		const BCStringRef itemStr = BCToString(arr->items[i]);
		BCStringBuilderAppendString(sb, itemStr);
		BCRelease($OBJ itemStr);
	}

	BCStringBuilderAppendChar(sb, ']');
	const BCStringRef result = BCStringBuilderFinish(sb);
	BCRelease($OBJ sb);
	return result;
}

static const BCClass kBCListClass = {
	"BCList",
	ArrayDeallocImpl,
	NULL,
	NULL,
	ArrayToStringImpl,
	NULL,
	sizeof(BCList)
};

const BCClassRef kBCListClassRef = (BCClassRef) &kBCListClass;

// =========================================================
// MARK: Public
// =========================================================

BCListRef BCListCreate(void) {
	const BCListRef arr = (BCListRef) BCObjectAlloc(NULL, (BCClassRef) &kBCListClass);
	arr->capacity = 8;
	arr->count = 0;
	arr->items = BCCalloc(arr->capacity, sizeof(BCObjectRef));
	return arr;
}

void BCListAdd(const BCListRef list, const BCObjectRef obj) {
	ListAdd(list, obj, BC_true);
}

BCObjectRef BCListGet(const BCListRef list, const size_t index) {
	if (index >= list->count) return NULL;
	return list->items[index];
}

BCListRef BCListCreateWithObjects(const BC_bool retain, const size_t count, ...) {
	const BCListRef arr = BCListCreate();
	va_list args;
	va_start(args, count);
	for (size_t i = 0; i < count; i++) {
		const BCObjectRef item = va_arg(args, BCObjectRef);
		ListAdd(arr, item, retain);
	}
	va_end(args);
	return arr;
}
