#ifndef BCRUNTIME_BC_STRBUFF_H
#define BCRUNTIME_BC_STRBUFF_H

#include <stdio.h>

typedef struct {
	char *data;
	size_t len;
	size_t cap;
} BC_StrBuilder;

void BC_StrBuilder_init(BC_StrBuilder *sb);
void BC_StrBuilder_append(BC_StrBuilder *sb, const char *fmt, ...);
void BC_StrBuilder_repeat(BC_StrBuilder *sb, const char *str, size_t count);
void BC_StrBuilder_free(const BC_StrBuilder *sb);

#endif //BCRUNTIME_BC_STRBUFF_H