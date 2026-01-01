#include "BC_LazyTable.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "../Strings/BC_StringBuilder.h"

#define CG "\033[48;5;234m"
#define CB "\033[48;5;235m"
#define R  "\033[0m"
#define SB "\033[1m"

void BC_LazyTableInit(BC_LazyTable *t, const char *title, const size_t cols, ...) {
	t->title = title;
	t->cols = cols;
	t->widths = calloc(cols, sizeof(size_t));
	t->headers = malloc(cols * sizeof(const char *));

	t->row_count = 0;
	t->row_capacity = 16;
	t->rows = malloc(t->row_capacity * sizeof(char **));

	va_list args;
	va_start(args, cols);
	for (size_t i = 0; i < cols; i++) {
		t->headers[i] = va_arg(args, const char *);
		t->widths[i] = strlen(t->headers[i]);
	}
	va_end(args);
}

void BC_LazyTableAddRow(BC_LazyTable *t, ...) {
	// Double
	if (t->row_count >= t->row_capacity) {
		t->row_capacity *= 2;
		t->rows = realloc(t->rows, t->row_capacity * sizeof(char **));
	}

	t->rows[t->row_count] = malloc(t->cols * sizeof(char *));

	va_list args;
	va_start(args, t);
	for (size_t i = 0; i < t->cols; i++) {
		const char *val = va_arg(args, const char *);
		t->rows[t->row_count][i] = strdup(val);
		const size_t len = strlen(val);
		if (len > t->widths[i]) {
			t->widths[i] = len;
		}
	}
	t->row_count++;
	va_end(args);
}

size_t BC_LazyTableTotalWidth(const BC_LazyTable *t) {
	size_t total = 0;
	for (size_t i = 0; i < t->cols; i++) {
		total += t->widths[i] + 3;
	}
	return total;
}

char *BC_LazyTableToString(BC_LazyTable *t) {
	BC_StringBuilder sb;
	BC_StringBuilderInit(&sb);

	const size_t total_width = BC_LazyTableTotalWidth(t);
	const size_t padding = total_width < 78 ? (78 - total_width) / 2 : 0;

	// Title
	BC_StringBuilderRepeat(&sb, " ", padding);
	BC_StringBuilderAppend(&sb, SB "%s" R "\n", t->title);

	// Top border
	BC_StringBuilderAppend(&sb, "┌");
	for (size_t i = 0; i < t->cols; i++) {
		BC_StringBuilderRepeat(&sb, "─", t->widths[i] + 2);
		BC_StringBuilderAppend(&sb, i == t->cols - 1 ? "┐\n" : "┬");
	}

	// Header row
	BC_StringBuilderAppend(&sb, "│");
	for (size_t i = 0; i < t->cols; i++) {
		BC_StringBuilderAppend(&sb, " " CB SB "%-*s" R, (int) t->widths[i], t->headers[i]);
		BC_StringBuilderAppend(&sb, " │");
	}
	BC_StringBuilderAppend(&sb, "\n");

	// Separator
	BC_StringBuilderAppend(&sb, "├");
	for (size_t i = 0; i < t->cols; i++) {
		BC_StringBuilderAppend(&sb, CG);
		BC_StringBuilderRepeat(&sb, "─", t->widths[i] + 2);
		BC_StringBuilderAppend(&sb, R);
		BC_StringBuilderAppend(&sb, i == t->cols - 1 ? "┤\n" : CG "┼" R);
	}

	// Rows
	for (size_t r = 0; r < t->row_count; r++) {
		BC_StringBuilderAppend(&sb, "│");
		for (size_t i = 0; i < t->cols; i++) {
			BC_StringBuilderAppend(&sb, " " CB "%-*s" R " │", (int) t->widths[i], t->rows[r][i]);
		}
		BC_StringBuilderAppend(&sb, "\n");
	}

	// Footer
	BC_StringBuilderAppend(&sb, "└");
	for (size_t i = 0; i < t->cols; i++) {
		BC_StringBuilderRepeat(&sb, "─", t->widths[i] + 2);
		BC_StringBuilderAppend(&sb, i == t->cols - 1 ? "┘\n" : "┴");
	}

	return sb.data;
}

void BC_LazyTablePrint(BC_LazyTable *t) {
	char *output = BC_LazyTableToString(t);
	if (output) {
		fputs(output, stdout);
		free(output);
	}
}

void BC_LazyTableFree(const BC_LazyTable *t) {
	for (size_t r = 0; r < t->row_count; r++) {
		for (size_t i = 0; i < t->cols; i++) {
			free(t->rows[r][i]);
		}
		free(t->rows[r]);
	}
	free(t->rows);
	free(t->widths);
	free(t->headers);
}