#ifndef BCOMPOSE_SCOPE_H
#define BCOMPOSE_SCOPE_H

#include "../BX_Types.h"
#include "BX_Context.h"
#include "BX_Node.h"
#include "BCore/BC_Macro.h"

// =========================================================
// MARK: Composable Scope Macros
// =========================================================

// Internal: Begin a node scope - creates node, adds to parent, sets as current
static inline BX_NodeRef INTERNAL_BX_NodeBegin(BX_NodeType type, uint32_t key) {
    BX_ContextRef ctx = BX_GetCurrentContext();
    if (!ctx) return NULL;

    BX_NodeRef parent = BX_ContextGetCurrentNode(ctx);
    BX_NodeRef node = BX_NodeCreate(type, key);

    if (parent && node) {
        BX_NodeAddChild(parent, node);
    }

    BX_ContextSetCurrentNode(ctx, node);
    return node;
}

// Internal: End a node scope - restore parent as current
static inline void INTERNAL_BX_NodeEnd(void) {
    BX_ContextRef ctx = BX_GetCurrentContext();
    if (!ctx) return;

    BX_NodeRef current = BX_ContextGetCurrentNode(ctx);
    if (current) {
        BX_ContextSetCurrentNode(ctx, BX_NodeGetParent(current));
    }
}

// =========================================================
// MARK: Base Scope Macro (for-loop trick)
// =========================================================

#define INTERNAL_BX_ScopeImpl(_uid_, _type_, _key_) \
    for (BX_NodeRef BC_M_CAT(_bx_node_, _uid_) = INTERNAL_BX_NodeBegin(_type_, _key_); \
         BC_M_CAT(_bx_node_, _uid_); \
         BC_M_CAT(_bx_node_, _uid_) = (INTERNAL_BX_NodeEnd(), (BX_NodeRef)NULL))

#define BX_ScopeWithKey(_type_, _key_) \
    INTERNAL_BX_ScopeImpl(BC_M_CAT(_bx_, __COUNTER__), _type_, _key_)

// Auto-generate key from line number (stable if code doesn't change)
#define BX_Scope(_type_) \
    BX_ScopeWithKey(_type_, __COUNTER__)

// =========================================================
// MARK: Compose Scope (Root)
// =========================================================

#define INTERNAL_BX_ComposeImpl(_uid_) \
    for (BC_bool BC_M_CAT(_bx_once_, _uid_) = (BX_ContextPush(), BC_true); \
         BC_M_CAT(_bx_once_, _uid_); \
         BC_M_CAT(_bx_once_, _uid_) = (BX_ContextPop(), BC_false))

#define BX_Compose() \
    INTERNAL_BX_ComposeImpl(BC_M_CAT(_bx_compose_, __COUNTER__))

// =========================================================
// MARK: Convenient Layout Macros
// =========================================================

#define BX_Column() BX_Scope(BX_NODE_TYPE_COLUMN)
#define BX_Row()    BX_Scope(BX_NODE_TYPE_ROW)
#define BX_Box()    BX_Scope(BX_NODE_TYPE_BOX)

// With explicit key for list items
#define BX_ColumnKey(_key_) BX_ScopeWithKey(BX_NODE_TYPE_COLUMN, _key_)
#define BX_RowKey(_key_)    BX_ScopeWithKey(BX_NODE_TYPE_ROW, _key_)
#define BX_BoxKey(_key_)    BX_ScopeWithKey(BX_NODE_TYPE_BOX, _key_)

// =========================================================
// MARK: Current Node Access
// =========================================================

static inline BX_NodeRef BX_CurrentNode(void) {
    BX_ContextRef ctx = BX_GetCurrentContext();
    return ctx ? BX_ContextGetCurrentNode(ctx) : nullptr;
}

#endif //BCOMPOSE_SCOPE_H
