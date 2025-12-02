#ifndef BCRUNTIME_BCFORMAT_H
#define BCRUNTIME_BCFORMAT_H

#include <stdio.h>

// =========================================================
// MARK: Formatted Output
// =========================================================

int BC_printf(const char* fmt, ...);
int BC_fprintf(FILE* stream, const char* fmt, ...);
int BC_snprintf(char* str, size_t size, const char* fmt, ...);

// =========================================================
// MARK: VA_LIST
// =========================================================

int BC_vprintf(const char* fmt, va_list args);
int BC_vfprintf(FILE* stream, const char* fmt, va_list args);
int BC_vsnprintf(char* str, size_t size, const char* fmt, va_list args);

// =========================================================
// MARK: Core
// =========================================================

typedef void (*BCFormatOutputFunc)(void* context, const char* data, size_t length);
int BCFormat(BCFormatOutputFunc outFunc, void* context, const char* fmt, va_list args);

#endif // BCRUNTIME_BCFORMAT_H
