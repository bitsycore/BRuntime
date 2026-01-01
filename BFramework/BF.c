#include "BF.h"

#include "BCore/BC.h"
#include "BCore/BC_Types.h"

// =========================================================
// MARK: Initialize
// =========================================================

extern void INTERNAL_BF_ClassRegistryInitialize();
extern void INTERNAL_BF_AutoreleaseInitialize();

extern void INTERNAL_BO_ObjectInitialize();
extern void INTERNAL_BO_ReleasePoolInitialize();
extern void INTERNAL_BO_NumberInitialize();
extern void INTERNAL_BO_MapInitialize();
extern void INTERNAL_BO_SetInitialize();
extern void INTERNAL_BO_ListInitialize();
extern void INTERNAL_BO_BytesArrayInitialize();
extern void INTERNAL_BO_StringPoolInitialize();
extern void INTERNAL_BO_StringInitialize();
extern void INTERNAL_BO_StringBuilderInitialize();

static BC_bool BF_IsInitialized = BC_false;

void BF_Initialize(const int argc, char** argv) {
	if (BF_IsInitialized) return;

	BC_Initialize(argc, argv);

	INTERNAL_BF_ClassRegistryInitialize();
	INTERNAL_BF_AutoreleaseInitialize();

	INTERNAL_BO_ObjectInitialize();
	INTERNAL_BO_ReleasePoolInitialize();
	INTERNAL_BO_NumberInitialize();
	INTERNAL_BO_MapInitialize();
	INTERNAL_BO_SetInitialize();
	INTERNAL_BO_ListInitialize();
	INTERNAL_BO_BytesArrayInitialize();
	INTERNAL_BO_StringPoolInitialize();
	INTERNAL_BO_StringInitialize();
	INTERNAL_BO_StringBuilderInitialize();

	BF_IsInitialized = BC_true;
}

// =========================================================
// MARK: Deinitialize
// =========================================================

extern void INTERNAL_BF_AutoreleaseDeinitialize();
extern void INTERNAL_BO_StringPoolDeinitialize();
extern void INTERNAL_BO_ObjectDebugDeinitialize();
extern void INTERNAL_BF_ClassRegistryDeinitialize();

BC_bool BF_IsDeinitialized = BC_false;

void BF_Deinitialize(void) {
	if (BF_IsDeinitialized || !BF_IsInitialized) return;

	INTERNAL_BO_StringPoolDeinitialize();
	INTERNAL_BO_ObjectDebugDeinitialize();

	INTERNAL_BF_AutoreleaseDeinitialize();
	INTERNAL_BF_ClassRegistryDeinitialize();

	BC_Deinitialize();

	BF_IsDeinitialized = BC_true;
}
