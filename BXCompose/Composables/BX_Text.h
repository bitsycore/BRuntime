#ifndef BCOMPOSE_TEXT_H
#define BCOMPOSE_TEXT_H

#include "../BX_Types.h"
#include "../Core/BX_Node.h"

// =========================================================
// MARK: Text Composable
// =========================================================

// Create a text node with the given string
// The string is copied into the node's content
BX_NodeRef BX_Text(const char* text);

// Create a text node with formatted string (printf-style)
BX_NodeRef BX_TextFormat(const char* format, ...);

// =========================================================
// MARK: Text Style
// =========================================================

typedef struct BX_TextStyle {
    float fontSize;
    BX_Color color;
    BC_bool bold;
    BC_bool italic;
} BX_TextStyle;

#define BX_TEXT_STYLE_DEFAULT ((BX_TextStyle){ \
    .fontSize = 14.0f, \
    .color = BX_COLOR_WHITE, \
    .bold = BC_false, \
    .italic = BC_false, \
})

// Apply text style to current text node
void BX_TextSetStyle(BX_TextStyle style);

#endif //BCOMPOSE_TEXT_H
