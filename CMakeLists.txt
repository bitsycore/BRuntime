cmake_minimum_required(VERSION 3.28)
project(BC C)

set(CMAKE_C_STANDARD 17)

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
)
