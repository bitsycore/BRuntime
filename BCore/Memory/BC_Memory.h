#ifndef BCORE_MEMORY_H
#define BCORE_MEMORY_H

#include "../BC_Settings.h"

#include <stdio.h>

#if BC_SETTINGS_DEBUG_ALLOCATION_TRACK == 1

__attribute__((malloc, alloc_size(1)))
void* BC_Malloc(size_t size);

__attribute__((malloc, alloc_size(1,2)))
void* BC_Calloc(size_t count, size_t size);

__attribute__((alloc_size(2)))
void* BC_Realloc(void* ptr, size_t newSize);

void BC_Free(void* ptr);

char* BC_Strdup(const char* str);

#else

#include <stdlib.h>

#define BC_Malloc(_size_) malloc(_size_)
#define BC_Calloc(_count_, _size_) calloc(_count_, _size_)
#define BC_Realloc(_ptr_, _newSize_) realloc(_ptr_, _newSize_)
#define BC_Free(_ptr_) free(_ptr_)
#define BC_Strdup(_str_) strdup(_str_)

#endif

typedef struct BC_MemoryInfo {
	size_t system_current_rss;
	size_t system_peak_rss;
#if BC_SETTINGS_DEBUG_ALLOCATION_TRACK == 1
	size_t totalAllocated;
	size_t currentAllocUsage;
	size_t peakAllocUsage;
	size_t allocationCount;
	size_t freeCount;
#endif
} BC_MemoryInfo;

/**
 *
 * @param info A valid pointer to a BC_MemoryInfo struct to be filled with memory statistics.
 * @return -1 if info is NULL, 0 if system_ info is unavailable and 1 if successful.
 */
int BC_MemoryInfoGet(BC_MemoryInfo* info);
void BC_MemoryInfoPrint();
void BC_MemoryInfoHeapReset();

#endif //BCRUNTIME_BC_Memory_H