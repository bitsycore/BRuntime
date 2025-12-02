#include "BCFormat.h"

#include "../Object/BCObject.h"
#include "../Object/BCString.h"
#include "../Utilities/BC_Memory.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

// =========================================================
// MARK: Core Parser
// =========================================================

static void AdvanceVAList(va_list* args, const char specifier, const char* lengthMod) {
	// Handle length modifiers
	if (lengthMod[0] == 'L') {
		// Long double
		if (specifier == 'f' || specifier == 'F' || specifier == 'e' ||
			specifier == 'E' || specifier == 'g' || specifier == 'G' ||
			specifier == 'a' || specifier == 'A') {
			va_arg(*args, long double);
			return;
		}
	}

	// Handle integer types with length modifiers
	if (specifier == 'd' || specifier == 'i' || specifier == 'u' ||
		specifier == 'o' || specifier == 'x' || specifier == 'X') {
		if (strcmp(lengthMod, "ll") == 0) {
			va_arg(*args, long long);
		}
		else if (strcmp(lengthMod, "l") == 0) {
			va_arg(*args, long);
		}
		else if (strcmp(lengthMod, "j") == 0) {
			va_arg(*args, intmax_t);
		}
		else if (strcmp(lengthMod, "z") == 0) {
			va_arg(*args, size_t);
		}
		else if (strcmp(lengthMod, "t") == 0) {
			va_arg(*args, ptrdiff_t);
		}
		else {
			// Default int (hh, h are promoted to int)
			va_arg(*args, int);
		}
		return;
	}

	// Handle floating point types
	if (specifier == 'f' || specifier == 'F' || specifier == 'e' ||
		specifier == 'E' || specifier == 'g' || specifier == 'G' ||
		specifier == 'a' || specifier == 'A') {
		va_arg(*args, double);
		return;
	}

	// Handle characters and strings
	if (specifier == 'c') {
		va_arg(*args, int); // char is promoted to int
		return;
	}
	if (specifier == 's') {
		va_arg(*args, char *);
		return;
	}
	if (specifier == 'p') {
		va_arg(*args, void *);
		return;
	}
	if (specifier == 'n') {
		va_arg(*args, int *);
	}
}

