#ifndef BCRUNTIME_BCSTRINGBUILDER_H
#define BCRUNTIME_BCSTRINGBUILDER_H

#include "../Core/BCTypes.h"

#include <stddef.h>

// =========================================================
// MARK: Settings
// =========================================================

#define BC_STRING_BUILDER_DEFAULT_CAPACITY 64

// =========================================================
// MARK: Class
// =========================================================

BCClassId BCStringBuilderClassId();

// =========================================================
// MARK: Constructors
// =========================================================

BCStringBuilderRef BCStringBuilderCreate(BCAllocatorRef allocator);
BCStringBuilderRef BCStringBuilderCreateWithCapacity(BCAllocatorRef allocator, size_t initialCapacity);

// =========================================================
// MARK: Appends
// =========================================================

void BCStringBuilderAppend(BCStringBuilderRef builder, const char* str);
void BCStringBuilderAppendString(BCStringBuilderRef builder, BCStringRef str);
void BCStringBuilderAppendChar(BCStringBuilderRef builder, char c);

__attribute__((format(printf, 2, 3)))
void BCStringBuilderAppendFormat(BCStringBuilderRef builder, const char* fmt, ...);

// =========================================================
// MARK: Getters
// =========================================================

size_t BCStringBuilderLength(BCStringBuilderRef builder);
size_t BCStringBuilderCapacity(BCStringBuilderRef builder);
const char* BCStringBuilderCPtr(BCStringBuilderRef builder);

// =========================================================
// MARK: Operations
// =========================================================

void BCStringBuilderEnsureCapacity(BCStringBuilderRef builder, size_t requiredCapacity);
void BCStringBuilderClear(BCStringBuilderRef builder);
BCStringRef BCStringBuilderFinish(BCStringBuilderRef builder, BC_bool pooled);

#endif //BCRUNTIME_BCSTRINGBUILDER_H
