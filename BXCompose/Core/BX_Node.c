#include "BX_Node.h"
#include "BX_Context.h"

#include "BCore/Memory/BC_Arena.h"
#include "BCore/Memory/BC_Memory.h"

#include <string.h>

// =========================================================
// MARK: Node Creation
// =========================================================

BX_NodeRef BX_NodeCreate(BX_NodeType type, uint32_t key) {
    return BX_NodeCreateWithContent(type, key, NULL, 0);
}

BX_NodeRef BX_NodeCreateWithContent(BX_NodeType type, uint32_t key, void* content, size_t contentSize) {
    BX_ContextRef ctx = BX_GetCurrentContext();
    if (!ctx) return NULL;

    BC_ArenaRef arena = BX_ContextGetFrameArena(ctx);
    BC_AllocatorRef allocator = BC_ArenaAllocator(arena);

    // Allocate node from frame arena
    BX_NodeRef node = BC_AllocatorAlloc(allocator, sizeof(struct BX_Node));
    if (!node) return NULL;

    // Initialize node
    memset(node, 0, sizeof(struct BX_Node));
    node->type = type;
    node->key = key;
    node->flags = BX_NODE_FLAG_VISIBLE | BX_NODE_FLAG_ENABLED | BX_NODE_FLAG_DIRTY;

    // Copy content if provided
    if (content && contentSize > 0) {
        node->content = BC_AllocatorAlloc(allocator, contentSize);
        if (node->content) {
            memcpy(node->content, content, contentSize);
            node->contentSize = contentSize;
        }
    }

    return node;
}

// =========================================================
// MARK: Tree Manipulation
// =========================================================

void BX_NodeAddChild(BX_NodeRef parent, BX_NodeRef child) {
    if (!parent || !child) return;

    // Remove from current parent if any
    if (child->parent) {
        BX_NodeRemoveFromParent(child);
    }

    child->parent = parent;
    child->nextSibling = NULL;

    if (!parent->firstChild) {
        parent->firstChild = child;
        parent->lastChild = child;
    } else {
        parent->lastChild->nextSibling = child;
        parent->lastChild = child;
    }
}

void BX_NodeRemoveFromParent(BX_NodeRef node) {
    if (!node || !node->parent) return;

    BX_NodeRef parent = node->parent;

    // Find and remove from sibling list
    if (parent->firstChild == node) {
        parent->firstChild = node->nextSibling;
        if (parent->lastChild == node) {
            parent->lastChild = NULL;
        }
    } else {
        BX_NodeRef prev = parent->firstChild;
        while (prev && prev->nextSibling != node) {
            prev = prev->nextSibling;
        }
        if (prev) {
            prev->nextSibling = node->nextSibling;
            if (parent->lastChild == node) {
                parent->lastChild = prev;
            }
        }
    }

    node->parent = NULL;
    node->nextSibling = NULL;
}

// =========================================================
// MARK: Traversal
// =========================================================

BX_NodeRef BX_NodeGetParent(BX_NodeRef node) {
    return node ? node->parent : NULL;
}

BX_NodeRef BX_NodeGetFirstChild(BX_NodeRef node) {
    return node ? node->firstChild : NULL;
}

BX_NodeRef BX_NodeGetNextSibling(BX_NodeRef node) {
    return node ? node->nextSibling : NULL;
}

// =========================================================
// MARK: Properties
// =========================================================

BX_NodeType BX_NodeGetType(BX_NodeRef node) {
    return node ? node->type : BX_NODE_TYPE_ROOT;
}

uint32_t BX_NodeGetKey(BX_NodeRef node) {
    return node ? node->key : 0;
}

void BX_NodeSetBounds(BX_NodeRef node, BX_Rect bounds) {
    if (node) {
        node->bounds = bounds;
    }
}

BX_Rect BX_NodeGetBounds(BX_NodeRef node) {
    if (node) {
        return node->bounds;
    }
    return (BX_Rect){0, 0, 0, 0};
}

// =========================================================
// MARK: Flags
// =========================================================

BC_bool BX_NodeHasFlag(BX_NodeRef node, BX_NodeFlags flag) {
    return node ? (node->flags & flag) != 0 : BC_false;
}

void BX_NodeSetFlag(BX_NodeRef node, BX_NodeFlags flag) {
    if (node) {
        node->flags |= flag;
    }
}

void BX_NodeClearFlag(BX_NodeRef node, BX_NodeFlags flag) {
    if (node) {
        node->flags &= ~flag;
    }
}

// =========================================================
// MARK: Dirty Tracking
// =========================================================

void BX_NodeMarkDirty(BX_NodeRef node) {
    if (!node) return;

    // Mark this node and propagate up to ancestors
    while (node) {
        if (node->flags & BX_NODE_FLAG_DIRTY) {
            break; // Already dirty, ancestors must be too
        }
        node->flags |= BX_NODE_FLAG_DIRTY;
        node = node->parent;
    }
}

void BX_NodeClearDirty(BX_NodeRef node) {
    if (node) {
        node->flags &= ~BX_NODE_FLAG_DIRTY;
    }
}

BC_bool BX_NodeIsDirty(BX_NodeRef node) {
    return node ? (node->flags & BX_NODE_FLAG_DIRTY) != 0 : BC_false;
}

// =========================================================
// MARK: Content Access
// =========================================================

void* BX_NodeGetContent(BX_NodeRef node) {
    return node ? node->content : NULL;
}

void BX_NodeSetContent(BX_NodeRef node, void* content, size_t size) {
    if (!node) return;

    BX_ContextRef ctx = BX_GetCurrentContext();
    if (!ctx) return;

    BC_ArenaRef arena = BX_ContextGetFrameArena(ctx);
    BC_AllocatorRef allocator = BC_ArenaAllocator(arena);

    // Allocate and copy new content
    if (content && size > 0) {
        node->content = BC_AllocatorAlloc(allocator, size);
        if (node->content) {
            memcpy(node->content, content, size);
            node->contentSize = size;
        }
    } else {
        node->content = NULL;
        node->contentSize = 0;
    }
}
