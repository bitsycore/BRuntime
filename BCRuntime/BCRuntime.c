#include "BCRuntime.h"

extern void ___BCINTERNAL___NumberInitialize();
extern void ___BCINTERNAL___StringPoolInit();
extern void ___BCINTERNAL___StringPoolDeinit();
extern void ___BCINTERNAL___ObjectDebugInit();
extern void ___BCINTERNAL___ObjectDebugDeinit();

#ifdef _WIN32
#include <Windows.h>
static void PlatformSpecificInitialize() {
	// For UTF-8 output
	SetConsoleOutputCP(CP_UTF8);

	// For ANSI escape sequences
	const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE) return;
	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode)) return;
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hOut, dwMode);
}
#else
#define PlatformSpecificInitialize()
#endif

void ___BCINTERNAL___InitializeImpl(void) {
	PlatformSpecificInitialize();
	___BCINTERNAL___ObjectDebugInit();
	___BCINTERNAL___StringPoolInit();
	___BCINTERNAL___NumberInitialize();
}

void ___BCINTERNAL___UninitializeImpl(void) {
	___BCINTERNAL___StringPoolDeinit();
	___BCINTERNAL___ObjectDebugDeinit();
}