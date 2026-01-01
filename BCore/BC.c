#include "BC.h"

#include "BC_Types.h"
#include "Memory/BC_Memory.h"

// =========================================================
// MARK: ARGS
// =========================================================

static int gArgc;
static char** gArgv;

int BC_Argc(void) {
	return gArgc;
}

char** BC_Argv(void) {
	return gArgv;
}

// =========================================================
// MARK: PLATFORM
// =========================================================

#ifdef _WIN32
#include <Windows.h>
static void PRIV_PlatformInitialize() {
	// For UTF-8 output
	SetConsoleOutputCP(CP_UTF8);

	// For ANSI escape sequences
	const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE)
		return;
	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode))
		return;
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hOut, dwMode);
}
#else
#define PRIV_PlatformInitialize()
#endif

// =========================================================
// MARK: INITIALIZE
// =========================================================

extern void INTERNAL_BC_MemoryInitialize();

static BC_bool BC_IsInitialized = BC_false;

void BC_Initialize(const int argc, char** argv) {
	if (BC_IsInitialized) return;

	gArgc = argc;
	gArgv = argv;

	PRIV_PlatformInitialize();

	INTERNAL_BC_MemoryInitialize();

	BC_IsInitialized = BC_true;
}

// =========================================================
// MARK: DEINITIALIZE
// =========================================================

static BC_bool BC_IsDeinitialized = BC_false;

void BC_Deinitialize(void) {
	if (BC_IsDeinitialized || !BC_IsInitialized) return;

	BC_MemoryInfoPrint();

	BC_IsDeinitialized = BC_true;
}
