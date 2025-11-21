cmake_minimum_required(VERSION 3.28)
project(BCRuntime C)

set(CMAKE_C_STANDARD 23)

add_executable(
		BCRuntime
		main.c
		BCRuntime/BCObject.c
		BCRuntime/BCObject.h
		BCRuntime/BCTypes.h
		BCRuntime/BCString.c
		BCRuntime/BCString.h
		BCRuntime/BCDictionary.c
		BCRuntime/BCDictionary.h
		BCRuntime/BCAutoreleasePool.c
		BCRuntime/BCAutoreleasePool.h
		BCRuntime/BCArray.c
		BCRuntime/BCArray.h
		BCRuntime/BCNumber.c
		BCRuntime/BCNumber.h
		BCRuntime/BCRuntime.h
		BCRuntime/BCRuntime.c
		BCRuntime/BCMacroTools.h
)