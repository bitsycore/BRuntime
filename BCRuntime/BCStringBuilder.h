#ifndef BCRUNTIME_BCSTRINGBUILDER_H
#define BCRUNTIME_BCSTRINGBUILDER_H

#include "BCTypes.h"

#include <stddef.h>

// =========================================================
// MARK: Settings
// =========================================================

#define BC_STRING_BUILDER_DEFAULT_CAPACITY 64

// =========================================================
// MARK: Class
// =========================================================

extern const BCClassRef kBCStringBuilderClassRef;

// =========================================================
// MARK: Creation
// =========================================================

BCStringBuilderRef BCStringBuilderCreate(void);
BCStringBuilderRef BCStringBuilderCreateWithCapacity(size_t initialCapacity);

// =========================================================
// MARK: Append Operations
// =========================================================

void BCStringBuilderAppend(BCStringBuilderRef builder, const char* str);
void BCStringBuilderAppendString(BCStringBuilderRef builder, BCStringRef str);
void BCStringBuilderAppendChar(BCStringBuilderRef builder, char c);

__attribute__((format(printf, 2, 3)))
void BCStringBuilderAppendFormat(BCStringBuilderRef builder, const char* fmt, ...);

// =========================================================
// MARK: Properties
// =========================================================

size_t BCStringBuilderLength(BCStringBuilderRef builder);
size_t BCStringBuilderCapacity(BCStringBuilderRef builder);
const char* BCStringBuilderCPtr(BCStringBuilderRef builder);

// =========================================================
// MARK: Operations
// =========================================================

void BCStringBuilderClear(BCStringBuilderRef builder);
BCStringRef BCStringBuilderFinalize(BCStringBuilderRef builder);

#endif //BCRUNTIME_BCSTRINGBUILDER_H
