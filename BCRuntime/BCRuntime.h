#ifndef BC_BCRUNTIME_H
#define BC_BCRUNTIME_H

#include "Utilities/BC_Keywords.h"
#include "Utilities/BC_Macro.h"

#include "Core/BCAutoreleasePool.h"
#include "Core/BCBoxMacro.h"
#include "Core/BCArena.h"

#include "Object/BCList.h"
#include "Object/BCMap.h"
#include "Object/BCNumber.h"
#include "Object/BCObject.h"
#include "Object/BCReleasePool.h"
#include "Object/BCString.h"

// ================================================
// MARK: RUNTIME INITIALIZATION
// ================================================

void BCInitialize(int argc, char** argv);
void BCDeinitialize(void);
int BCArgc(void);
char** BCArgv(void);

#ifdef BC_RUNTIME_MAIN

int BCMain(void);

int main(int argc, char** argv) {
	BCInitialize(argc, argv);
	BCMain();
	BCDeinitialize();
}

#endif

#endif // BC_BCRUNTIME_H
