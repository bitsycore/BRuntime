#include "BCStringBuilder.h"

#include "BCObject.h"
#include "BCString.h"
#include "../Core/BCAllocator.h"
#include "../Core/BCClass.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =========================================================
// MARK: Struct
// =========================================================

typedef struct BCStringBuilder {
	BCObject base;
	size_t length;
	size_t capacity;
	char* buffer;
} BCStringBuilder;

// =========================================================
// MARK: Internal
// =========================================================

static inline size_t NextPowerOfTwo(size_t x) {
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

static inline void AppendStr(const BCStringBuilderRef builder, const char* str, const size_t strLen) {
	if (strLen == 0) return;
	BCStringBuilderEnsureCapacity(builder, builder->length + strLen);
	memcpy(builder->buffer + builder->length, str, strLen);
	builder->length += strLen;
}

// =========================================================
// MARK: Class Methods
// =========================================================

void StringBuilderDeallocImpl(const BCObjectRef obj) {
	const BCStringBuilderRef builder = (BCStringBuilderRef)obj;
	if (builder->buffer) {
		const BCAllocatorRef alloc = BCObjectGetAllocator(obj);
		alloc->free(builder->buffer, alloc->context);
		builder->buffer = NULL;
	}
}

uint32_t StringBuilderHashImpl(const BCObjectRef obj) {
	return ___BCINTERNAL___StringHasher(((BCStringBuilder*)obj)->buffer);
}

BC_bool StringBuilderEqualImpl(const BCObjectRef a, const BCObjectRef b) {
	if (a == b) return BC_true;

	if (a->cls == BCStringBuilderClassId() && b->cls == BCStringBuilderClassId()) {
		const BCStringBuilderRef sb1 = (BCStringBuilderRef)a;
		const BCStringBuilderRef sb2 = (BCStringBuilderRef)b;

		if (sb1->length != sb2->length)
			return BC_false;

		return memcmp(sb1->buffer, sb2->buffer, sb1->length) == 0;
	}

	const BCStringRef aStr = BCToString(a);
	const BCStringRef bStr = BCToString(b);
	const BC_bool result = BCEqual($OBJ aStr, $OBJ bStr);
	BCRelease($OBJ aStr);
	BCRelease($OBJ bStr);
	return result;
}

BCStringRef StringBuilderToStringImpl(const BCObjectRef obj) {
	const BCStringBuilderRef builder = (BCStringBuilderRef)obj;
	return BCStringCreate("%.*s", (int)builder->length, builder->buffer);
}

BCObjectRef StringBuilderCopyImpl(const BCObjectRef obj) {
	const BCStringBuilderRef original = (BCStringBuilderRef)obj;
	const BCStringBuilderRef copy = BCStringBuilderCreateWithCapacity(BCObjectGetAllocator(obj), original->capacity);

	memcpy(copy->buffer, original->buffer, original->length);
	copy->length = original->length;

	return (BCObjectRef)copy;
}

// =========================================================
// MARK: Class
// =========================================================

static BCClass kBCStringBuilderClass = {
	.name = "BCStringBuilder",
	.id = BC_CLASS_ID_INVALID,
	.dealloc = StringBuilderDeallocImpl,
	.hash = StringBuilderHashImpl,
	.equal = StringBuilderEqualImpl,
	.toString = StringBuilderToStringImpl,
	.copy = StringBuilderCopyImpl,
	.allocSize = sizeof(BCStringBuilder)
};

BCClassId BCStringBuilderClassId() {
	return kBCStringBuilderClass.id;
}

void ___BCINTERNAL___StringBuilderInitialize(void) {
	BCClassRegistryInsert(&kBCStringBuilderClass);
}

// =========================================================
// MARK: Creation
// =========================================================

BCStringBuilderRef BCStringBuilderCreate(const BCAllocatorRef allocator) {
	return BCStringBuilderCreateWithCapacity(allocator, BC_STRING_BUILDER_DEFAULT_CAPACITY);
}

BCStringBuilderRef BCStringBuilderCreateWithCapacity(const BCAllocatorRef allocator, const size_t initialCapacity) {
	const size_t capacity = initialCapacity > 0 ? initialCapacity : BC_STRING_BUILDER_DEFAULT_CAPACITY;

	const BCStringBuilderRef builder = (BCStringBuilderRef)BCObjectAlloc(allocator, kBCStringBuilderClass.id);

	builder->buffer = BCAllocatorAlloc(allocator, capacity);
	builder->capacity = capacity;
	builder->length = 0;

	return builder;
}

// =========================================================
// MARK: Append Operations
// =========================================================

void BCStringBuilderAppend(const BCStringBuilderRef builder, const char* str) {
	if (!builder || !str) return;
	AppendStr(builder, str, strlen(str));
}

void BCStringBuilderAppendString(const BCStringBuilderRef builder, const BCStringRef str) {
	if (!builder || !str) return;
	AppendStr(builder, BCStringCPtr(str), BCStringLength(str));
}

void BCStringBuilderAppendChar(const BCStringBuilderRef builder, const char c) {
	if (!builder) return;
	BCStringBuilderEnsureCapacity(builder, builder->length + 1);
	builder->buffer[builder->length] = c;
	builder->length++;
}

void BCStringBuilderAppendFormat(const BCStringBuilderRef builder, const char* fmt, ...) {
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

	BCStringBuilderEnsureCapacity(builder, builder->length + len + 1); // No way to avoid null terminator with vsnprintf

	vsnprintf(builder->buffer + builder->length, len + 1, fmt, args);
	builder->length += len; // don't count the null terminator as we don't care about it

	va_end(args);
}

// =========================================================
// MARK: Properties
// =========================================================

size_t BCStringBuilderLength(const BCStringBuilderRef builder) {
	return builder ? builder->length : 0;
}

size_t BCStringBuilderCapacity(const BCStringBuilderRef builder) {
	return builder ? builder->capacity : 0;
}

const char* BCStringBuilderCPtr(const BCStringBuilderRef builder) {
	if (!builder) return NULL;


	// Add null terminator for C-string compatibility but do not
	// count it in length, but we need to ensure capacity

	if (builder->capacity < builder->length + 1) {
		BCStringBuilderEnsureCapacity(builder, builder->length + 1);
	}

	builder->buffer[builder->length] = '\0';

	return builder->buffer;
}

// =========================================================
// MARK: Methods
// =========================================================

void BCStringBuilderEnsureCapacity(const BCStringBuilderRef builder, const size_t requiredCapacity) {
	if (builder->capacity >= requiredCapacity) return;

	size_t newCapacity = builder->capacity;
	if (newCapacity < requiredCapacity) {
		newCapacity = NextPowerOfTwo(requiredCapacity);
	}

	// Reallocate buffer
	const BCAllocatorRef alloc = BCObjectGetAllocator((BCObjectRef)builder);
	char* newBuffer = alloc->alloc(newCapacity, alloc->context);
	if (!newBuffer) {
		fprintf(stderr, "BCStringBuilder: Failed to allocate buffer\n");
		return;
	}
	memcpy(newBuffer, builder->buffer, builder->length);
	BCAllocatorFree(alloc, builder->buffer);
	builder->buffer = newBuffer;
	builder->capacity = newCapacity;

	builder->buffer = newBuffer;
	builder->capacity = newCapacity;
}

void BCStringBuilderClear(const BCStringBuilderRef builder) {
	if (!builder)
		return;
	builder->length = 0;
}

BCStringRef BCStringBuilderFinish(const BCStringBuilderRef builder) {
	if (!builder) return NULL;
	return BCStringCreate("%.*s", (int)builder->length, builder->buffer);
}
