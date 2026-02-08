# ==============================================================================
# Skia.cmake - CMake module for building Google Skia graphics library
# ==============================================================================
#
# Purpose:
#   Downloads, configures, and builds the Skia graphics library from source
#   using Google's depot_tools (GN + Ninja build system).
#
# Requirements:
#   - depot_tools must be installed and available in PATH or DEPOT_TOOLS_PATH
#   - Python 3 (or Python 2 as fallback)
#   - Git (for syncing dependencies)
#   - CMake 3.21+
#
# Environment Variables:
#   DEPOT_TOOLS_PATH - Optional path to depot_tools directory
#
# Provides:
#   - skia::skia - Imported static library target
#
# ==============================================================================

include(FetchContent)
include(ExternalProject)

# Detect build type at configure time
if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_CONFIGURATION_TYPES MATCHES "Debug")
	set(SKIA_IS_DEBUG "true")
	set(SKIA_IS_OFFICIAL "false")
	set(SKIA_BUILD_TYPE "Debug")
	message(STATUS "Skia: Configuring for Debug build (_ITERATOR_DEBUG_LEVEL=2)")
else()
	set(SKIA_IS_DEBUG "false")
	set(SKIA_IS_OFFICIAL "true")
	set(SKIA_BUILD_TYPE "Release")
	message(STATUS "Skia: Configuring for Release build (_ITERATOR_DEBUG_LEVEL=0)")
endif()

# ------------------------------------------------------------------------------
# 1. Find Required Tools
# ------------------------------------------------------------------------------

# Find Python (prefer python3, fallback to python)
find_program(PYTHON_EXECUTABLE NAMES python3 python)
if(NOT PYTHON_EXECUTABLE)
	message(FATAL_ERROR
			"Python not found. Please install Python 3 and ensure it's in PATH.\n"
			"Download from: https://www.python.org/downloads/")
endif()
message(STATUS "Found Python: ${PYTHON_EXECUTABLE}")

# Find GN (from depot_tools)
if(DEFINED ENV{DEPOT_TOOLS_PATH})
	set(DEPOT_TOOLS_HINT "$ENV{DEPOT_TOOLS_PATH}")
	message(STATUS "Using DEPOT_TOOLS_PATH from environment: ${DEPOT_TOOLS_HINT}")
endif()

find_program(GN_EXECUTABLE NAMES gn gn.bat HINTS ${DEPOT_TOOLS_HINT})
if(NOT GN_EXECUTABLE)
	message(FATAL_ERROR
			"GN not found. Please install depot_tools and add to PATH.\n"
			"Instructions: https://skia.org/docs/user/build/\n"
			"Set DEPOT_TOOLS_PATH environment variable if not in PATH.")
endif()
message(STATUS "Found GN: ${GN_EXECUTABLE}")

# Find Ninja
find_program(NINJA_EXECUTABLE NAMES ninja ninja.exe HINTS ${DEPOT_TOOLS_HINT})
if(NOT NINJA_EXECUTABLE)
	message(FATAL_ERROR
			"Ninja not found. Please install depot_tools or Ninja and add to PATH.\n"
			"Download from: https://ninja-build.org/ or use depot_tools.")
endif()
message(STATUS "Found Ninja: ${NINJA_EXECUTABLE}")

# ------------------------------------------------------------------------------
# 2. Download Skia Source
# ------------------------------------------------------------------------------

# Pin to a stable milestone-based tag (m122 is a stable branch)
# You can update this to m123, m124, etc. for newer versions
FetchContent_Declare(
		skia_src
		GIT_REPOSITORY https://skia.googlesource.com/skia.git
		GIT_TAG        chrome/m122
		GIT_SHALLOW    FALSE  # Skia needs full history for some tools
)

message(STATUS "Fetching Skia source (this may take a while)...")
FetchContent_MakeAvailable(skia_src)

set(SKIA_SOURCE_DIR ${skia_src_SOURCE_DIR})
set(SKIA_BUILD_DIR  ${CMAKE_BINARY_DIR}/_deps/skia-build)

# ------------------------------------------------------------------------------
# 3. Determine Platform-Specific Library Extension
# ------------------------------------------------------------------------------

if(MSVC OR (WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "MSVC|Clang"))
	set(SKIA_LIB_EXT "lib")
	set(SKIA_LIBRARY "${SKIA_BUILD_DIR}/skia.lib")
