#ifndef BFRAMEWORK_FORMAT_H
#define BFRAMEWORK_FORMAT_H

#include <stdio.h>

// =========================================================
// MARK: Formatted Output
// =========================================================

int BF_Print(const char* fmt, ...);
int BF_PrintFile(FILE* stream, const char* fmt, ...);
int BF_PrintString(char* str, size_t size, const char* fmt, ...);

// =========================================================
// MARK: VA_LIST
// =========================================================

int BF_PrintVa(const char* fmt, va_list args);
int BF_PrintFileVa(FILE* stream, const char* fmt, va_list args);
int BF_PrintStringVa(char* str, size_t size, const char* fmt, va_list args);

// =========================================================
// MARK: Core
// =========================================================

typedef void (*BF_FormatOutputFunc)(void* context, const char* data, size_t length);
int BF_Format(BF_FormatOutputFunc outFunc, void* context, const char* fmt, va_list args);

#endif //BFRAMEWORK_FORMAT_H
