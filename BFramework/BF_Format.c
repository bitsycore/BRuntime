#include "BF_Format.h"

#include "BCore/Memory/BC_Memory.h"

#include "Object/BO_Object.h"
#include "Object/BO_String.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

// =========================================================
// MARK: Core Parser
// =========================================================

static void VaListNext(va_list* args, const char specifier, const char* lengthMod) {

	// =========================================================
	// LENGTH MODIFIER
	if (lengthMod[0] == 'L') {
		if (
			specifier == 'f'
			|| specifier == 'F'
			|| specifier == 'e'
			|| specifier == 'E'
			|| specifier == 'g'
			|| specifier == 'G'
			|| specifier == 'a'
			|| specifier == 'A'
		) {
			va_arg(*args, long double);
			return;
		}
	}

	// =========================================================
	// INTEGER TYPES
	if (
		specifier == 'd'
		|| specifier == 'i'
		|| specifier == 'u'
		|| specifier == 'o'
		|| specifier == 'x'
		|| specifier == 'X'
	) {
		if (strcmp(lengthMod, "ll") == 0) { va_arg(*args, long long); }
		else if (strcmp(lengthMod, "l") == 0) { va_arg(*args, long); }
		else if (strcmp(lengthMod, "j") == 0) { va_arg(*args, intmax_t); }
		else if (strcmp(lengthMod, "z") == 0) { va_arg(*args, size_t); }
		else if (strcmp(lengthMod, "t") == 0) { va_arg(*args, ptrdiff_t); }
		else { va_arg(*args, int); }
		return;
	}

	// =========================================================
	// FLOATING POINT TYPE
	if (
		specifier == 'f'
		|| specifier == 'F'
		|| specifier == 'e'
		|| specifier == 'E'
		|| specifier == 'g'
		|| specifier == 'G'
		|| specifier == 'a'
		|| specifier == 'A'
	) {
		va_arg(*args, double);
		return;
	}

	// =========================================================
	// CHAR & STRING
	if (specifier == 'c') { va_arg(*args, int); return; }
	if (specifier == 's') { va_arg(*args, char *); return; }
	if (specifier == 'p') { va_arg(*args, void *); return; }
	if (specifier == 'n') { va_arg(*args, int *); }

}

// =========================================================
// MARK: String Output
// =========================================================

