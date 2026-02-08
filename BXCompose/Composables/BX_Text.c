#include "BX_Text.h"

#include "../Core/BX_Context.h"
#include "../Core/BX_Scope.h"

#include "BCore/Memory/BC_Arena.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// =========================================================
// MARK: Text Content Storage
// =========================================================

typedef struct BX_TextContent {
    BX_TextStyle style;
    char text[];  // Flexible array member
} BX_TextContent;

// =========================================================
// MARK: Text Creation
// =========================================================

BX_NodeRef BX_Text(const char* text) {
    if (!text) return NULL;

    BX_ContextRef ctx = BX_GetCurrentContext();
    if (!ctx) return NULL;

    BX_NodeRef parent = BX_ContextGetCurrentNode(ctx);

    // Create text node
    BX_NodeRef node = BX_NodeCreate(BX_NODE_TYPE_TEXT, 0);
    if (!node) return NULL;

    // Add to parent
    if (parent) {
        BX_NodeAddChild(parent, node);
    }

    // Copy text content using arena
    BC_ArenaRef arena = BX_ContextGetFrameArena(ctx);
    BC_AllocatorRef allocator = BC_ArenaAllocator(arena);

    size_t textLen = strlen(text);
    size_t contentSize = sizeof(BX_TextContent) + textLen + 1;

    BX_TextContent* content = BC_AllocatorAlloc(allocator, contentSize);
    if (content) {
        content->style = BX_TEXT_STYLE_DEFAULT;
        memcpy(content->text, text, textLen + 1);

        node->content = content;
        node->contentSize = contentSize;
    }

    // Set estimated bounds based on text length
    // Rough estimate: 8 pixels per character width, 16 pixels height
    node->bounds.width = (float)(textLen * 8);
    node->bounds.height = 16.0f;

    return node;
}

BX_NodeRef BX_TextFormat(const char* format, ...) {
    if (!format) return NULL;

    BX_ContextRef ctx = BX_GetCurrentContext();
    if (!ctx) return NULL;

    // Format the string
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    return BX_Text(buffer);
}

// =========================================================
// MARK: Text Style
// =========================================================

void BX_TextSetStyle(BX_TextStyle style) {
    BX_NodeRef node = BX_CurrentNode();
    if (!node || BX_NodeGetType(node) != BX_NODE_TYPE_TEXT) return;

    BX_TextContent* content = (BX_TextContent*)node->content;
    if (content) {
        content->style = style;
    }
}
