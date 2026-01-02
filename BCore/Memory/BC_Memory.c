#include "BC_Memory.h"

#include "../Thread/BC_Threads.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if BC_SETTINGS_DEBUG_ALLOCATION_TRACK == 1

typedef struct {
    size_t totalAllocated;
    size_t currentUsage;
    size_t peakUsage;
    size_t allocationCount;
    size_t freeCount;
} MemoryStats;

static MemoryStats gMemoryStats = {0};
BC_MUTEX_MAYBE(gMemoryMutex)

typedef struct {
    size_t size;
    void* data;
} BC_MemoryBlock;

void INTERNAL_BC_MemoryInitialize() {
	BC_MutexInit(&gMemoryMutex);
}

void* BC_Malloc(const size_t size) {
    if (size == 0) return NULL;

    BC_MemoryBlock* block = malloc(sizeof(BC_MemoryBlock) + size);
    if (!block) return NULL;

    block->size = size;
    block->data = (char*)block + sizeof(BC_MemoryBlock);

    BC_MutexLock(&gMemoryMutex);
    gMemoryStats.totalAllocated += size;
    gMemoryStats.currentUsage += size;
    gMemoryStats.allocationCount++;

    if (gMemoryStats.currentUsage > gMemoryStats.peakUsage) {
        gMemoryStats.peakUsage = gMemoryStats.currentUsage;
    }
    BC_MutexUnlock(&gMemoryMutex);

    return block->data;
}

void* BC_Calloc(const size_t count, const size_t size) {
    const size_t totalSize = count * size;
    void* ptr = BC_Malloc(totalSize);

    if (ptr) {
        memset(ptr, 0, totalSize);
    }

    return ptr;
}

void* BC_Realloc(void* ptr, const size_t newSize) {
    if (!ptr) return BC_Malloc(newSize);
    if (newSize == 0) {
        BC_Free(ptr);
        return NULL;
    }

    BC_MemoryBlock* oldBlock = (BC_MemoryBlock*)((char*)ptr - sizeof(BC_MemoryBlock));
    const size_t oldSize = oldBlock->size;

    BC_MemoryBlock* newBlock = realloc(oldBlock, sizeof(BC_MemoryBlock) + newSize);
    if (!newBlock) return NULL;

    newBlock->size = newSize;
    newBlock->data = (char*)newBlock + sizeof(BC_MemoryBlock);

    BC_MutexLock(&gMemoryMutex);
    gMemoryStats.currentUsage = gMemoryStats.currentUsage - oldSize + newSize;
    gMemoryStats.totalAllocated += (newSize > oldSize) ? (newSize - oldSize) : 0;

    if (gMemoryStats.currentUsage > gMemoryStats.peakUsage) {
        gMemoryStats.peakUsage = gMemoryStats.currentUsage;
    }
    BC_MutexUnlock(&gMemoryMutex);

    return newBlock->data;
}

void BC_Free(void* ptr) {
    if (!ptr) return;

    BC_MemoryBlock* block = (BC_MemoryBlock*)((char*)ptr - sizeof(BC_MemoryBlock));

    BC_MutexLock(&gMemoryMutex);
    gMemoryStats.currentUsage -= block->size;
    gMemoryStats.freeCount++;
    BC_MutexUnlock(&gMemoryMutex);

    free(block);
}

char* BC_Strdup(const char* str) {
	if (!str) return NULL;

	const size_t len = strlen(str) + 1;
	char* dup = BC_Malloc(len);
	if (dup) {
		memcpy(dup, str, len);
	}
	return dup;
}

void BC_MemoryInfoHeapReset(void) {
	BC_MutexLock(&gMemoryMutex);
	gMemoryStats.totalAllocated = 0;
	gMemoryStats.currentUsage = 0;
	gMemoryStats.peakUsage = 0;
	gMemoryStats.allocationCount = 0;
	gMemoryStats.freeCount = 0;
	BC_MutexUnlock(&gMemoryMutex);
}
#else
void INTERNAL_BC_MemoryInitialize() {}
#endif

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")
#elif defined(__APPLE__)
#include <mach/mach.h>
#elif defined(__linux__)
#include <unistd.h>
#include <sys/resource.h>
#endif