int BCFormat(const BCFormatOutputFunc outFunc, void* context, const char* fmt, va_list args) {
	if (!fmt)
		return 0;

	const char* cursor = fmt;
	int totalWritten = 0;

	while (*cursor) {
		if (*cursor != '%') {
			// Find next % or end
			const char* start = cursor;
			while (*cursor && *cursor != '%') {
				cursor++;
			}
			const size_t len = cursor - start;
			outFunc(context, start, len);
			totalWritten += (int)len;
			continue;
		}

		// Found '%'
		const char* specStart = cursor;

		// Capture args state before parsing specifier (for vsnprintf)
		va_list copy;
		va_copy(copy, args);

		cursor++; // Skip '%'

		// Check for literal %
		if (*cursor == '%') {
			outFunc(context, "%", 1);
			totalWritten++;
			cursor++;
			va_end(copy); // Clean up
			continue;
		}

		// Check for Custom Object Specifier %@
		if (*cursor == '@') {
			va_end(copy); // Clean up, not needed for %@
			const BCObjectRef obj = va_arg(args, BCObjectRef);
			const BCStringRef str = BCToString(obj);
			const char* cStr = BCStringCPtr(str);
			const size_t len = BCStringLength(str);
			outFunc(context, cStr, len);
			totalWritten += (int)len;
			BCRelease((BCObjectRef)str);
			cursor++;
			continue;
		}

		// Parse Standard Specifier
		// Flags
		while (*cursor == '-' || *cursor == '+' || *cursor == ' ' ||
			*cursor == '#' || *cursor == '0') {
			cursor++;
		}

		// Width
		if (*cursor == '*') {
			cursor++;
			va_arg(args, int); // Consume width arg
		}
		else {
			while (*cursor >= '0' && *cursor <= '9')
				cursor++;
		}

		// Precision
		if (*cursor == '.') {
			cursor++;
			if (*cursor == '*') {
				cursor++;
				va_arg(args, int); // Consume precision arg
			}
			else {
				while (*cursor >= '0' && *cursor <= '9')
					cursor++;
			}
		}

		// Length Modifier
		char lengthMod[3] = {0};
		if (*cursor == 'h') {
			lengthMod[0] = 'h';
			cursor++;
			if (*cursor == 'h') {
				lengthMod[1] = 'h';
				cursor++;
			}
		}
		else if (*cursor == 'l') {
			lengthMod[0] = 'l';
			cursor++;
			if (*cursor == 'l') {
				lengthMod[1] = 'l';
				cursor++;
			}
		}
		else if (*cursor == 'j') {
			lengthMod[0] = 'j';
			cursor++;
		}
		else if (*cursor == 'z') {
			lengthMod[0] = 'z';
			cursor++;
		}
		else if (*cursor == 't') {
			lengthMod[0] = 't';
			cursor++;
		}
		else if (*cursor == 'L') {
			lengthMod[0] = 'L';
			cursor++;
		}

		// Specifier
		const char specifier = *cursor;
		cursor++;

		// Construct specifier string
		const size_t specLen = cursor - specStart;
		char* specBuf = BCMalloc(specLen + 1);
		memcpy(specBuf, specStart, specLen);
		specBuf[specLen] = '\0';

		// Format using vsnprintf
		// Use 'copy' which points to start of args for this specifier

		// First determine size
		va_list copy2;
		va_copy(copy2, copy);
		const int needed = vsnprintf(NULL, 0, specBuf, copy2);
		va_end(copy2);

		if (needed >= 0) {
			char* outBuf = BCMalloc(needed + 1);
			// Use copy again
			vsnprintf(outBuf, needed + 1, specBuf, copy);
			outFunc(context, outBuf, needed);
			totalWritten += needed;
			BCFree(outBuf);
		}

		BCFree(specBuf);
		va_end(copy);

		// Advance args
		AdvanceVAList(&args, specifier, lengthMod);
	}

	return totalWritten;
}

// =========================================================
// MARK: Output Helpers
// =========================================================

typedef struct {
	char* buffer;
	size_t capacity;
	size_t current;
} StringContext;

static void StringOutput(void* context, const char* data, const size_t length) {
	StringContext* ctx = context;
	if (ctx->current + length < ctx->capacity) {
		memcpy(ctx->buffer + ctx->current, data, length);
		ctx->current += length;
		ctx->buffer[ctx->current] = '\0';
	}
	else if (ctx->capacity > 0) {
		// Truncate
		const size_t available = ctx->capacity - ctx->current - 1;
		if (available > 0) {
			memcpy(ctx->buffer + ctx->current, data, available);
			ctx->current += available;
			ctx->buffer[ctx->current] = '\0';
		}
	}
}

static void FileOutput(void* context, const char* data, const size_t length) {
	fwrite(data, 1, length, context);
}

// =========================================================
// MARK: Implementations
// =========================================================

int BC_vprintf(const char* fmt, const va_list args) {
	return BCFormat(FileOutput, stdout, fmt, args);
}

int BC_vfprintf(FILE* stream, const char* fmt, const va_list args) {
	return BCFormat(FileOutput, stream, fmt, args);
}

int BC_vsnprintf(char* str, const size_t size, const char* fmt, const va_list args) {
	StringContext ctx = {str, size, 0};
	return BCFormat(StringOutput, &ctx, fmt, args);
}

int BC_printf(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	const int ret = BC_vprintf(fmt, args);
	va_end(args);
	return ret;
}

int BC_fprintf(FILE* stream, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	const int ret = BC_vfprintf(stream, fmt, args);
	va_end(args);
	return ret;
}

int BC_snprintf(char* str, const size_t size, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	const int ret = BC_vsnprintf(str, size, fmt, args);
	va_end(args);
	return ret;
}
