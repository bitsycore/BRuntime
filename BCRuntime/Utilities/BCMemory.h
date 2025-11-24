#ifndef BCRUNTIME_BCMEMORY_H
#define BCRUNTIME_BCMEMORY_H

#include "../BCSettings.h"

#include <stdio.h>

#if BC_SETTINGS_DEBUG_ALLOCATION_TRACK == 1

void BCFree(void* ptr);

__attribute__((malloc, alloc_size(1)))
void* BCMalloc(size_t size);

__attribute__((malloc, alloc_size(1,2)))
void* BCCalloc(size_t count, size_t size);

__attribute__((alloc_size(2)))
void* BCRealloc(void* ptr, size_t newSize);

#else

#include <stdlib.h>

#define BCMalloc(_size_) malloc(_size_)
#define BCCalloc(_count_, _size_) calloc(_count_, _size_)
#define BCRealloc(_ptr_, _newSize_) realloc(_ptr_, _newSize_)
#define BCFree(_ptr_) free(_ptr_)

#endif

typedef struct BCMemoryInfo {
	size_t system_current_rss;
	size_t system_peak_rss;
#if BC_SETTINGS_DEBUG_ALLOCATION_TRACK == 1
	size_t totalAllocated;
	size_t currentAllocUsage;
	size_t peakAllocUsage;
	size_t allocationCount;
	size_t freeCount;
#endif
} BCMemoryInfo;

/**
 *
 * @param info A valid pointer to a BCMemoryInfo struct to be filled with memory statistics.
 * @return -1 if info is NULL, 0 if system_ info is unavailable and 1 if successful.
 */
int BCMemoryInfoGet(BCMemoryInfo* info);
void BCMemoryInfoPrint();
void BCMemoryInfoHeapReset();

#endif //BCRUNTIME_BCMEMORY_H