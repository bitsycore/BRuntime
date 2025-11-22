#include "BCRuntime.h"

extern void _BCNumberInitialize();

#ifdef _WIN32
#include <Windows.h>
static void PlatformSpecificInitialize() {
	// Enable ANSI escape sequences
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

void __internal_BCInitialize(void) {
	PlatformSpecificInitialize();
	_BCNumberInitialize();
}

void __internal_BCUninitialize(void) {

}