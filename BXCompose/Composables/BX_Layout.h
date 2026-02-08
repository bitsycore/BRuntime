#ifndef BCOMPOSE_LAYOUT_H
#define BCOMPOSE_LAYOUT_H

#include "../BX_Types.h"
#include "../Core/BX_Scope.h"

// =========================================================
// MARK: Layout Modifiers
// =========================================================

typedef struct BX_LayoutModifier {
    BX_Size minSize;
    BX_Size maxSize;
    BX_Size preferredSize;
    BX_EdgeInsets padding;
    BX_EdgeInsets margin;
    float weight;       // For flex layouts
    BC_bool fillMaxWidth;
    BC_bool fillMaxHeight;
} BX_LayoutModifier;

// Default modifier
#define BX_LAYOUT_DEFAULT ((BX_LayoutModifier){ \
    .minSize = {0, 0}, \
    .maxSize = {INFINITY, INFINITY}, \
    .preferredSize = {-1, -1}, \
    .padding = {0, 0, 0, 0}, \
    .margin = {0, 0, 0, 0}, \
    .weight = 0.0f, \
    .fillMaxWidth = BC_false, \
    .fillMaxHeight = BC_false, \
})

// =========================================================
// MARK: Layout Functions
// =========================================================

// Apply modifier to current node
void BX_ApplyModifier(BX_LayoutModifier modifier);

// Convenience modifiers
void BX_Padding(float all);
void BX_PaddingSymmetric(float horizontal, float vertical);
void BX_PaddingEdges(float top, float right, float bottom, float left);
void BX_Margin(float all);
void BX_SetSize(float width, float height);
void BX_FillMaxSize(void);
void BX_FillMaxWidth(void);
void BX_FillMaxHeight(void);
void BX_Weight(float weight);

// =========================================================
// MARK: Layout Calculation
// =========================================================

// Calculate layout for a subtree
void BX_LayoutCalculate(BX_NodeRef root, BX_Size availableSize);

#endif //BCOMPOSE_LAYOUT_H
