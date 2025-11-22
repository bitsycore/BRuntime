#ifndef BCRUNTIME_BCMEMINFO_H
#define BCRUNTIME_BCMEMINFO_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
	size_t current_rss;
	size_t peak_rss;
} BCMemoryInfo;

bool BCMemoryInfoGet(BCMemoryInfo* info);
void BCMemoryInfoPrint();

#endif //BCRUNTIME_BCMEMINFO_H