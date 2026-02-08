cmake_minimum_required(VERSION 3.28)
project(BRuntime C)

set(CMAKE_C_STANDARD 23)

if (CMAKE_CXX_COMPILER)
endif()

add_subdirectory(BCore)
add_subdirectory(BFramework)

add_subdirectory(BSkia)
add_subdirectory(BXCompose)

add_subdirectory(BXComposeTest)
add_subdirectory(BTest)