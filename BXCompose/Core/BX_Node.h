#ifndef BCOMPOSE_NODE_H
#define BCOMPOSE_NODE_H

#include "../BX_Types.h"

// =========================================================
// MARK: Node Flags
// =========================================================

typedef enum BX_NodeFlags {
    BX_NODE_FLAG_DIRTY          = 1 << 0,   // Needs recomposition
    BX_NODE_FLAG_LAYOUT_DIRTY   = 1 << 1,   // Needs layout recalculation
    BX_NODE_FLAG_HAS_STATE      = 1 << 2,   // Has associated state
    BX_NODE_FLAG_VISIBLE        = 1 << 3,   // Is visible
    BX_NODE_FLAG_ENABLED        = 1 << 4,   // Is enabled for interaction
} BX_NodeFlags;

// =========================================================
// MARK: Node Structure (exposed for inline access)
// =========================================================

struct BX_Node {
    BX_NodeType type;
    uint32_t key;                   // Stable identity for diffing
    uint16_t flags;
    uint16_t stateSlotStart;        // First state slot index
    uint16_t stateSlotCount;        // Number of state slots used

    struct BX_Node* parent;
    struct BX_Node* firstChild;
    struct BX_Node* lastChild;
    struct BX_Node* nextSibling;

    BX_Rect bounds;                 // Computed layout bounds
    BX_EdgeInsets padding;
    BX_EdgeInsets margin;

    void* content;                  // Type-specific content (Arena allocated)
    size_t contentSize;
};

// =========================================================
// MARK: Node API
// =========================================================

// Creation (uses frame arena)
BX_NodeRef BX_NodeCreate(BX_NodeType type, uint32_t key);
BX_NodeRef BX_NodeCreateWithContent(BX_NodeType type, uint32_t key, void* content, size_t contentSize);

// Tree manipulation
void BX_NodeAddChild(BX_NodeRef parent, BX_NodeRef child);
void BX_NodeRemoveFromParent(BX_NodeRef node);

// Traversal
BX_NodeRef BX_NodeGetParent(BX_NodeRef node);
BX_NodeRef BX_NodeGetFirstChild(BX_NodeRef node);
BX_NodeRef BX_NodeGetNextSibling(BX_NodeRef node);

// Properties
BX_NodeType BX_NodeGetType(BX_NodeRef node);
uint32_t BX_NodeGetKey(BX_NodeRef node);
void BX_NodeSetBounds(BX_NodeRef node, BX_Rect bounds);
BX_Rect BX_NodeGetBounds(BX_NodeRef node);

// Flags
BC_bool BX_NodeHasFlag(BX_NodeRef node, BX_NodeFlags flag);
void BX_NodeSetFlag(BX_NodeRef node, BX_NodeFlags flag);
void BX_NodeClearFlag(BX_NodeRef node, BX_NodeFlags flag);

// Dirty tracking
void BX_NodeMarkDirty(BX_NodeRef node);
void BX_NodeClearDirty(BX_NodeRef node);
BC_bool BX_NodeIsDirty(BX_NodeRef node);

// Content access
void* BX_NodeGetContent(BX_NodeRef node);
void BX_NodeSetContent(BX_NodeRef node, void* content, size_t size);

#endif //BCOMPOSE_NODE_H
