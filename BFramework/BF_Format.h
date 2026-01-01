#ifndef BFRAMEWORK_FORMAT_H
#define BFRAMEWORK_FORMAT_H

#include <stdio.h>

// =========================================================
// MARK: Formatted Output
// =========================================================

int BF_printf(const char* fmt, ...);
int BF_fprintf(FILE* stream, const char* fmt, ...);
int BF_snprintf(char* str, size_t size, const char* fmt, ...);

// =========================================================
// MARK: VA_LIST
// =========================================================

int BF_vprintf(const char* fmt, va_list args);
int BF_vfprintf(FILE* stream, const char* fmt, va_list args);
int BF_vsnprintf(char* str, size_t size, const char* fmt, va_list args);

// =========================================================
// MARK: Core
// =========================================================

typedef void (*BFFormatOutputFunc)(void* context, const char* data, size_t length);
int BFFormat(BFFormatOutputFunc outFunc, void* context, const char* fmt, va_list args);

#endif //BFRAMEWORK_FORMAT_H
