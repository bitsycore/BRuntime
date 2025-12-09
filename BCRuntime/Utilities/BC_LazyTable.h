#ifndef BCRUNTIME_BC_TERMTABS_H
#define BCRUNTIME_BC_TERMTABS_H
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

void BC_LazyTable_init(BC_LazyTable* t, const char* title, size_t cols, ...);
void BC_LazyTable_addRow(BC_LazyTable* t, ...);
size_t BC_LazyTable_totalWidth(const BC_LazyTable* t);
char* BC_LazyTable_toString(BC_LazyTable* t);
void BC_LazyTable_print(BC_LazyTable* t);
void BC_LazyTable_free(const BC_LazyTable* t);

#endif //BCRUNTIME_BC_TERMTABS_H