#ifndef BCOMPOSE_REMEMBER_H
#define BCOMPOSE_REMEMBER_H

#include "../BX_Types.h"
#include "../BX_Settings.h"

#include <stddef.h>

// =========================================================
// MARK: Remember API
// =========================================================

// Allocate persisted storage for a value across recompositions
// Returns pointer to storage that persists until node is removed
void* BX_Remember(size_t size);

// Check if this is the first composition (for initialization)
BC_bool BX_IsFirstComposition(void);

// =========================================================
// MARK: Remember Macros
// =========================================================

// Remember a value of a specific type, initializing on first composition
// Usage: int* countPtr = BX_RememberPtr(int, 0);
#define BX_RememberPtr(_type_, _init_) \
    ((_type_*)({ \
        void* _ptr = BX_Remember(sizeof(_type_)); \
        if (BX_IsFirstComposition() && _ptr) { *(_type_*)_ptr = (_init_); } \
        _ptr; \
    }))

// Remember and dereference - returns value, not pointer
// Usage: int count = BX_RememberValue(int, 0);
// Note: Returns the dereferenced value, modifications need BX_RememberPtr
#define BX_RememberValue(_type_, _init_) \
    (*BX_RememberPtr(_type_, _init_))

#endif //BCOMPOSE_REMEMBER_H
