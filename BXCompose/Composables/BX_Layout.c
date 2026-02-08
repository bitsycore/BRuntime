#include "BX_Layout.h"

#include "../Core/BX_Node.h"
#include "../Core/BX_Context.h"

#include <math.h>

// =========================================================
// MARK: Modifier Application
// =========================================================

void BX_ApplyModifier(BX_LayoutModifier modifier) {
    BX_NodeRef node = BX_CurrentNode();
    if (!node) return;

    node->padding = modifier.padding;
    node->margin = modifier.margin;

    // Store other modifier data in node content
    // For now, simplified - just apply padding/margin
}

void BX_Padding(float all) {
    BX_NodeRef node = BX_CurrentNode();
    if (node) {
        node->padding = (BX_EdgeInsets){ all, all, all, all };
    }
}

void BX_PaddingSymmetric(float horizontal, float vertical) {
    BX_NodeRef node = BX_CurrentNode();
    if (node) {
        node->padding = (BX_EdgeInsets){ vertical, horizontal, vertical, horizontal };
    }
}

void BX_PaddingEdges(float top, float right, float bottom, float left) {
    BX_NodeRef node = BX_CurrentNode();
    if (node) {
        node->padding = (BX_EdgeInsets){ top, right, bottom, left };
    }
}

void BX_Margin(float all) {
    BX_NodeRef node = BX_CurrentNode();
    if (node) {
        node->margin = (BX_EdgeInsets){ all, all, all, all };
    }
}

void BX_SetSize(float width, float height) {
    BX_NodeRef node = BX_CurrentNode();
    if (node) {
        node->bounds.width = width;
        node->bounds.height = height;
    }
}

void BX_FillMaxSize(void) {
    // Will be used during layout calculation
    // For now, just mark the node
}

void BX_FillMaxWidth(void) {
    // Will be used during layout calculation
}

void BX_FillMaxHeight(void) {
    // Will be used during layout calculation
}

void BX_Weight(float weight) {
    (void)weight;
    // Store weight for flex layout
}

// =========================================================
// MARK: Layout Calculation
// =========================================================

static void PRIV_MeasureNode(BX_NodeRef node, BX_Size availableSize);
static void PRIV_PositionChildren(BX_NodeRef node);

void BX_LayoutCalculate(BX_NodeRef root, BX_Size availableSize) {
    if (!root) return;

    // Set root bounds
    root->bounds.x = 0;
    root->bounds.y = 0;
    root->bounds.width = availableSize.width;
    root->bounds.height = availableSize.height;

    // Measure and position
    PRIV_MeasureNode(root, availableSize);
    PRIV_PositionChildren(root);
}

static void PRIV_MeasureNode(BX_NodeRef node, BX_Size availableSize) {
    if (!node) return;

    BX_NodeType type = BX_NodeGetType(node);

    // Calculate content size based on children
    float contentWidth = 0;
    float contentHeight = 0;

    BX_NodeRef child = BX_NodeGetFirstChild(node);

    switch (type) {
        case BX_NODE_TYPE_COLUMN: {
            // Column: stack children vertically
            while (child) {
                PRIV_MeasureNode(child, availableSize);
                BX_Rect childBounds = BX_NodeGetBounds(child);

                contentHeight += childBounds.height + child->margin.top + child->margin.bottom;
                float childTotalWidth = childBounds.width + child->margin.left + child->margin.right;
                if (childTotalWidth > contentWidth) {
                    contentWidth = childTotalWidth;
                }

                child = BX_NodeGetNextSibling(child);
            }
            break;
        }

        case BX_NODE_TYPE_ROW: {
            // Row: stack children horizontally
            while (child) {
                PRIV_MeasureNode(child, availableSize);
                BX_Rect childBounds = BX_NodeGetBounds(child);

                contentWidth += childBounds.width + child->margin.left + child->margin.right;
                float childTotalHeight = childBounds.height + child->margin.top + child->margin.bottom;
                if (childTotalHeight > contentHeight) {
                    contentHeight = childTotalHeight;
                }

                child = BX_NodeGetNextSibling(child);
            }
            break;
        }

        case BX_NODE_TYPE_BOX:
        default: {
            // Box: children overlap, take max size
            while (child) {
                PRIV_MeasureNode(child, availableSize);
                BX_Rect childBounds = BX_NodeGetBounds(child);

                float childTotalWidth = childBounds.width + child->margin.left + child->margin.right;
                float childTotalHeight = childBounds.height + child->margin.top + child->margin.bottom;

                if (childTotalWidth > contentWidth) contentWidth = childTotalWidth;
                if (childTotalHeight > contentHeight) contentHeight = childTotalHeight;

                child = BX_NodeGetNextSibling(child);
            }
            break;
        }
    }

    // Add padding
    float totalWidth = contentWidth + node->padding.left + node->padding.right;
    float totalHeight = contentHeight + node->padding.top + node->padding.bottom;

    // If node already has explicit size, use that
    if (node->bounds.width <= 0) {
        node->bounds.width = totalWidth;
    }
    if (node->bounds.height <= 0) {
        node->bounds.height = totalHeight;
    }
}

static void PRIV_PositionChildren(BX_NodeRef node) {
    if (!node) return;

    BX_NodeType type = BX_NodeGetType(node);
    BX_NodeRef child = BX_NodeGetFirstChild(node);

    float offsetX = node->bounds.x + node->padding.left;
    float offsetY = node->bounds.y + node->padding.top;

    while (child) {
        child->bounds.x = offsetX + child->margin.left;
        child->bounds.y = offsetY + child->margin.top;

        // Recursively position children
        PRIV_PositionChildren(child);

        // Move offset based on layout type
        switch (type) {
            case BX_NODE_TYPE_COLUMN:
                offsetY += child->bounds.height + child->margin.top + child->margin.bottom;
                break;

            case BX_NODE_TYPE_ROW:
                offsetX += child->bounds.width + child->margin.left + child->margin.right;
                break;

            default:
                // Box: children overlay, don't advance
                break;
        }

        child = BX_NodeGetNextSibling(child);
    }
}
