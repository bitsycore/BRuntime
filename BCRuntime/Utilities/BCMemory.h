#ifndef BCRUNTIME_BCMEMORY_H
#define BCRUNTIME_BCMEMORY_H

#include <stddef.h>

void* BCMalloc(size_t size);
void* BCCalloc(size_t count, size_t size);
void* BCRealloc(void* ptr, size_t newSize);
void BCFree(void* ptr);

typedef struct {
	size_t system_current_rss;
	size_t system_peak_rss;
	size_t totalAllocated;
	size_t currentAllocUsage;
	size_t peakAllocUsage;
	size_t allocationCount;
	size_t freeCount;
} BCMemoryInfo;

int BCMemoryInfoGet(BCMemoryInfo* info);
void BCMemoryInfoPrint();
void BCMemoryInfoHeapReset();

#endif //BCRUNTIME_BCMEMORY_H