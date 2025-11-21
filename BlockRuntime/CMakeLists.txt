cmake_minimum_required(VERSION 3.22)

set(CMAKE_C_STANDARD 17)

add_library(
        BlockRuntime SHARED
        Block.h
        Block_private.h
        data.c
        runtime.c
        config.h
)