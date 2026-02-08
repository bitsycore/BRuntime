#include "BX_Canvas.h"

#include "BCore/Memory/BC_Memory.h"

#include <SDL3/SDL.h>

// =========================================================
// MARK: Canvas Structure (SDL Renderer Backend)
// =========================================================

struct BX_Canvas {
    SDL_Renderer* renderer;
    int width;
    int height;
};

// =========================================================
// MARK: Canvas Lifecycle
// =========================================================

BX_CanvasRef BX_CanvasCreate(void* renderer, int width, int height) {
    if (!renderer) return NULL;

    BX_CanvasRef canvas = BC_Calloc(1, sizeof(struct BX_Canvas));
    if (!canvas) return NULL;

    canvas->renderer = (SDL_Renderer*)renderer;
    canvas->width = width;
    canvas->height = height;

    return canvas;
}

void BX_CanvasDestroy(BX_CanvasRef canvas) {
    if (canvas) {
        BC_Free(canvas);
    }
}

// =========================================================
// MARK: Frame Management
// =========================================================

void BX_CanvasBegin(BX_CanvasRef canvas) {
    // Nothing needed for SDL renderer
    (void)canvas;
}

void BX_CanvasEnd(BX_CanvasRef canvas) {
    // Nothing needed for SDL renderer - present is done by window
    (void)canvas;
}

void BX_CanvasClear(BX_CanvasRef canvas, BX_Color color) {
    if (!canvas) return;

    SDL_SetRenderDrawColor(canvas->renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(canvas->renderer);
}

// =========================================================
// MARK: Drawing Primitives
// =========================================================

void BX_CanvasDrawRect(BX_CanvasRef canvas, BX_Rect rect, BX_Color color) {
    if (!canvas) return;

    SDL_SetRenderDrawColor(canvas->renderer, color.r, color.g, color.b, color.a);
    SDL_FRect sdlRect = { rect.x, rect.y, rect.width, rect.height };
    SDL_RenderFillRect(canvas->renderer, &sdlRect);
}

void BX_CanvasDrawRectOutline(BX_CanvasRef canvas, BX_Rect rect, BX_Color color, float strokeWidth) {
    if (!canvas) return;
    (void)strokeWidth; // SDL doesn't support stroke width for outline

    SDL_SetRenderDrawColor(canvas->renderer, color.r, color.g, color.b, color.a);
    SDL_FRect sdlRect = { rect.x, rect.y, rect.width, rect.height };
    SDL_RenderRect(canvas->renderer, &sdlRect);
}

void BX_CanvasDrawRoundedRect(BX_CanvasRef canvas, BX_Rect rect, float radius, BX_Color color) {
    // SDL doesn't have built-in rounded rect - fall back to regular rect
    // When Skia is enabled, this will use proper rounded rects
    (void)radius;
    BX_CanvasDrawRect(canvas, rect, color);
}

void BX_CanvasDrawCircle(BX_CanvasRef canvas, float cx, float cy, float radius, BX_Color color) {
    if (!canvas) return;

    // SDL doesn't have built-in circle drawing
    // Simple approximation using points
    SDL_SetRenderDrawColor(canvas->renderer, color.r, color.g, color.b, color.a);

    const int segments = 32;
    for (int i = 0; i < segments; i++) {
        float angle1 = (float)i / segments * 2.0f * 3.14159f;
        float angle2 = (float)(i + 1) / segments * 2.0f * 3.14159f;

        float x1 = cx + radius * SDL_cosf(angle1);
        float y1 = cy + radius * SDL_sinf(angle1);
        float x2 = cx + radius * SDL_cosf(angle2);
        float y2 = cy + radius * SDL_sinf(angle2);

        SDL_RenderLine(canvas->renderer, x1, y1, x2, y2);
    }
}

void BX_CanvasDrawLine(BX_CanvasRef canvas, float x1, float y1, float x2, float y2, BX_Color color, float strokeWidth) {
    if (!canvas) return;
    (void)strokeWidth; // SDL doesn't support line width

    SDL_SetRenderDrawColor(canvas->renderer, color.r, color.g, color.b, color.a);
    SDL_RenderLine(canvas->renderer, x1, y1, x2, y2);
}

void BX_CanvasDrawText(BX_CanvasRef canvas, const char* text, float x, float y, BX_Color color) {
    if (!canvas || !text) return;

    // SDL3 doesn't have built-in text rendering
    // When Skia is enabled, this will use proper text rendering
    // For now, just draw a placeholder rectangle
    (void)color;

    // Draw a placeholder box to indicate where text would be
    SDL_SetRenderDrawColor(canvas->renderer, color.r, color.g, color.b, 128);

    // Estimate text size (very rough)
    size_t len = SDL_strlen(text);
    SDL_FRect textRect = { x, y, (float)(len * 8), 16.0f };
    SDL_RenderRect(canvas->renderer, &textRect);
}

// =========================================================
// MARK: Canvas State
// =========================================================

void BX_CanvasSave(BX_CanvasRef canvas) {
    // SDL renderer doesn't have save/restore state
    // When Skia is enabled, this will work properly
    (void)canvas;
}

void BX_CanvasRestore(BX_CanvasRef canvas) {
    (void)canvas;
}

void BX_CanvasTranslate(BX_CanvasRef canvas, float dx, float dy) {
    (void)canvas;
    (void)dx;
    (void)dy;
    // Would need to track transform matrix manually for SDL
}

void BX_CanvasClipRect(BX_CanvasRef canvas, BX_Rect rect) {
    if (!canvas) return;

    SDL_Rect sdlRect = {
        (int)rect.x,
        (int)rect.y,
        (int)rect.width,
        (int)rect.height
    };
    SDL_SetRenderClipRect(canvas->renderer, &sdlRect);
}
