/**
 * BSkia Test - Simple Hello World
 *
 * This program tests that BSkia can successfully:
 * 1. Link with Skia
 * 2. Create a canvas
 * 3. Render text
 * 4. Save to PNG
 */
#include <stdio.h>
#include <stdlib.h>
#include "BSkia.h"

int main(int argc, char* argv[]) {
    printf("===========================================\n");
    printf("BSkia Test - Hello World\n");
    printf("===========================================\n\n");

    // Print version
    printf("Version: %s\n\n", BSkia_GetVersion());

    // Output filename
    const char* filename = "hello_skia.png";
    if (argc > 1) {
        filename = argv[1];
    }

    // Create test image
    printf("Creating PNG with text...\n");
    const bool success = BSkia_CreateTextPNG(filename, 800, 400, "Hello Skia from C!");

    if (success) {
        printf("\n✅ SUCCESS! Image created: %s\n", filename);
        printf("Open the file to see the rendered text.\n");
        return EXIT_SUCCESS;
    } else {
        printf("\n❌ FAILED! Could not create image.\n");
        return EXIT_FAILURE;
    }
}

