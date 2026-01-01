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

extern void ___BC_INTERNAL___MemoryInitialize();

static BC_bool ___BC_INTERNAL___Initialized = BC_false;

void BC_Initialize(const int argc, char** argv) {
	if (___BC_INTERNAL___Initialized) return;

	gArgc = argc;
	gArgv = argv;

	PRIV_PlatformInitialize();

	___BC_INTERNAL___MemoryInitialize();

	___BC_INTERNAL___Initialized = BC_true;
}

// =========================================================
// MARK: DEINITIALIZE
// =========================================================

static BC_bool ___BC_INTERNAL___Deinitialized = BC_false;

void BC_Deinitialize(void) {
	if (___BC_INTERNAL___Deinitialized || !___BC_INTERNAL___Initialized) return;

	BC_MemoryInfoPrint();

	___BC_INTERNAL___Deinitialized = BC_true;
}
