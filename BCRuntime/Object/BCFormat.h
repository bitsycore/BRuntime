#ifndef BCRUNTIME_BCFORMAT_H
#define BCRUNTIME_BCFORMAT_H

#include "../Core/BCTypes.h"
#include <stdarg.h>
#include <stdio.h>

// =========================================================
// MARK: Formatted Output
// =========================================================

/**
 * Prints formatted output to stdout.
 * Supports standard printf specifiers and %@ for BCObject.
 */
int BCPrintf(const char* fmt, ...);

/**
 * Prints formatted output to a file stream.
 * Supports standard printf specifiers and %@ for BCObject.
 */
int BCFprintf(FILE* stream, const char* fmt, ...);

/**
 * Writes formatted output to a string buffer.
 * Supports standard printf specifiers and %@ for BCObject.
 * Note: Caller must ensure buffer is large enough.
 */
int BCSprintf(char* str, const char* fmt, ...);

/**
 * Writes formatted output to a string buffer with size limit.
 * Supports standard printf specifiers and %@ for BCObject.
 */
int BCSnprintf(char* str, size_t size, const char* fmt, ...);

// =========================================================
// MARK: VA_LIST Variants
// =========================================================

int BCVPrintf(const char* fmt, va_list args);
int BCVFprintf(FILE* stream, const char* fmt, va_list args);
int BCVSprintf(char* str, const char* fmt, va_list args);
int BCVSnprintf(char* str, size_t size, const char* fmt, va_list args);

// =========================================================
// MARK: Core
// =========================================================

typedef void (*BCFormatOutputFunc)(void* context, const char* data, size_t length);

/**
 * Core formatting function.
 * Parses the format string and calls the output function for chunks of data.
 * Returns the total number of characters written.
 */
int BCFormat(BCFormatOutputFunc outFunc, void* context, const char* fmt, va_list args);

#endif // BCRUNTIME_BCFORMAT_H