int BC_MemoryInfoGet(BC_MemoryInfo* info) {
	if (!info) return -1;

	info->system_current_rss = 0;
	info->system_peak_rss = 0;

#if BC_SETTINGS_DEBUG_ALLOCATION_TRACK == 1

	BC_MutexLock(&gMemoryMutex);
	info->freeCount = gMemoryStats.freeCount;
	info->allocationCount = gMemoryStats.allocationCount;
	info->totalAllocated = gMemoryStats.totalAllocated;
	info->currentAllocUsage = gMemoryStats.currentUsage;
	info->peakAllocUsage = gMemoryStats.peakUsage;
	BC_MutexUnlock(&gMemoryMutex);

#endif

#if defined(_WIN32) || defined(_WIN64)
	PROCESS_MEMORY_COUNTERS pmc;
	if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
		info->system_current_rss = pmc.WorkingSetSize;
		info->system_peak_rss = pmc.PeakWorkingSetSize;
		return 1;
	}
#elif defined(__APPLE__)
	struct mach_task_basic_info minfo;
	mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
	if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t) & minfo, &count) == KERN_SUCCESS) {
		info->system_current_rss = minfo.resident_size;
		struct rusage usage;
		getrusage(RUSAGE_SELF, &usage);
		info->system_peak_rss = usage.ru_maxrss;
		return 1;
	}
#elif defined(__linux__)
	struct rusage usage;
	if (getrusage(RUSAGE_SELF, &usage) == 0) {
		// Reported in KB
		info->system_current_rss = usage.ru_maxrss * 1024;
		info->system_peak_rss = usage.ru_maxrss * 1024;
		return 1;
	}
#endif

#if BC_SETTINGS_DEBUG_ALLOCATION_TRACK == 1
	return 0;
#else
	return -1;
#endif
}

#define DGRAY "\033[48;5;234m"
#define BLACK "\033[48;5;235m"
#define RESET "\033[0m"
#define BOLD "\033[1m"

static void PRIV_FormatBytes(const size_t bytes, char *out, const size_t out_size)
{
	const char *units[] = { "B", "KB", "MB", "GB", "TB" };
	double value = (double)bytes;
	int unit = 0;

	while (value >= 1024.0 && unit < 4) {
		value /= 1024.0;
		unit++;
	}

	snprintf(out, out_size, "%.2f %s", value, units[unit]);
}

void BC_MemoryInfoPrint(void) {
	BC_MemoryInfo info;
	const int result = BC_MemoryInfoGet(&info);

	// Header
	printf("\n"
		" " "                        "BOLD"Memory Statistics"RESET"\n"
		"┌" "───────────────────────┬───────────────────────────────────────" "┐\n"
		"│"DGRAY BOLD"         Metric"RESET DGRAY"        │                 "BOLD"Value"RESET DGRAY"                 "
		RESET"│\n"
		"├"BLACK "───────────────────────┼───────────────────────────────────────"RESET"┤\n"
	);

	// System Memory (if available)
	if (result >= 1) {
		printf("│"DGRAY" System Current RSS    │ %34.2f MB "RESET"│\n",
				(double) info.system_current_rss / (1024.0 * 1024.0));
		printf("│"BLACK" System Peak RSS       │ %34.2f MB "RESET"│\n",
				(double) info.system_peak_rss / (1024.0 * 1024.0));
#if BC_SETTINGS_DEBUG_ALLOCATION_TRACK == 1
		printf("├"DGRAY"───────────────────────┼───────────────────────────────────────"RESET"┤\n");
	}

	// Heap Statistics
	char cur[37], peak[37], total[37];
	PRIV_FormatBytes(info.currentAllocUsage, cur, sizeof(cur));
	PRIV_FormatBytes(info.peakAllocUsage, peak, sizeof(peak));
	PRIV_FormatBytes(info.totalAllocated, total, sizeof(total));
	printf("│" DGRAY " Current Heap Usage    │ %37s " RESET "│\n", cur);
	printf("│" BLACK " Peak Heap Usage       │ %37s " RESET "│\n", peak);
	printf("│" DGRAY " Total Heap Allocated  │ %37s " RESET "│\n", total);


	printf("├"BLACK"───────────────────────┼───────────────────────────────────────"RESET"┤\n");
	printf("│"DGRAY" Number of Allocations │ %37zu "RESET"│\n", info.allocationCount);
	printf("│"BLACK" Number of Frees       │ %37zu "RESET"│\n", info.freeCount);
	printf("│"DGRAY" Active Allocations    │ %37zu "RESET"│\n",
			info.allocationCount - info.freeCount);
#else
    }
#endif
	// Footer
	printf("└───────────────────────┴───────────────────────────────────────┘\n\n");
}
