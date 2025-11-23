#include "BCMemory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BCThreads.h"

typedef struct {
    size_t totalAllocated;
    size_t currentUsage;
    size_t peakUsage;
    size_t allocationCount;
    size_t freeCount;
} BCMemoryStats;

static BCMemoryStats gMemoryStats = {0};
static BCMutex gMemoryMutex;
static BCOnceToken gMemoryMutexInitToken = BC_ONCE_INIT;

typedef struct {
    size_t size;
    void* data;
} BCMemoryBlock;

void ___BCINTERNAL___MemoryInitialize() {
	BCMutexInit(&gMemoryMutex);
}

void* BCMalloc(const size_t size) {
    if (size == 0) return NULL;

    BCMemoryBlock* block = malloc(sizeof(BCMemoryBlock) + size);
    if (!block) return NULL;

    block->size = size;
    block->data = (char*)block + sizeof(BCMemoryBlock);

    BCMutexLock(&gMemoryMutex);
    gMemoryStats.totalAllocated += size;
    gMemoryStats.currentUsage += size;
    gMemoryStats.allocationCount++;

    if (gMemoryStats.currentUsage > gMemoryStats.peakUsage) {
        gMemoryStats.peakUsage = gMemoryStats.currentUsage;
    }
    BCMutexUnlock(&gMemoryMutex);

    return block->data;
}

void* BCCalloc(const size_t count, const size_t size) {
    const size_t totalSize = count * size;
    void* ptr = BCMalloc(totalSize);

    if (ptr) {
        memset(ptr, 0, totalSize);
    }

    return ptr;
}

void* BCRealloc(void* ptr, const size_t newSize) {
    if (!ptr) return BCMalloc(newSize);
    if (newSize == 0) {
        BCFree(ptr);
        return NULL;
    }

    BCMemoryBlock* oldBlock = (BCMemoryBlock*)((char*)ptr - sizeof(BCMemoryBlock));
    const size_t oldSize = oldBlock->size;

    BCMemoryBlock* newBlock = realloc(oldBlock, sizeof(BCMemoryBlock) + newSize);
    if (!newBlock) return NULL;

    newBlock->size = newSize;
    newBlock->data = (char*)newBlock + sizeof(BCMemoryBlock);

    BCMutexLock(&gMemoryMutex);
    gMemoryStats.currentUsage = gMemoryStats.currentUsage - oldSize + newSize;
    gMemoryStats.totalAllocated += (newSize > oldSize) ? (newSize - oldSize) : 0;

    if (gMemoryStats.currentUsage > gMemoryStats.peakUsage) {
        gMemoryStats.peakUsage = gMemoryStats.currentUsage;
    }
    BCMutexUnlock(&gMemoryMutex);

    return newBlock->data;
}

void BCFree(void* ptr) {
    if (!ptr) return;

    BCMemoryBlock* block = (BCMemoryBlock*)((char*)ptr - sizeof(BCMemoryBlock));

    BCMutexLock(&gMemoryMutex);
    gMemoryStats.currentUsage -= block->size;
    gMemoryStats.freeCount++;
    BCMutexUnlock(&gMemoryMutex);

    free(block);
}

void BCGetMemoryStats(size_t* totalAllocated, size_t* currentUsage, size_t* peakUsage) {
	BCMutexLock(&gMemoryMutex);
    if (totalAllocated) *totalAllocated = gMemoryStats.totalAllocated;
    if (currentUsage) *currentUsage = gMemoryStats.currentUsage;
    if (peakUsage) *peakUsage = gMemoryStats.peakUsage;
    BCMutexUnlock(&gMemoryMutex);
}

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

void BCMemoryInfoHeapReset(void) {
	BCMutexLock(&gMemoryMutex);
	gMemoryStats.totalAllocated = 0;
	gMemoryStats.currentUsage = 0;
	gMemoryStats.peakUsage = 0;
	gMemoryStats.allocationCount = 0;
	gMemoryStats.freeCount = 0;
	BCMutexUnlock(&gMemoryMutex);
}

int BCMemoryInfoGet(BCMemoryInfo* info) {
	if (!info) return -1;

	info->system_current_rss = 0;
	info->system_peak_rss = 0;

	BCMutexLock(&gMemoryMutex);
	info->freeCount = gMemoryStats.freeCount;
	info->allocationCount = gMemoryStats.allocationCount;
	info->totalAllocated = gMemoryStats.totalAllocated;
	info->currentAllocUsage = gMemoryStats.currentUsage;
	info->peakAllocUsage = gMemoryStats.peakUsage;
	BCMutexUnlock(&gMemoryMutex);

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

	return 0;
}

#define DGRAY "\033[48;5;234m"
#define BLACK "\033[48;5;235m"
#define RESET "\033[0m"
#define BOLD "\033[1m"

void BCMemoryInfoPrint(void) {
    BCMemoryInfo info;
    const int result = BCMemoryInfoGet(&info);

    // Header
    printf("\n"
        " "          "                        "BOLD"Memory Statistics"RESET"\n"
        "┌"          "───────────────────────┬───────────────────────────────────────"     "┐\n"
        "│"DGRAY BOLD"         Metric"RESET DGRAY"        │                 "BOLD"Value"RESET DGRAY"                 "RESET"│\n"
        "├"BLACK     "───────────────────────┼───────────────────────────────────────"RESET"┤\n"
    );

    // System Memory (if available)
    if (result >= 1) {
    printf("│"DGRAY" System Current RSS    │ %34.2f MB "RESET"│\n", (double)info.system_current_rss / (1024.0 * 1024.0));
    printf("│"BLACK" System Peak RSS       │ %34.2f MB "RESET"│\n", (double)info.system_peak_rss / (1024.0 * 1024.0));
    printf("├"DGRAY"───────────────────────┼───────────────────────────────────────"RESET"┤\n");
    }

    // Heap Statistics
    printf("│"DGRAY" Current Heap Usage    │ %31zu bytes "RESET"│\n", info.currentAllocUsage);
    printf("│"BLACK" Peak Heap Usage       │ %31zu bytes "RESET"│\n", info.peakAllocUsage);
    printf("│"DGRAY" Total Heap Allocated  │ %31zu bytes "RESET"│\n", info.totalAllocated);
    printf("├"BLACK"───────────────────────┼───────────────────────────────────────"RESET"┤\n");
    printf("│"DGRAY" Number of Allocations │ %37zu "RESET"│\n", info.allocationCount);
    printf("│"BLACK" Number of Frees       │ %37zu "RESET"│\n", info.freeCount);
    printf("│"DGRAY" Active Allocations    │ %37zu "RESET"│\n",
           info.allocationCount - info.freeCount);

    // Footer
    printf("└───────────────────────┴───────────────────────────────────────┘\n\n");
}
