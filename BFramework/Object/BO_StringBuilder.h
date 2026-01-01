#ifndef BCRUNTIME_BCSTRINGBUILDER_H
#define BCRUNTIME_BCSTRINGBUILDER_H

#include "../BF_Types.h"

#include <stddef.h>

// =========================================================
// MARK: Settings
// =========================================================

#define BC_STRING_BUILDER_DEFAULT_CAPACITY 128

// =========================================================
// MARK: Class
// =========================================================

BF_ClassId BO_StringBuilderClassId();

// =========================================================
// MARK: Constructors
// =========================================================

BO_StringBuilderRef BO_StringBuilderCreate(BC_AllocatorRef allocator);
BO_StringBuilderRef BO_StringBuilderCreateWithCapacity(BC_AllocatorRef allocator, size_t initialCapacity);

// =========================================================
// MARK: Appends
// =========================================================

void BO_StringBuilderAppend(BO_StringBuilderRef builder, const char* str);
void BO_StringBuilderAppendString(BO_StringBuilderRef builder, BO_StringRef str);
void BO_StringBuilderAppendChar(BO_StringBuilderRef builder, char c);

__attribute__((format(printf, 2, 3)))
void BO_StringBuilderAppendFormat(BO_StringBuilderRef builder, const char* fmt, ...);

// =========================================================
// MARK: Getters
// =========================================================

size_t BO_StringBuilderLength(BO_StringBuilderRef builder);
size_t BO_StringBuilderCapacity(BO_StringBuilderRef builder);
const char* BO_StringBuilderCPtr(BO_StringBuilderRef builder);

// =========================================================
// MARK: Operations
// =========================================================

void BO_StringBuilderEnsureCapacity(BO_StringBuilderRef builder, size_t requiredCapacity);
void BO_StringBuilderClear(BO_StringBuilderRef builder);
BO_StringRef BO_StringBuilderFinish(BO_StringBuilderRef builder, BC_bool pooled);

#endif //BCRUNTIME_BCSTRINGBUILDER_H
