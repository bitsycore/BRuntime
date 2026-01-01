#include "BF.h"

#include "BCore/Memory/BC_Memory.h"

#include <stdio.h>

// =========================================================
// MARK: Initialize
// =========================================================

extern void ___BC_INTERNAL___MemoryInitialize();

extern void ___BF_INTERNAL___ClassRegistryInitialize();
extern void ___BF_INTERNAL___AutoreleaseInitialize();

extern void ___BO_INTERNAL___ObjectInitialize();
extern void ___BO_INTERNAL___ReleasePoolInitialize();
extern void ___BO_INTERNAL___NumberInitialize();
extern void ___BO_INTERNAL___MapInitialize();
extern void ___BO_INTERNAL___SetInitialize();
extern void ___BO_INTERNAL___ListInitialize();
extern void ___BO_INTERNAL___BytesArrayInitialize();
extern void ___BO_INTERNAL___StringPoolInitialize();
extern void ___BO_INTERNAL___StringInitialize();
extern void ___BO_INTERNAL___StringBuilderInitialize();

#ifdef _WIN32
#include <Windows.h>
static void PlatformSpecificInitialize() {
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
#define PlatformSpecificInitialize()
#endif

static BC_bool ___BF_INTERNAL___Initialized = BC_false;

static int gArgc;
static char** gArgv;

int BFArgc(void) {
	return gArgc;
}

char** BFArgv(void) {
	return gArgv;
}

void BFInitialize(const int argc, char** argv) {
	if (___BF_INTERNAL___Initialized)
		return;

	gArgc = argc;
	gArgv = argv;

	PlatformSpecificInitialize();

	___BC_INTERNAL___MemoryInitialize();

	___BF_INTERNAL___ClassRegistryInitialize();
	___BF_INTERNAL___AutoreleaseInitialize();

	___BO_INTERNAL___ObjectInitialize();
	___BO_INTERNAL___ReleasePoolInitialize();
	___BO_INTERNAL___NumberInitialize();
	___BO_INTERNAL___MapInitialize();
	___BO_INTERNAL___SetInitialize();
	___BO_INTERNAL___ListInitialize();
	___BO_INTERNAL___BytesArrayInitialize();
	___BO_INTERNAL___StringPoolInitialize();
	___BO_INTERNAL___StringInitialize();
	___BO_INTERNAL___StringBuilderInitialize();

	___BF_INTERNAL___Initialized = BC_true;
}

// =========================================================
// MARK: Deinitialize
// =========================================================

extern void ___BF_INTERNAL___AutoreleaseDeinitialize();
extern void ___BO_INTERNAL___StringPoolDeinitialize();
extern void ___BO_INTERNAL___ObjectDebugDeinitialize();
extern void ___BF_INTERNAL___ClassRegistryDeinitialize();

BC_bool ___BF_INTERNAL___Deinitialized = BC_false;

void BFDeinitialize(void) {
	if (___BF_INTERNAL___Deinitialized)
		return;

	___BO_INTERNAL___StringPoolDeinitialize();
	___BO_INTERNAL___ObjectDebugDeinitialize();

	___BF_INTERNAL___AutoreleaseDeinitialize();
	___BF_INTERNAL___ClassRegistryDeinitialize();

	BC_MemoryInfoPrint();

	___BF_INTERNAL___Deinitialized = BC_true;
}
