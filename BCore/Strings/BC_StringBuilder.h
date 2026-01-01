#ifndef BCORE_STRING_BUILDER_H
#define BCORE_STRING_BUILDER_H

#include <stdio.h>

typedef struct BC_StringBuilder {
	char *data;
	size_t len;
	size_t cap;
} BC_StringBuilder;

void BC_StringBuilderInit(BC_StringBuilder *sb);
void BC_StringBuilderAppend(BC_StringBuilder *sb, const char *fmt, ...);
void BC_StringBuilderRepeat(BC_StringBuilder *sb, const char *str, size_t count);
void BC_StringBuilderFree(const BC_StringBuilder *sb);

#endif //BCORE_STRING_BUILDER_H