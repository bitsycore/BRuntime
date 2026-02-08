#ifndef BCOMPOSE_CANVAS_H
#define BCOMPOSE_CANVAS_H

#include "../BX_Types.h"

// =========================================================
// MARK: Canvas Types
// =========================================================

typedef struct BX_Canvas* BX_CanvasRef;

// =========================================================
// MARK: Canvas API
// =========================================================

// Create a canvas (internally creates Skia surface or uses SDL renderer)
BX_CanvasRef BX_CanvasCreate(void* renderer, int width, int height);
void BX_CanvasDestroy(BX_CanvasRef canvas);

// Frame management
void BX_CanvasBegin(BX_CanvasRef canvas);
void BX_CanvasEnd(BX_CanvasRef canvas);
void BX_CanvasClear(BX_CanvasRef canvas, BX_Color color);

// Drawing primitives
void BX_CanvasDrawRect(BX_CanvasRef canvas, BX_Rect rect, BX_Color color);
void BX_CanvasDrawRectOutline(BX_CanvasRef canvas, BX_Rect rect, BX_Color color, float strokeWidth);
void BX_CanvasDrawRoundedRect(BX_CanvasRef canvas, BX_Rect rect, float radius, BX_Color color);
void BX_CanvasDrawCircle(BX_CanvasRef canvas, float cx, float cy, float radius, BX_Color color);
void BX_CanvasDrawLine(BX_CanvasRef canvas, float x1, float y1, float x2, float y2, BX_Color color, float strokeWidth);
void BX_CanvasDrawText(BX_CanvasRef canvas, const char* text, float x, float y, BX_Color color);

// State
void BX_CanvasSave(BX_CanvasRef canvas);
void BX_CanvasRestore(BX_CanvasRef canvas);
void BX_CanvasTranslate(BX_CanvasRef canvas, float dx, float dy);
void BX_CanvasClipRect(BX_CanvasRef canvas, BX_Rect rect);

#endif //BCOMPOSE_CANVAS_H
