cmake_minimum_required(VERSION 3.28)
project(BCRuntime C)

set(CMAKE_C_STANDARD 23)

if (CMAKE_CXX_COMPILER)
endif()

add_subdirectory(BCRuntime)

add_executable(
		TestBCRuntime
		main.c
)

target_link_libraries(TestBCRuntime BCRuntime)