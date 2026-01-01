#ifndef BFRAMEWORK_H
#define BFRAMEWORK_H

#include "BF_AutoreleasePool.h"
#include "BF_Boxing.h"
#include "BF_Class.h"
#include "BF_Format.h"
#include "BF_Settings.h"
#include "BF_Types.h"

#include "BObject/BO.h"

// ================================================
// MARK: RUNTIME INITIALIZATION
// ================================================

void BF_Initialize(int argc, char** argv);
void BF_Deinitialize(void);

#ifdef BF_IMPLEMENT_MAIN

int BF_Main(void);

int main(int argc, char** argv) {
	BF_Initialize(argc, argv);
	BF_Main();
	BF_Deinitialize();
}

#endif

#endif //BFRAMEWORK_H
