cmake_minimum_required(VERSION 3.28)
project(BC C)

set(CMAKE_C_STANDARD 23)

add_executable(
		BCRuntime
		main.c
		BC/BCObject.c
		BC/BCObject.h
		BC/BCTypes.h
		BC/BCString.c
		BC/BCString.h
		BC/BCDictionary.c
		BC/BCDictionary.h
		BC/BCAutoreleasePool.c
		BC/BCAutoreleasePool.h
		BC/BCArray.c
		BC/BCArray.h
		BC/BCNumber.c
		BC/BCNumber.h
		BC/BC.h
		BC/BC.c
		BC/BCMacroTools.h
)

target_compile_options(BCRuntime PRIVATE -fblocks)