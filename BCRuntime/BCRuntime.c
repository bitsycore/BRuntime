#include "BCRuntime.h"

#include <stdio.h>

#include "BCClassRegistry.h"
#include "Utilities/BCMemory.h"

extern void ___BCINTERNAL___MemoryInitialize();
extern void ___BCINTERNAL___ClassRegistryInitialize();
extern void ___BCINTERNAL___NumberInitialize();

extern void ___BCINTERNAL___StringPoolInitialize();
extern void ___BCINTERNAL___StringInitialize();
extern void ___BCINTERNAL___StringBuilderInitialize();

extern void ___BCINTERNAL___ObjectDebugInitialize();

extern void ___BCINTERNAL___StringPoolDeinitialize();
extern void ___BCINTERNAL___ObjectDebugDeinitialize();

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

BC_bool ___BCINTERNAL___Initialized = BC_false;
BC_bool ___BCINTERNAL___Deinitialized = BC_false;

int gArgc;
char **gArgv;

void BCInitialize(const int argc, char **argv) {
  if (___BCINTERNAL___Initialized)
    return;

  gArgc = argc;
  gArgv = argv;

  PlatformSpecificInitialize();
  ___BCINTERNAL___MemoryInitialize();
  ___BCINTERNAL___ClassRegistryInitialize();

  ___BCINTERNAL___StringInitialize();
  ___BCINTERNAL___StringBuilderInitialize();
  ___BCINTERNAL___StringPoolInitialize();
  ___BCINTERNAL___NumberInitialize();

  ___BCINTERNAL___ObjectDebugInitialize();

  ___BCINTERNAL___Initialized = BC_true;
}

void BCDeinitialize(void) {
  if (___BCINTERNAL___Deinitialized)
    return;

  ___BCINTERNAL___StringPoolDeinitialize();
  ___BCINTERNAL___ObjectDebugDeinitialize();
  ___BCINTERNAL___ClassRegistryDeinitialize();

  BCMemoryInfoPrint();

  ___BCINTERNAL___Deinitialized = BC_true;
}