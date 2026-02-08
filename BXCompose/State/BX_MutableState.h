#ifndef BCOMPOSE_MUTABLESTATE_H
#define BCOMPOSE_MUTABLESTATE_H

#include "../BX_Types.h"
#include "BFramework/BObject/BO_Object.h"

// =========================================================
// MARK: MutableState Types
// =========================================================

// MutableState is a refcounted object that holds a value
// and triggers recomposition when changed
struct BX_MutableState {
    BO_Object base;         // Refcounted base
    void* value;
    size_t valueSize;
    uint32_t version;       // Incremented on each change
    BX_NodeRef boundNode;   // Node to invalidate on change
};

// =========================================================
// MARK: MutableState API
// =========================================================

// Create a new mutable state with initial value
BX_StateRef BX_MutableStateCreate(size_t valueSize, const void* initialValue);

// Get the value pointer (read-only access)
const void* BX_MutableStateGetValue(BX_StateRef state);

// Set the value (triggers recomposition if changed)
void BX_MutableStateSetValue(BX_StateRef state, const void* value, size_t size);

// Get current version (for change detection)
uint32_t BX_MutableStateGetVersion(BX_StateRef state);

// Bind state to a node (for dirty tracking)
void BX_MutableStateBindNode(BX_StateRef state, BX_NodeRef node);

// =========================================================
// MARK: State Macros
// =========================================================

// Create and remember a state
// Usage: BX_StateRef countState = BX_State(int, 0);
#define BX_State(_type_, _init_) \
    ({ \
        BX_StateRef* _statePtr = BX_RememberPtr(BX_StateRef, NULL); \
        if (*_statePtr == NULL) { \
            _type_ _initVal = (_init_); \
            *_statePtr = BX_MutableStateCreate(sizeof(_type_), &_initVal); \
            BX_MutableStateBindNode(*_statePtr, BX_CurrentNode()); \
        } \
        *_statePtr; \
    })

// Get value from state
// Usage: int count = BX_StateGet(countState, int);
#define BX_StateGet(_state_, _type_) \
    (*(_type_*)BX_MutableStateGetValue(_state_))

// Set value on state (triggers recomposition)
// Usage: BX_StateSet(countState, int, count + 1);
#define BX_StateSet(_state_, _type_, _value_) \
    do { \
        _type_ _tmp = (_value_); \
        BX_MutableStateSetValue(_state_, &_tmp, sizeof(_type_)); \
    } while(0)

#endif //BCOMPOSE_MUTABLESTATE_H
