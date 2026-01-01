#include "BO_List.h"

#include "BCore/Memory/BC_Memory.h"

#include "BO_StringBuilder.h"
#include "../BF_Class.h"

#include <stdarg.h>
#include <stdio.h>

// =========================================================
// MARK: Struct
// =========================================================

typedef struct BO_List {
	BO_Object base;
	size_t count;
	size_t capacity;
	BO_ObjectRef* items;
} BO_List;

// =========================================================
// MARK: Forwards
// =========================================================

static void PRIV_ListAdd(BO_ListRef arr, BO_ObjectRef item, BC_bool retain);

// =========================================================
// MARK: Impl
// =========================================================

static void IMPL_ArrayDealloc(const BO_ObjectRef obj) {
	const BO_ListRef arr = (BO_ListRef)obj;
	for (size_t i = 0; i < arr->count; i++) BO_Release(arr->items[i]);
	BC_Free(arr->items);
}

static BO_StringRef IMPL_ArrayToString(const BO_ObjectRef obj) {
	const BO_ListRef arr = (BO_ListRef)obj;
	const BO_StringBuilderRef sb = BO_StringBuilderCreate(NULL);
	BO_StringBuilderAppendChar(sb, '[');

	for (size_t i = 0; i < arr->count; i++) {
		if (i > 0) {
			BO_StringBuilderAppend(sb, ", ");
		}
		const BO_StringRef itemStr = BO_ToString(arr->items[i]);
		BO_StringBuilderAppendString(sb, itemStr);
		BO_Release($OBJ itemStr);
	}

	BO_StringBuilderAppendChar(sb, ']');
	const BO_StringRef result = BO_StringBuilderFinish(sb, BC_false);
	BO_Release($OBJ sb);
	return result;
}

// =========================================================
// MARK: Class
// =========================================================

static BF_Class kBO_ListClass = {
	.name = "BO_List",
	.id = BF_CLASS_ID_INVALID,
	.dealloc = IMPL_ArrayDealloc,
	.hash = NULL,
	.equal = NULL,
	.toString = IMPL_ArrayToString,
	.copy = NULL,
	.allocSize = sizeof(BO_List)
};

BF_ClassId BO_ListClassId(void) {
	return kBO_ListClass.id;
}

void ___BO_INTERNAL___ListInitialize(void) {
	BF_ClassRegistryInsert(&kBO_ListClass);
}

// =========================================================
// MARK: Constructors
// =========================================================

BO_ListRef BO_ListCreate(void) {
	const BO_ListRef arr = (BO_ListRef)BO_ObjectAlloc(NULL, kBO_ListClass.id);
	arr->capacity = 8;
	arr->count = 0;
	arr->items = BC_Calloc(arr->capacity, sizeof(BO_ObjectRef));
	return arr;
}

BO_ObjectRef BO_ListGet(const BO_ListRef list, const size_t index) {
	if (index >= list->count) return NULL;
	return list->items[index];
}

BO_ListRef BO_ListCreateWithObjects(const BC_bool retain, const size_t count, ...) {
	const BO_ListRef arr = BO_ListCreate();
	va_list args;
	va_start(args, count);
	for (size_t i = 0; i < count; i++) {
		const BO_ObjectRef item = va_arg(args, BO_ObjectRef);
		PRIV_ListAdd(arr, item, retain);
	}
	va_end(args);
	return arr;
}

// =========================================================
// MARK: Methods
// =========================================================

void BO_ListAdd(const BO_ListRef list, const BO_ObjectRef obj) { PRIV_ListAdd(list, obj, BC_true); }

size_t BO_ListCount(const BO_ListRef list) { return list->count; }

BC_bool BO_ListIsEmpty(const BO_ListRef list) { return list->count == 0; }

void BO_ListClear(const BO_ListRef list) {
	for (size_t i = 0; i < list->count; i++) {
		BO_Release(list->items[i]);
	}
	list->count = 0;
}

void BO_ListRemove(const BO_ListRef list, const BO_ObjectRef obj) {
	const size_t index = BO_ListIndexOf(list, obj);
	if (index != -1) {
		BO_ListRemoveAt(list, (size_t)index);
	}
}

void BO_ListRemoveAt(const BO_ListRef list, const size_t index) {
	if (index >= list->count)
		return;

	BO_Release(list->items[index]);

	// Shift
	for (size_t i = index; i < list->count - 1; i++) {
		list->items[i] = list->items[i + 1];
	}

	list->count--;
	list->items[list->count] = NULL;
}

BC_bool BO_ListContains(const BO_ListRef list, const BO_ObjectRef obj) { return BO_ListIndexOf(list, obj) != -1; }

size_t BO_ListIndexOf(const BO_ListRef list, const BO_ObjectRef obj) {
	for (size_t i = 0; i < list->count; i++) {
		if (BO_Equal(list->items[i], obj)) {
			return (size_t)i;
		}
	}
	return -1;
}

BO_ObjectRef BO_ListFirst(const BO_ListRef list) {
	if (list->count == 0)
		return NULL;
	return list->items[0];
}

BO_ObjectRef BO_ListLast(const BO_ListRef list) {
	if (list->count == 0)
		return NULL;
	return list->items[list->count - 1];
}

void BO_ListForEach(const BO_ListRef list, void (*block)(BO_ObjectRef item, size_t index)) {
	if (!block)
		return;
	for (size_t i = 0; i < list->count; i++) {
		block(list->items[i], i);
	}
}

BO_ListRef BO_ListMap(const BO_ListRef list, BO_ObjectRef (*transform)(BO_ObjectRef item, size_t index, void* ctx), void* ctx) {
	if (!transform)
		return NULL;
	const BO_ListRef newList = BO_ListCreate();
	for (size_t i = 0; i < list->count; i++) {
		const BO_ObjectRef transformed = transform(list->items[i], i, ctx);
		BO_ListAdd(newList, transformed);
	}
	return newList;
}

BO_ListRef BO_ListFilter(const BO_ListRef list, BC_bool (*predicate)(BO_ObjectRef item, size_t index)) {
	if (!predicate)
		return NULL;
	const BO_ListRef newList = BO_ListCreate();
	for (size_t i = 0; i < list->count; i++) {
		if (predicate(list->items[i], i)) {
			BO_ListAdd(newList, list->items[i]);
		}
	}
	return newList;
}

// =========================================================
// MARK: Internal
// =========================================================

static void PRIV_ListAdd(const BO_ListRef arr, const BO_ObjectRef item, const BC_bool retain) {
	if (arr->count == arr->capacity) {
		arr->capacity *= 2;
		void* newBuff = BC_Realloc(arr->items, arr->capacity * sizeof(BO_ObjectRef));
		if (!newBuff)
			return;
		arr->items = newBuff;
	}
	arr->items[arr->count++] = retain ? BO_Retain(item) : item;
}
