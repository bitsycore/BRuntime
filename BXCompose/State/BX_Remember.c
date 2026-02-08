#include "BX_Remember.h"

#include "../Core/BX_Context.h"
#include "../Core/BX_Node.h"

#include "BCore/BC_Keywords.h"
#include "BCore/Memory/BC_Memory.h"

#include <string.h>

// =========================================================
// MARK: State Slot Storage
// =========================================================

// State slots are stored per-node in a separate system-allocated array
// This allows them to persist across frame arena resets

typedef struct BX_StateSlot {
    void* data;
    size_t size;
} BX_StateSlot;

typedef struct BX_NodeStateStorage {
    BX_StateSlot slots[BX_SETTINGS_MAX_STATE_SLOTS];
    uint32_t slotCount;
    uint32_t currentSlot;  // Cursor for sequential access during composition
} BX_NodeStateStorage;

// TLS storage for current node's state
// In a real implementation, this would be associated with node identity
static BC_TLS BX_NodeStateStorage* gBX_CurrentStateStorage = NULL;
static BC_TLS BC_bool gBX_IsFirstComposition = BC_true;

// =========================================================
// MARK: Internal Functions
// =========================================================

static BX_NodeStateStorage* PRIV_GetOrCreateStateStorage(void) {
    if (!gBX_CurrentStateStorage) {
        gBX_CurrentStateStorage = BC_Calloc(1, sizeof(BX_NodeStateStorage));
        gBX_IsFirstComposition = BC_true;
    }
    return gBX_CurrentStateStorage;
}

// =========================================================
// MARK: Public API
// =========================================================

void* BX_Remember(size_t size) {
    BX_NodeStateStorage* storage = PRIV_GetOrCreateStateStorage();
    if (!storage) return NULL;

    uint32_t slotIdx = storage->currentSlot;

    // Check if we've exceeded max slots
    if (slotIdx >= BX_SETTINGS_MAX_STATE_SLOTS) {
        return NULL;
    }

    BX_StateSlot* slot = &storage->slots[slotIdx];

    // First composition - allocate storage
    if (gBX_IsFirstComposition || slot->data == NULL) {
        if (slot->data && slot->size != size) {
            // Size changed - reallocate
            BC_Free(slot->data);
            slot->data = NULL;
        }

        if (!slot->data) {
            slot->data = BC_Calloc(1, size);
            slot->size = size;
        }

        if (slotIdx >= storage->slotCount) {
            storage->slotCount = slotIdx + 1;
        }
    }

    storage->currentSlot++;
    return slot->data;
}

BC_bool BX_IsFirstComposition(void) {
    return gBX_IsFirstComposition;
}

// =========================================================
// MARK: Internal - Called by Context
// =========================================================

void INTERNAL_BX_StateBeginComposition(void) {
    BX_NodeStateStorage* storage = PRIV_GetOrCreateStateStorage();
    if (storage) {
        storage->currentSlot = 0;
    }
}

void INTERNAL_BX_StateEndComposition(void) {
    gBX_IsFirstComposition = BC_false;
}

void INTERNAL_BX_StateReset(void) {
    if (gBX_CurrentStateStorage) {
        // Free all slot data
        for (uint32_t i = 0; i < gBX_CurrentStateStorage->slotCount; i++) {
            if (gBX_CurrentStateStorage->slots[i].data) {
                BC_Free(gBX_CurrentStateStorage->slots[i].data);
            }
        }
        BC_Free(gBX_CurrentStateStorage);
        gBX_CurrentStateStorage = NULL;
    }
    gBX_IsFirstComposition = BC_true;
}
