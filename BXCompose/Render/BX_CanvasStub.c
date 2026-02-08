// BX_CanvasStub.c - Stub implementation when Skia is not available
// This file is included when BX_USE_SKIA is OFF
// The actual implementation is in BX_Canvas.c using SDL renderer

// This file exists only to satisfy CMake when Skia is disabled
// All functionality is provided by BX_Canvas.c using SDL

// When Skia is enabled, BX_Skia.cpp will provide the proper implementation
// and BX_Canvas.c will use Skia calls instead of SDL

#include "../BX_Types.h"

// Placeholder - no actual code needed as BX_Canvas.c provides the SDL fallback
