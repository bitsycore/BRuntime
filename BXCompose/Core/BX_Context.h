#ifndef BCOMPOSE_CONTEXT_H
#define BCOMPOSE_CONTEXT_H

#include "../BX_Types.h"
#include "BCore/Memory/BC_Arena.h"

// =========================================================
// MARK: Composition Context
// =========================================================

// Get the current composition context (TLS)
BX_ContextRef BX_GetCurrentContext(void);

// Context management
void BX_ContextPush(void);
void BX_ContextPop(void);

// Context accessors
BX_NodeRef BX_ContextGetRoot(BX_ContextRef ctx);
BX_NodeRef BX_ContextGetCurrentNode(BX_ContextRef ctx);
BC_ArenaRef BX_ContextGetFrameArena(BX_ContextRef ctx);

// Node cursor management (for building tree)
void BX_ContextSetCurrentNode(BX_ContextRef ctx, BX_NodeRef node);
uint32_t BX_ContextGetNextSlotIndex(BX_ContextRef ctx);
void BX_ContextResetSlotIndex(BX_ContextRef ctx);

// Composition state
BC_bool BX_ContextIsFirstComposition(BX_ContextRef ctx);
void BX_ContextMarkComposed(BX_ContextRef ctx);

// =========================================================
// MARK: Internal Functions
// =========================================================

void INTERNAL_BX_ContextInitialize(void);
void INTERNAL_BX_ContextDeinitialize(void);

#endif //BCOMPOSE_CONTEXT_H