typedef struct StringContext {
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

// =========================================================
// MARK: BufferedFile Output
// =========================================================

#define BUFFERED_FILE_BUFFER_SIZE 8192

typedef struct BufferedFileContext {
	FILE* stream;
	char buffer[BUFFERED_FILE_BUFFER_SIZE];
	size_t used;
} BufferedFileContext;

static void FlushBufferedFile(BufferedFileContext* ctx) {
	if (ctx->used > 0) {
		fwrite(ctx->buffer, 1, ctx->used, ctx->stream);
		ctx->used = 0;
	}
}

static void BufferedFileOutput(void* context, const char* data, const size_t length) {
	BufferedFileContext* ctx = context;

	// ========================================================
	// DATA LARGER THAN BUFFER SIZE
	if (length >= BUFFERED_FILE_BUFFER_SIZE) {
		FlushBufferedFile(ctx);
		fwrite(data, 1, length, ctx->stream);
		return;
	}

	// ========================================================
	// NOT ENOUGH SPACE IN BUFFER SO FLUSH IT
	if (ctx->used + length > BUFFERED_FILE_BUFFER_SIZE) { FlushBufferedFile(ctx); }

	// ========================================================
	// COPY TO BUFFER
	memcpy(ctx->buffer + ctx->used, data, length);
	ctx->used += length;
}

// =========================================================
// MARK: Public
// =========================================================

int BFFormat(const BFFormatOutputFunc outFunc, void* context, const char* fmt, va_list args) {
	if (!fmt) return 0;

	const char* cursor = fmt;
	int totalWritten = 0;

	while (*cursor) {

		// =========================================================
		// FIND NEXT %
		if (*cursor != '%') {

			const char* start = cursor;
			while (*cursor && *cursor != '%') {
				cursor++;
			}
			const size_t len = cursor - start;
			outFunc(context, start, len);
			totalWritten += (int)len;
			continue;
		}

		// =========================================================
		// FOUND % THEN SKIP IT
		const char* specStart = cursor;
		cursor++;

		// =========================================================
		// LITERAL %%
		if (*cursor == '%') {
			outFunc(context, "%", 1);
			totalWritten++;
			cursor++;
			continue;
		}

		// =========================================================
		// CAPTURE ARGS STATE BEFORE CONSUMING IT (for vsnprintf)
		va_list copy;
		va_copy(copy, args);

		// =========================================================
		// BO_Object %@
		if (*cursor == '@') {
			const BO_ObjectRef obj = va_arg(args, BO_ObjectRef);
			const BO_StringRef str = BO_ToString(obj);
			const char* cStr = BO_StringCPtr(str);
			const size_t len = BO_StringLength(str);
			outFunc(context, cStr, len);
			totalWritten += (int)len;
			BO_Release((BO_ObjectRef)str);
			cursor++;
			continue;
		}

		// =========================================================
		// FLAGS SPECIFIER
		while (
			*cursor == '-'
			|| *cursor == '+'
			|| *cursor == ' '
			|| *cursor == '#'
			|| *cursor == '0'
		) {
			cursor++;
		}

		// =========================================================
		// WIDTH
		if (*cursor == '*') {
			cursor++;
			// Consume width
			va_arg(args, int);
		}
		else {
			while (*cursor >= '0' && *cursor <= '9') cursor++;
		}

		// =========================================================
		// PRECISION
		if (*cursor == '.') {
			cursor++;
			if (*cursor == '*') {
				cursor++;
				// Consume precision
				va_arg(args, int);
			}
			else {
				while (*cursor >= '0' && *cursor <= '9')
					cursor++;
			}
		}

		// =========================================================
		// LENGTH
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

		// =========================================================
		// Specifier
		const char specifier = *cursor;
		cursor++;

		// =========================================================
		// Buffer Stack or Heap for format specifier
		const size_t specLen = cursor - specStart;
		char specStackBuf[64];
		char* specBuf;

		if (specLen < sizeof(specStackBuf)) {
			specBuf = specStackBuf;
		} else {
			specBuf = BCMalloc(specLen + 1);
		}

		memcpy(specBuf, specStart, specLen);
		specBuf[specLen] = '\0';

		// ========================================================
		// Format using vsnprintf
		// Use 'copy' which points to start of args for this specifier
		va_list copy2;
		va_copy(copy2, copy);
		// Size Calculation
		const int needed = vsnprintf(NULL, 0, specBuf, copy2);
		va_end(copy2);

		if (needed >= 0) {

			// ======================================
			// Buffer Stack or Heap
			char outStackBuf[512];
			char* outBuf;
			if (needed < sizeof(outStackBuf)) {
				outBuf = outStackBuf;
			} else {
				outBuf = BCMalloc(needed + 1);
			}

			// Use copy again
			vsnprintf(outBuf, needed + 1, specBuf, copy);
			outFunc(context, outBuf, needed);
			totalWritten += needed;

			// ========================================================
			// CLEANUP
			if (outBuf != outStackBuf) { BCFree(outBuf); }
		}


		// ========================================================
		// CLEANUP
		if (specBuf != specStackBuf) { BCFree(specBuf); } // Free only if heap allocated
		va_end(copy);

		// ========================================================
		// Advance args
		VaListNext(&args, specifier, lengthMod);
	}

	return totalWritten;
}

int BF_vprintf(const char* fmt, va_list args) {
	BufferedFileContext ctx = {stdout, {0}, 0};
	const int result = BFFormat(BufferedFileOutput, &ctx, fmt, args);
	FlushBufferedFile(&ctx);
	return result;
}

int BF_vfprintf(FILE* stream, const char* fmt, va_list args) {
	BufferedFileContext ctx = {stream, {0}, 0};
	const int result = BFFormat(BufferedFileOutput, &ctx, fmt, args);
	FlushBufferedFile(&ctx);
	return result;
}

int BF_vsnprintf(char* str, const size_t size, const char* fmt, va_list args) {
	StringContext ctx = {str, size, 0};
	return BFFormat(StringOutput, &ctx, fmt, args);
}

int BF_printf(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	const int ret = BF_vprintf(fmt, args);
	va_end(args);
	return ret;
}

int BF_fprintf(FILE* stream, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	const int ret = BF_vfprintf(stream, fmt, args);
	va_end(args);
	return ret;
}

int BF_snprintf(char* str, const size_t size, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	const int ret = BF_vsnprintf(str, size, fmt, args);
	va_end(args);
	return ret;
}
