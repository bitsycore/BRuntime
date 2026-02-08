#include "BX_MutableState.h"
#include "BX_Remember.h"

#include "../Core/BX_Node.h"

#include "BFramework/BF_Class.h"
#include "BCore/Memory/BC_Memory.h"

#include <string.h>

// =========================================================
// MARK: Class Definition
// =========================================================

static void PRIV_MutableStateDealloc(BO_ObjectRef obj);

static BF_Class gBX_MutableStateClass = {
    .name = "BX_MutableState",
    .id = BF_CLASS_ID_INVALID,
    .dealloc = PRIV_MutableStateDealloc,
    .hash = NULL,
    .equal = NULL,
    .toString = NULL,
    .copy = NULL,
    .allocSize = sizeof(struct BX_MutableState),
};

static BF_ClassId gBX_MutableStateClassId = BF_CLASS_ID_INVALID;

static void PRIV_EnsureClassRegistered(void) {
    if (gBX_MutableStateClassId == BF_CLASS_ID_INVALID) {
        gBX_MutableStateClassId = BF_ClassRegistryInsert(&gBX_MutableStateClass);
    }
}

// =========================================================
// MARK: Lifecycle
// =========================================================

static void PRIV_MutableStateDealloc(BO_ObjectRef obj) {
    BX_StateRef state = (BX_StateRef)obj;
    if (state->value) {
        BC_Free(state->value);
        state->value = NULL;
    }
}

BX_StateRef BX_MutableStateCreate(size_t valueSize, const void* initialValue) {
    PRIV_EnsureClassRegistered();

    BX_StateRef state = (BX_StateRef)BO_ObjectAlloc(NULL, gBX_MutableStateClassId);
    if (!state) return NULL;

    state->valueSize = valueSize;
    state->version = 0;
    state->boundNode = NULL;

    if (valueSize > 0) {
        state->value = BC_Malloc(valueSize);
        if (state->value && initialValue) {
            memcpy(state->value, initialValue, valueSize);
        }
    } else {
        state->value = NULL;
    }

    return state;
}

// =========================================================
// MARK: Value Access
// =========================================================

const void* BX_MutableStateGetValue(BX_StateRef state) {
    return state ? state->value : NULL;
}

void BX_MutableStateSetValue(BX_StateRef state, const void* value, size_t size) {
    if (!state || !value) return;

    // Check if value actually changed
    if (state->value && state->valueSize == size) {
        if (memcmp(state->value, value, size) == 0) {
            return; // No change
        }
    }

    // Reallocate if size changed
    if (state->valueSize != size) {
        if (state->value) {
            BC_Free(state->value);
        }
        state->value = BC_Malloc(size);
        state->valueSize = size;
    }

    // Copy new value
    if (state->value) {
        memcpy(state->value, value, size);
    }

    // Increment version
    state->version++;

    // Mark bound node as dirty
    if (state->boundNode) {
        BX_NodeMarkDirty(state->boundNode);
    }
}

uint32_t BX_MutableStateGetVersion(BX_StateRef state) {
    return state ? state->version : 0;
}

void BX_MutableStateBindNode(BX_StateRef state, BX_NodeRef node) {
    if (state) {
        state->boundNode = node;
    }
}
