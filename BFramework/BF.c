#include "BF.h"

#include "BCore/BC.h"
#include "BCore/BC_Types.h"

// =========================================================
// MARK: Initialize
// =========================================================

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

static BC_bool ___BF_INTERNAL___Initialized = BC_false;

void BF_Initialize(const int argc, char** argv) {
	if (___BF_INTERNAL___Initialized) return;

	BC_Initialize(argc, argv);

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

void BF_Deinitialize(void) {
	if (___BF_INTERNAL___Deinitialized || !___BF_INTERNAL___Initialized) return;

	___BO_INTERNAL___StringPoolDeinitialize();
	___BO_INTERNAL___ObjectDebugDeinitialize();

	___BF_INTERNAL___AutoreleaseDeinitialize();
	___BF_INTERNAL___ClassRegistryDeinitialize();

	BC_Deinitialize();

	___BF_INTERNAL___Deinitialized = BC_true;
}
