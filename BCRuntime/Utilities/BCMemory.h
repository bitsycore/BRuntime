#ifndef BCRUNTIME_BCMEMORY_H
#define BCRUNTIME_BCMEMORY_H

#include <stddef.h>

void BCFree(void* ptr);
__attribute__((malloc, alloc_size(1)))
void* BCMalloc(size_t size);
__attribute__((malloc, alloc_size(1,2)))
void* BCCalloc(size_t count, size_t size);
__attribute__((alloc_size(2)))
void* BCRealloc(void* ptr, size_t newSize);

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