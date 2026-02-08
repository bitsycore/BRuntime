# =========================================================
# BCompose - Compose-like UI Framework
# =========================================================

cmake_minimum_required(VERSION 3.28)

# =========================================================
# Dependencies: SDL3
# =========================================================

include(FetchContent)

# SDL3 - Graphics and input handling
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        main
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SDL3)

# =========================================================
# Dependencies: Skia (via pre-built binaries or system)
# =========================================================

# Option 1: Use system-installed Skia
# find_package(skia CONFIG QUIET)

# Option 2: Use pre-built Skia (download from skia.org/docs/user/download)
# For now, we'll create a minimal C binding that can work with stubs
# until Skia is properly configured

option(BX_USE_SKIA "Enable Skia rendering backend" OFF)

if(BX_USE_SKIA)
    # Try to find Skia
    find_path(SKIA_INCLUDE_DIR "include/core/SkCanvas.h"
        PATHS
            "$ENV{SKIA_DIR}"
            "C:/skia"
            "/usr/local/skia"
    )

    find_library(SKIA_LIBRARY
        NAMES skia
        PATHS
            "$ENV{SKIA_DIR}/out/Release"
            "$ENV{SKIA_DIR}/out/Static"
            "C:/skia/out/Release"
            "/usr/local/skia/out/Release"
    )

    if(SKIA_INCLUDE_DIR AND SKIA_LIBRARY)
        message(STATUS "Found Skia: ${SKIA_LIBRARY}")
        set(BX_HAS_SKIA TRUE)
    else()
        message(WARNING "Skia not found. Using stub renderer.")
        set(BX_HAS_SKIA FALSE)
    endif()
else()
    set(BX_HAS_SKIA FALSE)
endif()

# =========================================================
# BCompose Library Sources
# =========================================================

set(BCOMPOSE_SOURCES
    # Core
    BX.c
    BX.h
    BX_Settings.h
    BX_Types.h

    # Composition Core
    Core/BX_Context.c
    Core/BX_Context.h
    Core/BX_Node.c
    Core/BX_Node.h
    Core/BX_Scope.h

    # State Management
    State/BX_Remember.c
    State/BX_Remember.h
    State/BX_MutableState.c
    State/BX_MutableState.h

    # Rendering
    Render/BX_Window.c
    Render/BX_Window.h
    Render/BX_Canvas.c
    Render/BX_Canvas.h
)

# Add Skia bindings if available
if(BX_HAS_SKIA)
    list(APPEND BCOMPOSE_SOURCES
        Render/BX_Skia.cpp
        Render/BX_Skia.h
    )
endif()

# Add stub renderer if Skia not available
if(NOT BX_HAS_SKIA)
    list(APPEND BCOMPOSE_SOURCES
        Render/BX_CanvasStub.c
    )
endif()

# Composable widgets
list(APPEND BCOMPOSE_SOURCES
    Composables/BX_Layout.c
    Composables/BX_Layout.h
    Composables/BX_Text.c
    Composables/BX_Text.h
)

# Create static library
add_library(BCompose STATIC ${BCOMPOSE_SOURCES})

# =========================================================
# Include Directories
# =========================================================

target_include_directories(BCompose
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# =========================================================
# Link Dependencies
# =========================================================

target_link_libraries(BCompose
    PUBLIC
        BCore
        BFramework
        SDL3::SDL3-static
)

if(BX_HAS_SKIA)
    target_include_directories(BCompose PRIVATE ${SKIA_INCLUDE_DIR})
    target_link_libraries(BCompose PRIVATE ${SKIA_LIBRARY})
    target_compile_definitions(BCompose PRIVATE BX_HAS_SKIA=1)
endif()

# =========================================================
# Compile Definitions
# =========================================================

target_compile_definitions(BCompose PRIVATE
    $<$<CONFIG:Debug>:BX_SETTINGS_DEBUG=1>
    $<$<CONFIG:Release>:BX_SETTINGS_DEBUG=0>
)

# =========================================================
# C++ Standard (for Skia bindings if used)
# =========================================================

if(BX_HAS_SKIA)
    set_target_properties(BCompose PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
    )
endif()
