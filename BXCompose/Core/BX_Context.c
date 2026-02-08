#include "BX_Context.h"

#include "BX_Node.h"
#include "../BX_Settings.h"

#include "BCore/BC_Keywords.h"
#include "BCore/Memory/BC_Memory.h"
#include "BCore/Memory/BC_Arena.h"

#include <stdio.h>

// =========================================================
// MARK: Context Structure
// =========================================================

typedef struct BX_Context {
    struct BX_Context* parent;
    BX_NodeRef root;
    BX_NodeRef currentNode;
    BC_ArenaRef frameArena;
    uint32_t slotIndex;
    BC_bool isFirstComposition;
} BX_Context;

// =========================================================
// MARK: Thread Local Storage
// =========================================================

static BC_TLS BX_ContextRef gBX_CurrentContext = NULL;

// Free list for context reuse
static BC_TLS BX_ContextRef gBX_FreeContextList = NULL;
static BC_TLS uint8_t gBX_FreeContextCount = 0;

#define BX_CONTEXT_FREE_LIST_LIMIT 8

// =========================================================
// MARK: Private Functions
// =========================================================

static inline BX_ContextRef PRIV_AllocContext(void) {
    BX_ContextRef ctx;

    // Try to reuse from free list first
    if (gBX_FreeContextList) {
        ctx = gBX_FreeContextList;
        gBX_FreeContextList = ctx->parent;
        gBX_FreeContextCount--;
    } else {
        ctx = BC_Malloc(sizeof(BX_Context));
        ctx->frameArena = BC_ArenaCreate(NULL, BX_SETTINGS_FRAME_ARENA_SIZE);
    }

    // Reset context state
    ctx->parent = NULL;
    ctx->root = NULL;
    ctx->currentNode = NULL;
    ctx->slotIndex = 0;
    ctx->isFirstComposition = BC_true;

    // Reset arena for new frame
    BC_ArenaReset(ctx->frameArena);

    return ctx;
}

static inline void PRIV_FreeOrRecycleContext(BX_ContextRef ctx) {
    if (!ctx) return;

    // Add to free list if there's room, otherwise free
    if (gBX_FreeContextCount < BX_CONTEXT_FREE_LIST_LIMIT) {
        ctx->parent = gBX_FreeContextList;
        gBX_FreeContextList = ctx;
        gBX_FreeContextCount++;
    } else {
        BC_ArenaDestroy(ctx->frameArena);
        BC_Free(ctx);
    }
}

// =========================================================
// MARK: Internal Functions
// =========================================================

void INTERNAL_BX_ContextInitialize(void) {
    gBX_CurrentContext = NULL;
    gBX_FreeContextList = NULL;
    gBX_FreeContextCount = 0;
}

void INTERNAL_BX_ContextDeinitialize(void) {
    // Pop any active contexts
    while (gBX_CurrentContext) {
        BX_ContextPop();
    }

    // Free all contexts in free list
    BX_ContextRef ctx = gBX_FreeContextList;
    while (ctx) {
        BX_ContextRef next = ctx->parent;
        BC_ArenaDestroy(ctx->frameArena);
        BC_Free(ctx);
        ctx = next;
    }

    gBX_FreeContextList = NULL;
    gBX_FreeContextCount = 0;
}

// =========================================================
// MARK: Public API
// =========================================================

BX_ContextRef BX_GetCurrentContext(void) {
    return gBX_CurrentContext;
}

void BX_ContextPush(void) {
    BX_ContextRef ctx = PRIV_AllocContext();
    ctx->parent = gBX_CurrentContext;

    // Create root node for this composition
    ctx->root = BX_NodeCreate(BX_NODE_TYPE_ROOT, 0);
    ctx->currentNode = ctx->root;

    gBX_CurrentContext = ctx;
}

void BX_ContextPop(void) {
    if (!gBX_CurrentContext) return;

    BX_ContextRef ctx = gBX_CurrentContext;
    gBX_CurrentContext = ctx->parent;

    // Cleanup nodes (they use the arena, so just reset)
    // The arena reset happens when context is reused

    PRIV_FreeOrRecycleContext(ctx);
}

BX_NodeRef BX_ContextGetRoot(BX_ContextRef ctx) {
    return ctx ? ctx->root : NULL;
}

BX_NodeRef BX_ContextGetCurrentNode(BX_ContextRef ctx) {
    return ctx ? ctx->currentNode : NULL;
}

BC_ArenaRef BX_ContextGetFrameArena(BX_ContextRef ctx) {
    return ctx ? ctx->frameArena : NULL;
}

void BX_ContextSetCurrentNode(BX_ContextRef ctx, BX_NodeRef node) {
    if (ctx) {
        ctx->currentNode = node;
    }
}

uint32_t BX_ContextGetNextSlotIndex(BX_ContextRef ctx) {
    if (!ctx) return 0;
    return ctx->slotIndex++;
}

void BX_ContextResetSlotIndex(BX_ContextRef ctx) {
    if (ctx) {
        ctx->slotIndex = 0;
    }
}

BC_bool BX_ContextIsFirstComposition(BX_ContextRef ctx) {
    return ctx ? ctx->isFirstComposition : BC_true;
}

void BX_ContextMarkComposed(BX_ContextRef ctx) {
    if (ctx) {
        ctx->isFirstComposition = BC_false;
    }
}
