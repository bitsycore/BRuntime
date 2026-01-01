#ifndef BFRAMEWORK_H
#define BFRAMEWORK_H

#include "BCore/BC_Keywords.h"
#include "BCore/BC_Macro.h"

#include "BF_AutoreleasePool.h"
#include "BF_Boxing.h"

#include "Object/BO_List.h"
#include "Object/BO_Map.h"
#include "Object/BO_Number.h"
#include "Object/BO_Object.h"
#include "Object/BO_ReleasePool.h"
#include "Object/BO_String.h"

// ================================================
// MARK: RUNTIME INITIALIZATION
// ================================================

void BFInitialize(int argc, char** argv);
void BFDeinitialize(void);
int BFArgc(void);
char** BFArgv(void);

#ifdef BF_RUNTIME_MAIN

int BCMain(void);

int main(int argc, char** argv) {
	BFInitialize(argc, argv);
	BCMain();
	BFDeinitialize();
}

#endif

#endif //BFRAMEWORK_H
