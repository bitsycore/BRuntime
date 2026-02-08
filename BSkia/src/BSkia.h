//
// Created by Doge on 08/02/2026.
//

#ifndef BRUNTIME_BSKIA_H
#define BRUNTIME_BSKIA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/**
 * Simple test function to verify Skia integration.
 * Creates a PNG image with text.
 *
 * @param filename Path to output PNG file (e.g., "hello.png")
 * @param width Image width in pixels
 * @param height Image height in pixels
 * @param text Text to render
 * @return true on success, false on failure
 */
bool BSkia_CreateTextPNG(const char* filename, int width, int height, const char* text);

/**
 * Get Skia version string for testing
 * @return Version string
 */
const char* BSkia_GetVersion(void);

#ifdef __cplusplus
}
#endif

#endif // BRUNTIME_BSKIA_H
