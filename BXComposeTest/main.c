/**
 * BComposeTest - Hello World Demo
 *
 * Demonstrates the BCompose declarative UI framework:
 * - Composable scope macros (BX_Compose, BX_Column, BX_Row)
 * - State management (BX_State, BX_Remember)
 * - SDL3 window and rendering
 */

#include <BXCompose/BX.h>
#include <BFramework/BF.h>

#include <stdio.h>

// =========================================================
// MARK: Demo Compose Function
// =========================================================

static int gClickCount = 0;

void DemoCompose(void* userData) {
    (void)userData;

    // Root column layout
    BX_Column() {
        BX_Padding(20);
    	BX_SetSize(15, 15);

        // Title text
        BX_Text("Hello BCompose!");

        // Spacer
        BX_Box() {
            BX_SetSize(0, 20);
        }

        // Content row
        BX_Row() {
            BX_Padding(10);

            BX_Text("This is a declarative UI in C");
        }

        // Counter demo
        BX_Box() {
            BX_SetSize(0, 20);
        }

        BX_Column() {
            BX_Padding(10);

            // Using remember to persist value
            const auto counter = BX_RememberPtr(int, 0);

            // Display counter
            //BX_TextFormat("Frame counter: %d", *counter);
            //(*counter)++;

            // Show click count
            BX_TextFormat("Clicks: %d (click window to increment)", gClickCount);
        }
    }
}

// =========================================================
// MARK: Main Entry Point
// =========================================================

int main(int argc, char* argv[]) {
    // Initialize BFramework (for memory, autorelease, etc.)
    BF_Initialize(argc, argv);

    // Initialize BCompose (SDL3)
    BX_Initialize();

    printf("===========================================\n");
    printf("  BCompose Hello World Demo\n");
    printf("===========================================\n");
    printf("Press ESC or close window to exit.\n\n");

    // Create window
    BX_WindowRef window = BX_WindowCreateSimple("BCompose Hello World", 800, 600);
    if (!window) {
        fprintf(stderr, "Failed to create window!\n");
        BX_Deinitialize();
        BF_Deinitialize();
        return 1;
    }

    // Run main loop
    BX_WindowRun(window, DemoCompose, NULL);

    // Cleanup
    BX_WindowDestroy(window);
    BX_Deinitialize();
    BF_Deinitialize();

    printf("\nBCompose demo finished.\n");
    return 0;
}
