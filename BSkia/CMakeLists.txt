cmake_minimum_required(VERSION 3.21)
project(BSkiaProject C CXX)

set(CMAKE_CXX_STANDARD 20)

include(cmake/Skia.cmake)

# BSkia library
add_library(BSkia STATIC
		src/BSkia.cpp
		src/BSkia.h
)

# Link Skia privately (internal dependency)
target_link_libraries(BSkia PRIVATE skia::skia msvcrtd)

# Make BSkia headers available to consumers
target_include_directories(BSkia PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Test executable
add_executable(BSkiaTest
		test/test_bskia.c
)

target_link_libraries(BSkiaTest PRIVATE BSkia)

# Set output directory for the test
set_target_properties(BSkiaTest PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)