#include "BCMemInfo.h"

#include <stdio.h>

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

bool BCMemoryInfoGet(BCMemoryInfo* info) {
	if (!info) return false;

#if defined(_WIN32) || defined(_WIN64)
	PROCESS_MEMORY_COUNTERS pmc;
	if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
		info->current_rss = pmc.WorkingSetSize;
		info->peak_rss = pmc.PeakWorkingSetSize;
		return true;
	}
#elif defined(__APPLE__)
	struct mach_task_basic_info minfo;
	mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
	if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t) & minfo, &count) == KERN_SUCCESS) {
		info->current_rss = minfo.resident_size;
		struct rusage usage;
		getrusage(RUSAGE_SELF, &usage);
		info->peak_rss = usage.ru_maxrss;
		return true;
	}
#elif defined(__linux__)
	struct rusage usage;
	if (getrusage(RUSAGE_SELF, &usage) == 0) {
		// Reported in KB
		info->current_rss = usage.ru_maxrss * 1024;
		info->peak_rss = usage.ru_maxrss * 1024;
		return true;
	}
#endif

	return false;
}

void BCMemoryInfoPrint() {
	BCMemoryInfo info;
	if (BCMemoryInfoGet(&info)) {
		printf("Current Memory: %.2f MB\n", (double)info.current_rss / (1024.0 * 1024.0));
		printf("Peak Memory:    %.2f MB\n", (double)info.peak_rss / (1024.0 * 1024.0));
	}
	else {
		printf("Failed to get memory info\n");
	}
}
