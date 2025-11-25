#include "BCStringBuilder.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BCClass.h"
#include "BCObject.h"
#include "BCString.h"
#include "Utilities/BCMemory.h"

// =========================================================
// MARK: Struct
// =========================================================

typedef struct BCStringBuilder {
	BCObject base;
	char* buffer;
	size_t length;
	size_t capacity;
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
	x |= x >> 32; // for 64-bit size_t
#endif
	x++;
	return x;
}

static void EnsureCapacity(const BCStringBuilderRef builder, const size_t requiredCapacity) {
	if (builder->capacity >= requiredCapacity)
		return;

	// Geometric growth: double until we exceed required capacity
	size_t newCapacity = builder->capacity;
	if (newCapacity < requiredCapacity) {
		newCapacity = NextPowerOfTwo(requiredCapacity);
	}

	char* newBuffer = (char*)BCRealloc(builder->buffer, newCapacity);
	if (!newBuffer) {
		fprintf(stderr, "BCStringBuilder: Failed to reallocate buffer\n");
		return;
	}

	builder->buffer = newBuffer;
	builder->capacity = newCapacity;
}

// =========================================================
// MARK: Class Methods
// =========================================================

void StringBuilderDeallocImpl(const BCObjectRef obj) {
	const BCStringBuilderRef builder = (BCStringBuilderRef)obj;
	if (builder->buffer) {
		BCFree(builder->buffer);
		builder->buffer = NULL;
	}
}

uint32_t StringBuilderHashImpl(const BCObjectRef obj) {
	return ___BCINTERNAL___StringHasher(((BCStringBuilder*)obj)->buffer);
}

BC_bool StringBuilderEqualImpl(const BCObjectRef a, const BCObjectRef b) {
	if (a == b)
		return BC_true;

	const BCStringBuilderRef sb1 = (BCStringBuilderRef)a;
	const BCStringBuilderRef sb2 = (BCStringBuilderRef)b;

	if (sb1->length != sb2->length)
		return BC_false;

	return memcmp(sb1->buffer, sb2->buffer, sb1->length) == 0;
}

BCStringRef StringBuilderToStringImpl(const BCObjectRef obj) {
	const BCStringBuilderRef builder = (BCStringBuilderRef)obj;
	return BCStringCreate("%.*s", (int)builder->length, builder->buffer);
}

BCObjectRef StringBuilderCopyImpl(const BCObjectRef obj) {
	const BCStringBuilderRef original = (BCStringBuilderRef)obj;
	const BCStringBuilderRef copy =
		BCStringBuilderCreateWithCapacity(original->capacity);

	memcpy(copy->buffer, original->buffer, original->length);
	copy->length = original->length;

	return (BCObjectRef)copy;
}

// =========================================================
// MARK: Class
// =========================================================

static const BCClass kBCStringBuilderClass = {
	.name = "BCStringBuilder",
	.dealloc = StringBuilderDeallocImpl,
	.hash = StringBuilderHashImpl,
	.equal = StringBuilderEqualImpl,
	.toString = StringBuilderToStringImpl,
	.copy = StringBuilderCopyImpl,
	.allocSize = sizeof(BCStringBuilder)
};

const BCClassRef kBCStringBuilderClassRef = (BCClassRef)&kBCStringBuilderClass;

// =========================================================
// MARK: Creation
// =========================================================

BCStringBuilderRef BCStringBuilderCreate(void) {
	return BCStringBuilderCreateWithCapacity(BC_STRING_BUILDER_DEFAULT_CAPACITY);
}

BCStringBuilderRef BCStringBuilderCreateWithCapacity(const size_t initialCapacity) {
	const size_t capacity = initialCapacity > 0 ? initialCapacity : BC_STRING_BUILDER_DEFAULT_CAPACITY;

	const BCStringBuilderRef builder = (BCStringBuilderRef)BCObjectAlloc((BCClassRef)&kBCStringBuilderClass, NULL);

	builder->buffer = (char*)BCMalloc(capacity);
	builder->capacity = capacity;
	builder->length = 0;
	builder->buffer[0] = '\0';

	return builder;
}

// =========================================================
// MARK: Append Operations
// =========================================================

void BCStringBuilderAppend(const BCStringBuilderRef builder, const char* str) {
	if (!builder || !str)
		return;

	const size_t strLen = strlen(str);
	if (strLen == 0)
		return;

	const size_t requiredCapacity =
		builder->length + strLen + 1; // +1 for null terminator
	EnsureCapacity(builder, requiredCapacity);

	memcpy(builder->buffer + builder->length, str, strLen);
	builder->length += strLen;
	builder->buffer[builder->length] = '\0';
}

void BCStringBuilderAppendString(const BCStringBuilderRef builder, const BCStringRef str) {
	if (!builder || !str)
		return;
	BCStringBuilderAppend(builder, BCStringCPtr(str));
}

void BCStringBuilderAppendChar(const BCStringBuilderRef builder, const char c) {
	if (!builder)
		return;

	const size_t requiredCapacity = builder->length + 2; // +1 for char, +1 for null terminator
	EnsureCapacity(builder, requiredCapacity);

	builder->buffer[builder->length] = c;
	builder->length++;
	builder->buffer[builder->length] = '\0';
}

void BCStringBuilderAppendFormat(const BCStringBuilderRef builder, const char* fmt, ...) {
	if (!builder || !fmt)
		return;

	va_list args, copy;
	va_start(args, fmt);
	va_copy(copy, args);

	const int len = vsnprintf(NULL, 0, fmt, copy);
	va_end(copy);

	if (len < 0) {
		va_end(args);
		return;
	}

	const size_t requiredCapacity = builder->length + len + 1;
	EnsureCapacity(builder, requiredCapacity);

	vsnprintf(builder->buffer + builder->length, len + 1, fmt, args);
	builder->length += len;

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
	return builder ? builder->buffer : NULL;
}

// =========================================================
// MARK: Operations
// =========================================================

void BCStringBuilderClear(const BCStringBuilderRef builder) {
	if (!builder)
		return;

	builder->length = 0;
	if (builder->buffer) {
		builder->buffer[0] = '\0';
	}
}

BCStringRef BCStringBuilderFinalize(const BCStringBuilderRef builder) {
	if (!builder)
		return NULL;

	// Create a new BCString with the current content
	const BCStringRef result = BCStringCreate("%.*s", (int)builder->length, builder->buffer);

	// Keep the builder usable - don't clear it
	// Users can call BCStringBuilderClear explicitly if needed

	return result;
}
