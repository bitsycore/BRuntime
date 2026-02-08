#include "BX_Window.h"
#include "BX_Canvas.h"

#include "../Core/BX_Context.h"
#include "../Core/BX_Node.h"

#include "BCore/Memory/BC_Memory.h"

#include <SDL3/SDL.h>

// =========================================================
// MARK: Window Structure
// =========================================================

struct BX_Window {
    SDL_Window* sdlWindow;
    SDL_Renderer* sdlRenderer;
    BX_CanvasRef canvas;
    int width;
    int height;
    BC_bool shouldClose;
};

// =========================================================
// MARK: Window Creation
// =========================================================

BX_WindowRef BX_WindowCreate(const BX_WindowConfig* config) {
    if (!config) return NULL;

    BX_WindowRef window = BC_Calloc(1, sizeof(struct BX_Window));
    if (!window) return NULL;

    // Set window flags
    SDL_WindowFlags flags = 0;
    if (config->resizable) {
        flags |= SDL_WINDOW_RESIZABLE;
    }
    if (config->fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN;
    }

    // Create SDL window
    window->sdlWindow = SDL_CreateWindow(
        config->title ? config->title : "BCompose",
        config->width > 0 ? config->width : 800,
        config->height > 0 ? config->height : 600,
        flags
    );

    if (!window->sdlWindow) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        BC_Free(window);
        return NULL;
    }

    // Create SDL renderer
    window->sdlRenderer = SDL_CreateRenderer(window->sdlWindow, NULL);
    if (!window->sdlRenderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window->sdlWindow);
        BC_Free(window);
        return NULL;
    }

    // Store dimensions
    SDL_GetWindowSize(window->sdlWindow, &window->width, &window->height);

    // Create canvas for drawing
    window->canvas = BX_CanvasCreate(window->sdlRenderer, window->width, window->height);

    window->shouldClose = BC_false;

    return window;
}

BX_WindowRef BX_WindowCreateSimple(const char* title, int width, int height) {
    BX_WindowConfig config = {
        .title = title,
        .width = width,
        .height = height,
        .resizable = BC_true,
        .fullscreen = BC_false,
    };
    return BX_WindowCreate(&config);
}

void BX_WindowDestroy(BX_WindowRef window) {
    if (!window) return;

    if (window->canvas) {
        BX_CanvasDestroy(window->canvas);
    }

    if (window->sdlRenderer) {
        SDL_DestroyRenderer(window->sdlRenderer);
    }

    if (window->sdlWindow) {
        SDL_DestroyWindow(window->sdlWindow);
    }

    BC_Free(window);
}

// =========================================================
// MARK: Window Properties
// =========================================================

void BX_WindowGetSize(BX_WindowRef window, int* width, int* height) {
    if (!window) {
        if (width) *width = 0;
        if (height) *height = 0;
        return;
    }

    if (width) *width = window->width;
    if (height) *height = window->height;
}

void BX_WindowClose(BX_WindowRef window) {
    if (window) {
        window->shouldClose = BC_true;
    }
}

BC_bool BX_WindowShouldClose(BX_WindowRef window) {
    return window ? window->shouldClose : BC_true;
}

// =========================================================
// MARK: Main Loop
// =========================================================

// Forward declaration for rendering
static void PRIV_RenderNode(BX_CanvasRef canvas, BX_NodeRef node);

void BX_WindowRun(BX_WindowRef window, BX_ComposeFunc composeFunc, void* userData) {
    if (!window || !composeFunc) return;

    SDL_Event event;

    while (!window->shouldClose) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    window->shouldClose = BC_true;
                    break;

                case SDL_EVENT_WINDOW_RESIZED:
                    window->width = event.window.data1;
                    window->height = event.window.data2;
                    // Recreate canvas for new size
                    if (window->canvas) {
                        BX_CanvasDestroy(window->canvas);
                    }
                    window->canvas = BX_CanvasCreate(window->sdlRenderer, window->width, window->height);
                    break;

                default:
                    break;
            }
        }

        // Run composition
        BX_ContextPush();
        composeFunc(userData);

        // Get the composed tree
        BX_ContextRef ctx = BX_GetCurrentContext();
        BX_NodeRef root = BX_ContextGetRoot(ctx);

        // Clear and render
        BX_CanvasBegin(window->canvas);
        BX_CanvasClear(window->canvas, BX_ColorRGB(30, 30, 30));

        if (root) {
            PRIV_RenderNode(window->canvas, root);
        }

        BX_CanvasEnd(window->canvas);
    	BX_ContextMarkComposed(ctx);
        // Pop context (will reset arena)
        BX_ContextPop();

        // Present
        SDL_RenderPresent(window->sdlRenderer);

        // Simple frame rate limit (~60 FPS)
        SDL_Delay(16);
    }
}

// =========================================================
// MARK: Rendering
// =========================================================

static void PRIV_RenderNode(BX_CanvasRef canvas, BX_NodeRef node) {
    if (!node) return;

    // Render this node based on type
    BX_NodeType type = BX_NodeGetType(node);
    BX_Rect bounds = BX_NodeGetBounds(node);

    switch (type) {
        case BX_NODE_TYPE_BOX:
            BX_CanvasDrawRect(canvas, bounds, BX_ColorRGB(60, 60, 80));
            break;

        case BX_NODE_TYPE_COLUMN:
        case BX_NODE_TYPE_ROW:
            // Layout containers - just draw children
            break;

        case BX_NODE_TYPE_TEXT: {
            const char* text = (const char*)BX_NodeGetContent(node);
            if (text) {
                BX_CanvasDrawText(canvas, text, bounds.x, bounds.y, BX_COLOR_WHITE);
            }
            break;
        }

        default:
            break;
    }

    // Render children
    BX_NodeRef child = BX_NodeGetFirstChild(node);
    while (child) {
        PRIV_RenderNode(canvas, child);
        child = BX_NodeGetNextSibling(child);
    }
}
