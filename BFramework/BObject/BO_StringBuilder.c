#include "BO_StringBuilder.h"

#include "BO_Object.h"
#include "BO_String.h"
#include "../../BCore/Memory/BC_Allocator.h"
#include "../BF_Class.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =========================================================
// MARK: Struct
// =========================================================

typedef struct BO_StringBuilder {
	BO_Object base;
	size_t length;
	size_t capacity;
	char* buffer;
} BO_StringBuilder;

// =========================================================
// MARK: Internal
// =========================================================

static inline size_t PRIV_NextPowerOfTwo(size_t x) {
	if (x <= 1) return 1;
	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
#if SIZE_MAX > UINT32_MAX
	x |= x >> 32; // 64-bit
#endif
	x++;
	return x;
}

static inline void PRIV_AppendStr(const BO_StringBuilderRef builder, const char* str, const size_t strLen) {
	if (strLen == 0) return;
	BO_StringBuilderEnsureCapacity(builder, builder->length + strLen);
	memcpy(builder->buffer + builder->length, str, strLen);
	builder->length += strLen;
}

// =========================================================
// MARK: Class Methods
// =========================================================

void IMPL_StringBuilderDealloc(const BO_ObjectRef obj) {
	const BO_StringBuilderRef builder = (BO_StringBuilderRef)obj;
	if (builder->buffer) {
		const BC_AllocatorRef alloc = BO_ObjectGetAllocator(obj);
		alloc->free(builder->buffer, alloc->context);
		builder->buffer = NULL;
	}
}

uint32_t IMPL_StringBuilderHash(const BO_ObjectRef obj) {
	return INTERNAL_BO_StringHasher(((BO_StringBuilder*)obj)->buffer);
}

BC_bool IMPL_StringBuilderEqual(const BO_ObjectRef a, const BO_ObjectRef b) {
	if (a == b) return BC_true;

	if (a->cls == BO_StringBuilderClassId() && b->cls == BO_StringBuilderClassId()) {
		const BO_StringBuilderRef sb1 = (BO_StringBuilderRef)a;
		const BO_StringBuilderRef sb2 = (BO_StringBuilderRef)b;

		if (sb1->length != sb2->length)
			return BC_false;

		return memcmp(sb1->buffer, sb2->buffer, sb1->length) == 0;
	}

	const BO_StringRef aStr = BO_ToString(a);
	const BO_StringRef bStr = BO_ToString(b);
	const BC_bool result = BO_Equal($OBJ aStr, $OBJ bStr);
	BO_Release($OBJ aStr);
	BO_Release($OBJ bStr);
	return result;
}

BO_StringRef IMPL_StringBuilderToString(const BO_ObjectRef obj) {
	const BO_StringBuilderRef builder = (BO_StringBuilderRef)obj;
	return BO_StringCreate("%.*s", (int)builder->length, builder->buffer);
}

BO_ObjectRef IMPL_StringBuilderCopy(const BO_ObjectRef obj) {
	const BO_StringBuilderRef original = (BO_StringBuilderRef)obj;
	const BO_StringBuilderRef copy = BO_StringBuilderCreateWithCapacity(BO_ObjectGetAllocator(obj), original->capacity);

	memcpy(copy->buffer, original->buffer, original->length);
	copy->length = original->length;

	return (BO_ObjectRef)copy;
}

// =========================================================
// MARK: Class
// =========================================================

static BF_Class kBO_StringBuilderClass = {
	.name = "BO_StringBuilder",
	.id = BF_CLASS_ID_INVALID,
	.dealloc = IMPL_StringBuilderDealloc,
	.hash = IMPL_StringBuilderHash,
	.equal = IMPL_StringBuilderEqual,
	.toString = IMPL_StringBuilderToString,
	.copy = IMPL_StringBuilderCopy,
	.allocSize = sizeof(BO_StringBuilder)
};

BF_ClassId BO_StringBuilderClassId() {
	return kBO_StringBuilderClass.id;
}

void INTERNAL_BO_StringBuilderInitialize(void) {
	BF_ClassRegistryInsert(&kBO_StringBuilderClass);
}

// =========================================================
// MARK: Creation
// =========================================================

BO_StringBuilderRef BO_StringBuilderCreate(const BC_AllocatorRef allocator) {
	return BO_StringBuilderCreateWithCapacity(allocator, BC_STRING_BUILDER_DEFAULT_CAPACITY);
}

