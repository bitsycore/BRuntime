#include "BX.h"

#include <SDL3/SDL.h>

// =========================================================
// MARK: Runtime Initialization
// =========================================================

void BX_Initialize(void) {
    // Initialize SDL3
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return;
    }

    // Initialize BCompose subsystems
    INTERNAL_BX_ContextInitialize();
}

void BX_Deinitialize(void) {
    // Deinitialize BCompose subsystems
    INTERNAL_BX_ContextDeinitialize();

    // Quit SDL3
    SDL_Quit();
}
