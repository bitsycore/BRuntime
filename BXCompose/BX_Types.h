#ifndef BCOMPOSE_TYPES_H
#define BCOMPOSE_TYPES_H

#include "BFramework/BF_Types.h"
#include "BCore/BC_Types.h"

#include <stdint.h>
#include <stdbool.h>

// =========================================================
// MARK: Core Types
// =========================================================

typedef struct BX_Context* BX_ContextRef;
typedef struct BX_Node* BX_NodeRef;
typedef struct BX_MutableState* BX_StateRef;

// =========================================================
// MARK: Geometry Types
// =========================================================

typedef struct BX_Size {
    float width;
    float height;
} BX_Size;

typedef struct BX_Offset {
    float x;
    float y;
} BX_Offset;

typedef struct BX_Rect {
    float x;
    float y;
    float width;
    float height;
} BX_Rect;

typedef struct BX_EdgeInsets {
    float top;
    float right;
    float bottom;
    float left;
} BX_EdgeInsets;

// =========================================================
// MARK: Color
// =========================================================

typedef struct BX_Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} BX_Color;

#define BX_ColorRGB(r, g, b) ((BX_Color){(r), (g), (b), 255})
#define BX_ColorRGBA(r, g, b, a) ((BX_Color){(r), (g), (b), (a)})
#define BX_ColorHex(hex) ((BX_Color){ \
    .r = (uint8_t)(((hex) >> 16) & 0xFF), \
    .g = (uint8_t)(((hex) >> 8) & 0xFF), \
    .b = (uint8_t)((hex) & 0xFF), \
    .a = 255 \
})

// Common colors
#define BX_COLOR_BLACK   BX_ColorRGB(0, 0, 0)
#define BX_COLOR_WHITE   BX_ColorRGB(255, 255, 255)
#define BX_COLOR_RED     BX_ColorRGB(255, 0, 0)
#define BX_COLOR_GREEN   BX_ColorRGB(0, 255, 0)
#define BX_COLOR_BLUE    BX_ColorRGB(0, 0, 255)

// =========================================================
// MARK: Node Types
// =========================================================

typedef enum BX_NodeType {
    BX_NODE_TYPE_ROOT = 0,
    BX_NODE_TYPE_BOX,
    BX_NODE_TYPE_COLUMN,
    BX_NODE_TYPE_ROW,
    BX_NODE_TYPE_TEXT,
    BX_NODE_TYPE_BUTTON,
    BX_NODE_TYPE_CUSTOM,
} BX_NodeType;

// =========================================================
// MARK: Callback Types
// =========================================================

typedef void (*BX_ComposeFunc)(void* userData);
typedef void (*BX_DrawFunc)(BX_NodeRef node, void* canvas);
typedef void (*BX_EventHandler)(BX_NodeRef node, void* event);

#endif //BCOMPOSE_TYPES_H
