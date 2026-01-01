#ifndef BCORE_LAZY_TABLE_H
#define BCORE_LAZY_TABLE_H

#include <stdio.h>

typedef struct BC_LazyTable {
	const char *title;
	size_t cols;
	size_t *widths;
	const char **headers;
	char ***rows;
	size_t row_count;
	size_t row_capacity;
} BC_LazyTable;

void BC_LazyTableInit(BC_LazyTable* t, const char* title, size_t cols, ...);
void BC_LazyTableAddRow(BC_LazyTable* t, ...);
size_t BC_LazyTableTotalWidth(const BC_LazyTable* t);
char* BC_LazyTableToString(BC_LazyTable* t);
void BC_LazyTablePrint(BC_LazyTable* t);
void BC_LazyTableFree(const BC_LazyTable* t);

#endif //BCORE_LAZY_TABLE_H