else()
	set(SKIA_LIB_EXT "a")
	set(SKIA_LIBRARY "${SKIA_BUILD_DIR}/libskia.a")
endif()

message(STATUS "Skia library will be: ${SKIA_LIBRARY}")

# ------------------------------------------------------------------------------
# 4. Configure GN Build Arguments
# ------------------------------------------------------------------------------

set(GN_ARGS
		"is_debug=${SKIA_IS_DEBUG}"
		"is_official_build=${SKIA_IS_OFFICIAL}"
		"skia_use_system_libjpeg_turbo=false"
		"skia_use_system_libpng=false"
		"skia_use_system_zlib=false"
		"skia_use_system_expat=false"
		"skia_use_system_icu=false"
		"skia_use_system_harfbuzz=false"
		"skia_use_libwebp_decode=false"
		"skia_use_libwebp_encode=false"
		"skia_enable_skottie=false"
		"skia_enable_svg=false"
		"skia_enable_pdf=false"
		"skia_enable_skshaper=true"
		"skia_enable_skparagraph=false"
		"skia_enable_gpu=true"
		"skia_use_gl=true"
		"skia_use_angle=false"
		"skia_use_vulkan=false"
		"skia_use_metal=false"
		"skia_use_direct3d=false"
		"skia_enable_tools=false"
)

# Add build-type specific flags
if(SKIA_BUILD_TYPE STREQUAL "Debug")
	list(APPEND GN_ARGS
			"extra_cflags=[\"/MDd\", \"/D_ITERATOR_DEBUG_LEVEL=2\"]"
			"extra_cflags_cc=[\"/std:c++17\", \"/MDd\", \"/D_ITERATOR_DEBUG_LEVEL=2\"]"
	)
else()
	list(APPEND GN_ARGS
			"extra_cflags=[\"/MD\", \"/D_ITERATOR_DEBUG_LEVEL=0\"]"
			"extra_cflags_cc=[\"/std:c++17\", \"/MD\", \"/D_ITERATOR_DEBUG_LEVEL=0\"]"
	)
endif()

# Add Windows-specific arguments
if(WIN32)
	list(APPEND GN_ARGS
			"target_os=\"win\""
			"target_cpu=\"x64\""
			"clang_win=\"\""  # Use system compiler
	)
endif()

# Convert list to space-separated string
string(REPLACE ";" " " GN_ARGS_STR "${GN_ARGS}")

# ------------------------------------------------------------------------------
# 5. Build Skia with ExternalProject
# ------------------------------------------------------------------------------

ExternalProject_Add(SkiaBuild
		SOURCE_DIR ${SKIA_SOURCE_DIR}

		# Configure: sync dependencies and generate build files
		CONFIGURE_COMMAND
		${PYTHON_EXECUTABLE} tools/git-sync-deps
		COMMAND ${GN_EXECUTABLE} gen ${SKIA_BUILD_DIR} --args=${GN_ARGS_STR}

		# Build: compile with Ninja
		BUILD_COMMAND
		${NINJA_EXECUTABLE} -C ${SKIA_BUILD_DIR} skia

		# Specify outputs for proper dependency tracking
		BUILD_BYPRODUCTS ${SKIA_LIBRARY}

		# No install step needed
		INSTALL_COMMAND ""

		# Build in source directory
		BUILD_IN_SOURCE TRUE
)

# ------------------------------------------------------------------------------
# 6. Create Imported Library Target
# ------------------------------------------------------------------------------

add_library(skia STATIC IMPORTED GLOBAL)

set_target_properties(skia PROPERTIES
		IMPORTED_LOCATION ${SKIA_LIBRARY}
		INTERFACE_INCLUDE_DIRECTORIES
		"${SKIA_SOURCE_DIR};${SKIA_SOURCE_DIR}/include;${SKIA_SOURCE_DIR}/modules"
)

# Ensure skia target waits for build to complete
add_dependencies(skia SkiaBuild)

# Create namespaced alias for modern CMake usage
add_library(skia::skia ALIAS skia)

add_custom_target(SkiaClean
		COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/_deps/skia-build
		COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/_deps/skia_src-build
		COMMENT "Cleaning Skia build cache"
)

message(STATUS "Skia configuration complete")