BO_StringBuilderRef BO_StringBuilderCreateWithCapacity(const BC_AllocatorRef allocator, const size_t initialCapacity) {
	const size_t capacity = initialCapacity > 0 ? initialCapacity : BC_STRING_BUILDER_DEFAULT_CAPACITY;

	const BO_StringBuilderRef builder = (BO_StringBuilderRef)BO_ObjectAlloc(allocator, kBO_StringBuilderClass.id);

	builder->buffer = BC_AllocatorAlloc(allocator, capacity);
	builder->capacity = capacity;
	builder->length = 0;

	return builder;
}

// =========================================================
// MARK: Append Operations
// =========================================================

void BO_StringBuilderAppend(const BO_StringBuilderRef builder, const char* str) {
	if (!builder || !str) return;
	PRIV_AppendStr(builder, str, strlen(str));
}

void BO_StringBuilderAppendString(const BO_StringBuilderRef builder, const BO_StringRef str) {
	if (!builder || !str) return;
	PRIV_AppendStr(builder, BO_StringCPtr(str), BO_StringLength(str));
}

void BO_StringBuilderAppendChar(const BO_StringBuilderRef builder, const char c) {
	if (!builder) return;
	BO_StringBuilderEnsureCapacity(builder, builder->length + 1);
	builder->buffer[builder->length] = c;
	builder->length++;
}

void BO_StringBuilderAppendFormat(const BO_StringBuilderRef builder, const char* fmt, ...) {
	if (!builder || !fmt) return;

	va_list args, copy;
	va_start(args, fmt);
	va_copy(copy, args);

	const int len = vsnprintf(NULL, 0, fmt, copy);
	va_end(copy);

	if (len < 0) {
		va_end(args);
		return;
	}

	BO_StringBuilderEnsureCapacity(builder, builder->length + len + 1); // No way to avoid null terminator with vsnprintf

	vsnprintf(builder->buffer + builder->length, len + 1, fmt, args);
	builder->length += len; // don't count the null terminator as we don't care about it

	va_end(args);
}

// =========================================================
// MARK: Properties
// =========================================================

size_t BO_StringBuilderLength(const BO_StringBuilderRef builder) {
	return builder ? builder->length : 0;
}

size_t BO_StringBuilderCapacity(const BO_StringBuilderRef builder) {
	return builder ? builder->capacity : 0;
}

const char* BO_StringBuilderCPtr(const BO_StringBuilderRef builder) {
	if (!builder) return NULL;


	// Add null terminator for C-string compatibility but do not
	// count it in length, but we need to ensure capacity

	if (builder->capacity < builder->length + 1) {
		BO_StringBuilderEnsureCapacity(builder, builder->length + 1);
	}

	builder->buffer[builder->length] = '\0';

	return builder->buffer;
}

// =========================================================
// MARK: Methods
// =========================================================

void BO_StringBuilderEnsureCapacity(const BO_StringBuilderRef builder, const size_t requiredCapacity) {
	if (builder->capacity >= requiredCapacity) return;

	size_t newCapacity = builder->capacity;
	if (newCapacity < requiredCapacity) {
		newCapacity = PRIV_NextPowerOfTwo(requiredCapacity);
	}

	// Reallocate buffer
	const BC_AllocatorRef alloc = BO_ObjectGetAllocator((BO_ObjectRef)builder);
	char* newBuffer = alloc->alloc(newCapacity, alloc->context);
	if (!newBuffer) {
		fprintf(stderr, "BO_StringBuilder: Failed to allocate buffer\n");
		return;
	}
	memcpy(newBuffer, builder->buffer, builder->length);
	BC_AllocatorFree(alloc, builder->buffer);
	builder->buffer = newBuffer;
	builder->capacity = newCapacity;

	builder->buffer = newBuffer;
	builder->capacity = newCapacity;
}

void BO_StringBuilderClear(const BO_StringBuilderRef builder) {
	if (!builder)
		return;
	builder->length = 0;
}

BO_StringRef BO_StringBuilderFinish(const BO_StringBuilderRef builder, const BC_bool pooled) {
	if (!builder) return NULL;
	if (!pooled) {
		return BO_StringCreate("%.*s", (int)builder->length, builder->buffer);
	}
	return BO_StringPooledWithInfo(builder->buffer, builder->length, INTERNAL_BO_StringHasher(builder->buffer), BC_false);
}
