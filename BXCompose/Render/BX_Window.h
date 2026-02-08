#ifndef BCOMPOSE_WINDOW_H
#define BCOMPOSE_WINDOW_H

#include "../BX_Types.h"

// =========================================================
// MARK: Window Types
// =========================================================

typedef struct BX_Window* BX_WindowRef;

typedef struct BX_WindowConfig {
    const char* title;
    int width;
    int height;
    BC_bool resizable;
    BC_bool fullscreen;
} BX_WindowConfig;

// =========================================================
// MARK: Window API
// =========================================================

// Create a window with the given configuration
BX_WindowRef BX_WindowCreate(const BX_WindowConfig* config);

// Create a window with default configuration
BX_WindowRef BX_WindowCreateSimple(const char* title, int width, int height);

// Destroy a window
void BX_WindowDestroy(BX_WindowRef window);

// Get window dimensions
void BX_WindowGetSize(BX_WindowRef window, int* width, int* height);

// Run the main loop with a compose function
// The compose function is called every frame
void BX_WindowRun(BX_WindowRef window, BX_ComposeFunc composeFunc, void* userData);

// Request window to close
void BX_WindowClose(BX_WindowRef window);

// Check if window should close
BC_bool BX_WindowShouldClose(BX_WindowRef window);

#endif //BCOMPOSE_WINDOW_H
