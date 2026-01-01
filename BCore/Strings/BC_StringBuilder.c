#include "BC_StringBuilder.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Memory/BC_Memory.h"

void BC_StringBuilderInit(BC_StringBuilder *sb) {
	sb->cap = 1024;
	sb->len = 0;
	sb->data = BC_Malloc(sb->cap);
	sb->data[0] = '\0';
}

void BC_StringBuilderAppend(BC_StringBuilder *sb, const char *fmt, ...) {
	va_list args;

	va_start(args, fmt);
	const int required = vsnprintf(NULL, 0, fmt, args);
	va_end(args);

	if (required < 0) return;

	if (sb->len + required + 1 > sb->cap) {
		while (sb->len + required + 1 > sb->cap) {
			sb->cap *= 2;
		}
		sb->data = BC_Realloc(sb->data, sb->cap);
	}

	// 3. Write data
	va_start(args, fmt);
	vsnprintf(sb->data + sb->len, required + 1, fmt, args);
	va_end(args);

	sb->len += required;
}

void BC_StringBuilderRepeat(BC_StringBuilder *sb, const char *str, const size_t count) {
	size_t s_len = strlen(str);
	const size_t total_len = s_len * count;

	if (sb->len + total_len + 1 > sb->cap) {
		while (sb->len + total_len + 1 > sb->cap) sb->cap *= 2;
		sb->data = BC_Realloc(sb->data, sb->cap);
	}

	for (size_t i = 0; i < count; i++) {
		memcpy(sb->data + sb->len, str, s_len);
		sb->len += s_len;
	}
	sb->data[sb->len] = '\0';
}

void BC_StringBuilderFree(const BC_StringBuilder *sb) {
	BC_Free(sb->data);
}