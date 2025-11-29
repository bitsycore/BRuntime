#include "BCList.h"

#include "BCStringBuilder.h"
#include "../Core/BCClass.h"
#include "../Utilities/BC_Memory.h"

#include <stdarg.h>
#include <stdio.h>

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
// MARK: Forwards
// =========================================================

static void ListAdd(BCListRef arr, BCObjectRef item, BC_bool retain);

// =========================================================
// MARK: Impl
// =========================================================

static void ArrayDeallocImpl(const BCObjectRef obj) {
	const BCListRef arr = (BCListRef)obj;
	for (size_t i = 0; i < arr->count; i++) BCRelease(arr->items[i]);
	BCFree(arr->items);
}

static BCStringRef ArrayToStringImpl(const BCObjectRef obj) {
	const BCListRef arr = (BCListRef)obj;
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
	const BCStringRef result = BCStringBuilderFinish(sb, BC_false);
	BCRelease($OBJ sb);
	return result;
}

// =========================================================
// MARK: Class
// =========================================================

static BCClass kBCListClass = {
	.name = "BCList",
	.id = BC_CLASS_ID_INVALID,
	.dealloc = ArrayDeallocImpl,
	.hash = NULL,
	.equal = NULL,
	.toString = ArrayToStringImpl,
	.copy = NULL,
	.allocSize = sizeof(BCList)
};

BCClassId BCListClassId(void) {
	return kBCListClass.id;
}

void ___BCINTERNAL___ListInitialize(void) {
	BCClassRegistryInsert(&kBCListClass);
}

// =========================================================
// MARK: Constructors
// =========================================================

BCListRef BCListCreate(void) {
	const BCListRef arr = (BCListRef)BCObjectAlloc(NULL, kBCListClass.id);
	arr->capacity = 8;
	arr->count = 0;
	arr->items = BCCalloc(arr->capacity, sizeof(BCObjectRef));
	return arr;
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

// =========================================================
// MARK: Methods
// =========================================================

void BCListAdd(const BCListRef list, const BCObjectRef obj) { ListAdd(list, obj, BC_true); }

size_t BCListCount(const BCListRef list) { return list->count; }

BC_bool BCListIsEmpty(const BCListRef list) { return list->count == 0; }

void BCListClear(const BCListRef list) {
	for (size_t i = 0; i < list->count; i++) {
		BCRelease(list->items[i]);
	}
	list->count = 0;
}

void BCListRemove(const BCListRef list, const BCObjectRef obj) {
	const size_t index = BCListIndexOf(list, obj);
	if (index != -1) {
		BCListRemoveAt(list, (size_t)index);
	}
}

void BCListRemoveAt(const BCListRef list, const size_t index) {
	if (index >= list->count)
		return;

	BCRelease(list->items[index]);

	// Shift
	for (size_t i = index; i < list->count - 1; i++) {
		list->items[i] = list->items[i + 1];
	}

	list->count--;
	list->items[list->count] = NULL;
}

BC_bool BCListContains(const BCListRef list, const BCObjectRef obj) { return BCListIndexOf(list, obj) != -1; }

size_t BCListIndexOf(const BCListRef list, const BCObjectRef obj) {
	for (size_t i = 0; i < list->count; i++) {
		if (BCEqual(list->items[i], obj)) {
			return (size_t)i;
		}
	}
	return -1;
}

BCObjectRef BCListFirst(const BCListRef list) {
	if (list->count == 0)
		return NULL;
	return list->items[0];
}

BCObjectRef BCListLast(const BCListRef list) {
	if (list->count == 0)
		return NULL;
	return list->items[list->count - 1];
}

void BCListForEach(const BCListRef list, void (*block)(BCObjectRef item, size_t index)) {
	if (!block)
		return;
	for (size_t i = 0; i < list->count; i++) {
		block(list->items[i], i);
	}
}

BCListRef BCListMap(const BCListRef list, BCObjectRef (*transform)(BCObjectRef item, size_t index, void* ctx), void* ctx) {
	if (!transform)
		return NULL;
	const BCListRef newList = BCListCreate();
	for (size_t i = 0; i < list->count; i++) {
		const BCObjectRef transformed = transform(list->items[i], i, ctx);
		BCListAdd(newList, transformed);
	}
	return newList;
}

BCListRef BCListFilter(const BCListRef list, BC_bool (*predicate)(BCObjectRef item, size_t index)) {
	if (!predicate)
		return NULL;
	const BCListRef newList = BCListCreate();
	for (size_t i = 0; i < list->count; i++) {
		if (predicate(list->items[i], i)) {
			BCListAdd(newList, list->items[i]);
		}
	}
	return newList;
}

// =========================================================
// MARK: Internal
// =========================================================

static void ListAdd(const BCListRef arr, const BCObjectRef item, const BC_bool retain) {
	if (arr->count == arr->capacity) {
		arr->capacity *= 2;
		void* newBuff = BCRealloc(arr->items, arr->capacity * sizeof(BCObjectRef));
		if (!newBuff)
			return;
		arr->items = newBuff;
	}
	arr->items[arr->count++] = retain ? BCRetain(item) : item;
}
